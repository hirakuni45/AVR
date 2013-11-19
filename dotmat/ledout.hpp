#pragma once
//=====================================================================//
/*!	@file
	@brief	LED dot matrix 出力
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <stdint.h>

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	LED 出力クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class ledout {

		static const uint16_t fb_width_  = 16;
		static const uint16_t fb_height_ = 16;

		uint8_t		fb_[fb_width_ * fb_height_ / 8];
		uint8_t		count_;
		volatile uint16_t	frame_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		ledout() : count_(0), frame_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	コピー
			@param[in]	fb	フレームバッファ
		*/
		//-----------------------------------------------------------------//
		void copy(const uint8_t* fb) {
			uint8_t* p = fb_;
			for(uint8_t i = 0; i < (fb_width_ * fb_height_ / 8); ++i) {
				*p++ = *fb++;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	LED 表示を許可
			@param[in]	f	不許可なら「false」
		*/
		//-----------------------------------------------------------------//
		void enable(bool f = true) {
			// Y-Decoder enable.
			if(f) PORTD &= ~0b00000100;
			else PORTD |=  0b00000100;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ダイナミック・スキャンのリフレッシュ
		*/
		//-----------------------------------------------------------------//
		void reflesh() {
			uint8_t hi = ~fb_[(count_ << 1)];
			uint8_t lo = ~fb_[(count_ << 1) + 1];
			for(uint8_t i = 0; i < 8; ++i) {
				PORTB &= ~0b00000001;
				PORTD &= ~0b00000001;
				if(lo & 1) { PORTB |= 0x01; }
				if(hi & 1) { PORTD |= 0x01; }
				lo >>= 1;
				PORTB |=  0b00000100;	// CLK=H
		 		hi >>= 1;
				PORTB &= ~0b00000100;	// CLK=L
			}

			uint8_t a = PORTC;
			a &= 0xf0;
			PORTB &= ~0b00001000;	// /STB=L
			PORTC = a | (count_ & 0xf);
			PORTB |=  0b00001000;	// /STB=H

			++count_;
			if(count_ >= 16) {
				count_ = 0;
				++frame_;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フレームの取得
			@return フレーム
		*/
		//-----------------------------------------------------------------//
		uint16_t get_frame() const { return frame_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フレームの同期
		*/
		//-----------------------------------------------------------------//
		void sync() const {
			volatile uint16_t cnt;
			cnt = frame_;
			while(frame_ == cnt) ;
		}
	};
}