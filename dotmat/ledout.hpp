#pragma once
//=====================================================================//
/*!	@file
	@breif	LED dot matrix 出力
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <stdint.h>

namespace device {

	class ledout {

		static const uint16_t fb_width_  = 16;
		static const uint16_t fb_height_ = 16;

		uint8_t		fb_[fb_width_ * fb_height_ / 8];
		uint8_t		count_;
		volatile uint16_t	frame_;

	public:
		ledout() : count_(0), frame_(0) { }

		void copy(const uint8_t* fb) {
			uint8_t* p = fb_;
			for(uint8_t i = 0; i < (fb_width_ * fb_height_ / 8); ++i) {
				*p++ = *fb++;
			}
		}

		void reflesh() const {
			uint8_t hi = ~fb_[(count_ << 1)];
			uint8_t lo = ~fb_[(count_ << 1) + 1];
			for(uint8_t i = 0; i < 8; ++i) {
				PORTB &= ~0b00000011;
				if(lo & 1) { PORTB |= 0x01; }
				if(hi & 1) { PORTB |= 0x02; }
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
		}

		void count() {
			++count_;
			if(count_ >= 16) {
				count_ = 0;
				++frame_;
			}
		}

		uint16_t get_frame() const { return frame_; }

		void sync() const {
			volatile uint16_t cnt;
			cnt = frame_;
			while(frame_ == cnt) ;
		}
	};
}
