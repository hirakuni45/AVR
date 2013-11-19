//=====================================================================//
/*!	@file
	@brief	パルス・サウンド関係
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

// Master clock: 20MHz  / 8
// octave-1 (center)
// ( 0) C    :  65.41 Hz	38220
// ( 1) C#/Db:  69.30 Hz	36075
// ( 2) D    :  73.42 Hz	34051
// ( 3) D#/Eb:  77.78 Hz	32142
// ( 4) E    :  82.41 Hz	30336
// ( 5) F    :  87.31 Hz	28634
// ( 6) F#/Gb:  92.50 Hz	27027
// ( 7) G    :  98.00 Hz	25510
// ( 8) G#/Ab: 103.83 Hz	24078
// ( 9) A    : 110.00 Hz	22727
// (10) A#/Bb: 116.54 Hz	21452
// (11) B    : 123.47 Hz	20248
// ※出力はさらに半分の周波数になる

namespace device {

	static const uint16_t sound12_para_[] PROGMEM = {
		38220-1, 36075-1, 34051-1, 32142-1,
		30336-1, 28634-1, 27027-1, 25510-1,
		24078-1, 22727-1, 21452-1, 20248-1
	};


	static void disable_dev_()
	{
		TCCR1B = 0b00000000;
		TCCR1A = 0b00000000;
		TCCR1C = 0b00000000;
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
		@brief	パルスサウンド初期化
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

		music_slot_[0].volume_master_ = 0;
		music_slot_[1].volume_master_ = 0;
#endif
	// 16 ビットタイマー設定
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

		music_slot_[0].enable_ = false;
		music_slot_[0].length_ = 0;
		music_slot_[0].music_player_ = 0;
		music_slot_[0].tr_ = 0;
		music_slot_[1].enable_ = false;
		music_slot_[1].length_ = 0;
		music_slot_[1].music_player_ = 0;
		music_slot_[1].tr_ = 0;
	}


	static void set_frq_(uint8_t index)
	{
		uint16_t v = pgm_read_word_near(&sound12_para_[index % 12]);
		uint16_t oct = index / 12;
		OCR1A = (v >> oct) - 1;
		TCNT1  = 0;
	}


	void psound::service_chanel_(music_slot& ms)
	{
		// ボリューム・フェード
		if(ms.fader_speed_ != 0) {
			int16_t cnt = ms.volume_master_;
			cnt += ms.fader_speed_;
			if(cnt > 255) cnt = 255;
			else if(cnt < 0) cnt = 0;
			ms.volume_master_ = cnt;
		}

		// エンベロープカーブ生成
		if(ms.envelope_down_) {
			int16_t tmp = static_cast<int16_t>(ms.envelope_);
			if(ms.envelope_down_ && ms.length_ < ms.envelope_cmp_) {
				tmp -= static_cast<int16_t>(ms.envelope_down_);
			}
			if(tmp < 0) tmp = 0;
			else if(tmp > 255) tmp = 255;
			ms.envelope_ = tmp;
		}

		uint16_t vol = ms.volume_master_;
		vol *= static_cast<uint16_t>(ms.envelope_) + 1;
		ms.volume_reg_ = vol >> 8;

		if(ms.length_) {
			uint16_t cnt = static_cast<uint16_t>(ms.tempo_master_);
			cnt += static_cast<uint16_t>(ms.tempo_ + 1);
			if(cnt > 255) {
				--ms.length_;
			}
			ms.tempo_master_ = cnt;
			ms.index_trg_ = false;
		} else {
			const prog_uint8_t* p = ms.music_player_;
			while(p != 0) {
				uint8_t cmd = pgm_read_byte_near(p);
				++p;
				if(cmd <= sound_key::Q) {	///< 音階の設定
					uint8_t len = pgm_read_byte_near(p);
					++p;
					ms.music_player_ = p;
					ms.length_top_ = len;
					ms.length_ = len - 1;
					if(cmd < sound_key::Q) {
						int8_t n = static_cast<int8_t>(cmd) + static_cast<int8_t>(ms.tr_);
						if(n < 0) n = 0;
						else if(n >= sound_key::Q) n = sound_key::Q - 1;
						ms.index_reg_ = static_cast<uint8_t>(n);
						ms.envelope_ = 255;
					} else {
						ms.index_reg_ = sound_key::Q;
						ms.envelope_ = 0;
					}
					ms.index_trg_ = true;
// 音色の設定
ms.envelope_cmp_ = len >> 1;
ms.envelope_down_ = 10;
					return;
				} else if(cmd == sound_key::END) {
					ms.music_player_ = 0;
					ms.index_reg_ = sound_key::Q;
					ms.index_trg_ = true;
					return;
				} else if(cmd == sound_key::TEMPO) {
					ms.tempo_ = pgm_read_byte_near(p);
					++p;
				} else if(cmd == sound_key::VOLUME) {
					ms.volume_master_ = pgm_read_byte_near(p);
					++p;
				} else if(cmd == sound_key::FOR) {
					uint8_t cnt = pgm_read_byte_near(p);
					++p;
					ms.push16(reinterpret_cast<uint16_t>(p));
					ms.push(cnt);
				} else if(cmd == sound_key::BEFORE) {
					uint8_t cnt = ms.pop();
					uint16_t ptr = ms.pop16();
					--cnt;
					if(cnt) {
						p = reinterpret_cast<const prog_uint8_t*>(ptr);
						ms.push16(ptr);
						ms.push(cnt);
						continue;
					}
				} else if(cmd == sound_key::TR) {
					ms.tr_ = pgm_read_byte_near(p);
					++p;
				} else if(cmd == sound_key::TR_) {
					ms.tr_ += pgm_read_byte_near(p);
					++p;
				}
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	パルスサウンドサービス(1/100 で呼ぶ）
	*/
	//-----------------------------------------------------------------//
	void psound::service()
	{
		bool ch0 = false;
		if(music_slot_[0].music_player_) ch0 = true;
		bool ch1 = false;
		if(music_slot_[1].music_player_) ch1 = true;

		service_chanel_(music_slot_[0]);
		service_chanel_(music_slot_[1]);

		bool trg = false;
		uint8_t vol = 0;
		uint8_t idx = sound_key::Q;
		if(ch0) {
			trg = music_slot_[0].index_trg_;
			vol = music_slot_[0].volume_reg_;
			idx = music_slot_[0].index_reg_;
		}
		if(ch1) {
			trg = music_slot_[1].index_trg_;
			vol = music_slot_[1].volume_reg_;
			idx = music_slot_[1].index_reg_;
		}

		set_volume_(vol);

		if(trg) {
			if(idx < sound_key::Q) {
				set_frq_(idx);
				enable_dev_(0b010);	// 1/8 divider
			} else {
				disable_dev_();
			}
		} else if(ch0 && ch1) {
			disable_dev_();
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	パルスサウンドリクエスト（直接）
		@param[in]	index	音階
		@param[in]	length	音長
		@param[in]	chanel	チャンネル
	*/
	//-----------------------------------------------------------------//
	void psound::request(uint8_t index, uint8_t length, uint8_t chanel)
	{
		music_slot& ms = music_slot_[chanel & 1];
		ms.enable_ = true;
		ms.index_ = index;
		ms.length_top_ = length;
		ms.length_ = length - 1;
		ms.tempo_master_ = 0;
		ms.tr_ = 0;
		ms.envelope_ = 255;	// attack
		enable_dev_(0b010);	// 1/8 divider
		set_frq_(index);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	パルスサウンド演奏
		@param[in]	music	音階と音長の組をテーブルにしたもの
		@param[in]	chanel	チャンネル
	*/
	//-----------------------------------------------------------------//
	void psound::play_P(const prog_uint8_t *music, uint8_t chanel)
	{
		music_slot& ms = music_slot_[chanel & 1];

		ms.enable_ = true;
		ms.music_player_ = music;
		ms.length_ = 0;
		ms.tr_ = 0;
		ms.tempo_master_ = 0;
		ms.tempo_ = 255;			///< 初期設定テンポ（一番速い）
		ms.fader_speed_ = 0;		///< フェーダー無し		
		ms.volume_master_ = 255;	///< 初期設定マスターボリューム
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	コンペアマッチに直接値を設定する。
		@param[in]	divide	ディバイダーの設定値
		@param[in]	cmpv	コンペアマッチに設定する値
	*/
	//-----------------------------------------------------------------//
	void psound::direct(uint8_t divide, uint16_t cmpv)
	{
		music_slot_[0].enable_ = false;
		music_slot_[1].enable_ = false;
		enable_dev_(divide);
		OCR1A = cmpv;
		TCNT1 = 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	トグル出力の周波数を設定する
		@param[in]	freq	出力周波数(Hz)下位４ビット小数点
		@return 「true」なら正常終了
	*/
	//-----------------------------------------------------------------//
	bool psound::freq(uint32_t freq)
	{
		if(freq == 0) {
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
				return false;	// Over flow...
			}
			a = b / d;
		} while(a > 65536) ;

		direct(divi, a - 1);

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	プリスケーラーの分周比を取得
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



