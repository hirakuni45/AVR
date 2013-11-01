//=====================================================================//
/*!	@file
	@breif	パルス・サウンド関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "psound.hpp"

// 1.059463094 : 2 ^ (1 / 12)

// octave-0
// ( 0) C    : 32.70319566257483

// octave-3 (center)
// ( 0) C    : 261.63 Hz		9393
// ( 1) C#/Db: 277.18 Hz		8866
// ( 2) D    : 293.66 Hz		8369
// ( 3) D#/Eb: 311.13 Hz		7899
// ( 4) E    : 329.63 Hz		7456
// ( 5) F    : 349.23 Hz		7037
// ( 6) F#/Gb: 369.99 Hz		6642
// ( 7) G    : 392.00 Hz		6269
// ( 8) G#/Ab: 415.30 Hz		5918
// ( 9) A    : 440.00 Hz		5585
// (10) A#/Bb: 466.16 Hz		5272
// (11) B    : 493.88 Hz		4976

namespace device {

	static const uint16_t sound12_para_[] PROGMEM = {
		9393, 8866, 8369, 7899, 7456, 7037, 6642, 6269, 5918, 5585, 5272, 4976
	};

	void psound::disable_()
	{
		TCCR1B = 0b00000000;
		TCCR1A = 0b00000000;
		TCCR1C = 0b00000000;
		enable_ = false;
	}

	static void enable_dev_(uint8_t divide)
	{
		TCCR1A = 0b01000000;
		TCCR1B = 0b00001000 | (divide & 7);
		TCCR1C = 0b10000000;	// FOC1A
	}

#ifdef PSOUND_VOLUME_ENABLE
	static void set_volume_(uint8_t vol)
	{
		OCR0B = vol;
	}
#endif

	//-----------------------------------------------------------------//
	/*!
		@breif	パルスサウンド初期化
	*/
	//-----------------------------------------------------------------//
	void psound::init()
	{
#ifdef PSOUND_VOLUME_ENABLE
// Timer0: 8 ビットタイマー設定 (高速PWM）
		OCR0A  = 0;
		OCR0B  = 0;
		TCCR0A = 0b00100011;	// 高速 PWM 出力 OC0B (PD5)
		TCCR0B = 0b00000001;	// 1/1 clock プリスケーラ－
		DDRD  |= 0b00100000;	// PD5 output

		master_volume_ = 0;
#endif
	// 16 ビットタイマー設定
#if 1
		TCNT1  = 0;

		TCCR1A = 0b00000000;	// OC1A toggle (Compare Match)
		TCCR1B = 0b00000000;	// 一致クリア(CTC)、プリスケーラー(1/8)
								// 000: No clock source(Timer/Counter stopped)
								// 001: 1/1
								// 010: 1/8
								// 011: 1/64
								// 100: 1/256
								// 101: 1/1024
		// output port
		OCR1A = 0;
		DDRB |= _BV(1);			// OC1A
#endif
		disable_();

		length_ = 0;
		count_ = 0;
		music_player_ = 0;
	}


	static void set_frq_(uint8_t index)
	{
		uint16_t v = pgm_read_word_near(&sound12_para_[index % 12]);
		uint16_t oct = index / 12;
		OCR1A = (v >> oct) - 1;
		TCNT1  = 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@breif	パルスサウンドサービス(1/100 で呼ぶ）
	*/
	//-----------------------------------------------------------------//
	void psound::service()
	{
#ifdef PSOUND_VOLUME_ENABLE
		// ボリューム・フェード
		if(fader_speed_ != 0) {
			int16_t cnt = master_volume_;
			cnt += fader_speed_;
			if(cnt > 255) cnt = 255;
			else if(cnt < 0) cnt = 0;
			master_volume_ = cnt;
		}

		uint16_t vol = master_volume_;
		vol *= static_cast<uint16_t>(envelope_) + 1;
		set_volume_(vol >> 8);

		if(envelope_) {
			int16_t tmp = envelope_;
			tmp -= 25;
			if(tmp < 0) tmp = 0;
			envelope_ = tmp; 
		}
#endif
		if(!enable_) return;

		if(count_ < length_) {
			if(count_ == 0) {
				enable_dev_(0b010);	// 1/8 divider
				set_frq_(index_);
			}
			++count_;
		} else {
			if(music_player_) {
				const prog_uint8_t* p = music_player_;
				uint8_t cmd = pgm_read_byte_near(p);
				if(cmd) {
					++p;
					uint8_t len = pgm_read_byte_near(p);
					++p;
					request(cmd - 1, len);
					music_player_ = p;
					return;
				} else {
					music_player_ = 0;
					disable_();
				}
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@breif	パルスサウンドリクエスト
		@param[in]	index	音階
		@param[in]	length	音長
	*/
	//-----------------------------------------------------------------//
	void psound::request(uint8_t index, uint8_t length)
	{
		enable_ = true;
		index_ = index;
		length_ = length;
		envelope_ = 255;	// attack
		count_ = 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@breif	パルスサウンド演奏
		@param[in]	music	音階と音長の組をテーブルにしたもの
	*/
	//-----------------------------------------------------------------//
	void psound::play_P(const prog_uint8_t *music)
	{
		enable_ = true;
		music_player_ = music;
	}


	//-----------------------------------------------------------------//
	/*!
		@breif	コンペアマッチに直接値を設定する。
		@param[in]	divider	ディバイダーの設定値
		@param[in]	cmpv	コンペアマッチに設定する値
	*/
	//-----------------------------------------------------------------//
	void psound::direct(uint8_t divide, uint16_t cmpv)
	{
		enable_ = false;
		enable_dev_(divide);
		OCR1A = cmpv;
		TCNT1 = 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@breif	トグル出力の周波数を設定する
		@param[in]	freq	出力周波数(Hz)下位４ビット小数点
		@return 「true」なら正常終了
	*/
	//-----------------------------------------------------------------//
	bool psound::freq(uint32_t freq)
	{
		if(freq == 0) {
			disable_();
			return false;
		}

		uint32_t b = (F_CPU << 4) / freq;
		uint8_t divi = 0;
		uint32_t a, d;
		do {
			++divi;
			if(divi == 1) d = 1;
			else if(divi == 2) d = 8;
			else if(divi == 3) d = 64;
			else if(divi == 4) d = 256;
			else if(divi == 5) d = 1024;
			else {
				disable_();
				return false;	// Over flow...
			}
			a = b / d;
		} while(a > 65536) ;

		direct(divi, a - 1);

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@breif	プリスケーラーの分周比を取得
		@return 分周比
	*/
	//-----------------------------------------------------------------//
	int16_t psound::get_divider() const
	{
		uint8_t a = TCCR1B & 0x7;
		if(a == 0) return 0;
		else if(a == 1) return 1;
		else if(a == 2) return 8;
		else if(a == 3) return 64;
		else if(a == 4) return 256;
		else if(a == 5) return 1024;

		return -1;
	}

}



