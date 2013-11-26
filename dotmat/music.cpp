//=====================================================================//
/*!	@file
	@brief	音楽、効果音関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/pgmspace.h>
#include "music.hpp"

namespace sound {

	// ストラップ（doriko）、イントロ部分
	static const uint8_t bgm_01_[] PROGMEM = {
		device::psound::sound_key::TEMPO,	210,

		device::psound::sound_key::D + 12 * 5, 32,
		device::psound::sound_key::D + 12 * 5, 32,
		device::psound::sound_key::E + 12 * 5, 32,
		device::psound::sound_key::Q,          16,
		device::psound::sound_key::D + 12 * 5,  8,
		device::psound::sound_key::E + 12 * 5,  8,
		device::psound::sound_key::Fs+ 12 * 5, 16,
		device::psound::sound_key::G + 12 * 5, 16,
		device::psound::sound_key::Fs+ 12 * 5, 16,
		device::psound::sound_key::E + 12 * 5,  8,
		device::psound::sound_key::D + 12 * 5, 24,
		device::psound::sound_key::Q,          32,

		device::psound::sound_key::A + 12 * 5, 32,
		device::psound::sound_key::A + 12 * 5, 16,
		device::psound::sound_key::D + 12 * 5, 16,
		device::psound::sound_key::E + 12 * 5, 32,
		device::psound::sound_key::Q,          16,
		device::psound::sound_key::D + 12 * 5, 16,
		device::psound::sound_key::A + 12 * 4, 16,
		device::psound::sound_key::Cs+ 12 * 5, 16,
		device::psound::sound_key::D + 12 * 5,  8,
		device::psound::sound_key::E + 12 * 5, 32,
		device::psound::sound_key::Q,          32,

		device::psound::sound_key::D + 12 * 5, 32,
		device::psound::sound_key::D + 12 * 5, 32,
		device::psound::sound_key::E + 12 * 5, 32,
		device::psound::sound_key::Q,          16,
		device::psound::sound_key::D + 12 * 5,  8,
		device::psound::sound_key::E + 12 * 5,  8,
		device::psound::sound_key::Fs+ 12 * 5, 16,
		device::psound::sound_key::G + 12 * 5, 16,
		device::psound::sound_key::Fs+ 12 * 5, 16,
		device::psound::sound_key::E + 12 * 5,  8,
		device::psound::sound_key::D + 12 * 5, 24,
		device::psound::sound_key::Q,          32,

		device::psound::sound_key::A + 12 * 5, 32,
		device::psound::sound_key::A + 12 * 5, 16,
		device::psound::sound_key::D + 12 * 5, 16,
		device::psound::sound_key::E + 12 * 5, 32,
		device::psound::sound_key::Q,          16,
		device::psound::sound_key::D + 12 * 5, 16,
		device::psound::sound_key::Cs+ 12 * 5, 16,
		device::psound::sound_key::D + 12 * 5, 16,
		device::psound::sound_key::E + 12 * 5,  8,
		device::psound::sound_key::D + 12 * 5, 24,
		device::psound::sound_key::Q,          16,

		device::psound::sound_key::Q,          255,
		device::psound::sound_key::END
	};

	/// 消去効果音
	static const uint8_t snd_erase_[] PROGMEM = {
		device::psound::sound_key::A + 12 * 4, 1,
		device::psound::sound_key::B + 12 * 6, 1,
		device::psound::sound_key::E + 12 * 4, 1,
		device::psound::sound_key::A + 12 * 4, 1,
		device::psound::sound_key::B + 12 * 6, 1,
		device::psound::sound_key::E + 12 * 4, 1,
		device::psound::sound_key::A + 12 * 4, 1,
		device::psound::sound_key::B + 12 * 6, 1,
		device::psound::sound_key::E + 12 * 4, 1,
		device::psound::sound_key::END
	};

	/// 落下効果音
	static const uint8_t snd_fall_[] PROGMEM = {
		device::psound::sound_key::A + 12 * 5, 1,
		device::psound::sound_key::B + 12 * 5, 1,
		device::psound::sound_key::D + 12 * 5, 1,
		device::psound::sound_key::E + 12 * 5, 1,
		device::psound::sound_key::END
	};

	/// 移動効果音
	static const uint8_t snd_move_[] PROGMEM = {
		device::psound::sound_key::A + 12 * 4, 1,
		device::psound::sound_key::C + 12 * 4, 1,
		device::psound::sound_key::E + 12 * 4, 1,
		device::psound::sound_key::END
	};

	/// 回転時効果音
	static const uint8_t snd_rot_[] PROGMEM = {
		device::psound::sound_key::A + 12 * 2, 1,
		device::psound::sound_key::C + 12 * 2, 1,
		device::psound::sound_key::E + 12 * 2, 1,
		device::psound::sound_key::END
	};

	/// 決定効果音
	static const uint8_t snd_collect_[] PROGMEM = {
		device::psound::sound_key::FOR, 3,
		device::psound::sound_key::A + 12 * 2, 1,
		device::psound::sound_key::B + 12 * 3, 1,
		device::psound::sound_key::E + 12 * 2, 1,
		device::psound::sound_key::A + 12 * 2, 1,
		device::psound::sound_key::B + 12 * 3, 1,
		device::psound::sound_key::E + 12 * 2, 1,
		device::psound::sound_key::A + 12 * 2, 1,
		device::psound::sound_key::B + 12 * 3, 1,
		device::psound::sound_key::E + 12 * 2, 1,
		device::psound::sound_key::TR_, 2,
		device::psound::sound_key::BEFORE,
		device::psound::sound_key::END
	};

	/// カウントアップ効果音
	static const uint8_t snd_cup_[] PROGMEM = {
		device::psound::sound_key::A  + 12 * 5, 1,
		device::psound::sound_key::Cs + 12 * 6, 1,
		device::psound::sound_key::E  + 12 * 6, 1,
		device::psound::sound_key::END
	};

	/// カウントダウン効果音
	static const uint8_t snd_cdown_[] PROGMEM = {
		device::psound::sound_key::A  + 12 * 5, 1,
		device::psound::sound_key::C  + 12 * 6, 1,
		device::psound::sound_key::Eb + 12 * 6, 1,
		device::psound::sound_key::END
	};


	/// スタート効果音
	static const uint8_t snd_start_[] PROGMEM = {
		device::psound::sound_key::FOR, 2,
		device::psound::sound_key::A  + 12 * 4, 1,
		device::psound::sound_key::B  + 12 * 4, 1,
		device::psound::sound_key::E  + 12 * 4, 1,
		device::psound::sound_key::A  + 12 * 4, 1,
		device::psound::sound_key::C  + 12 * 4, 1,
		device::psound::sound_key::E  + 12 * 4, 1,
		device::psound::sound_key::A  + 12 * 4, 1,
		device::psound::sound_key::As + 12 * 4, 1,
		device::psound::sound_key::Eb + 12 * 4, 1,
		device::psound::sound_key::TR_, 1,
		device::psound::sound_key::BEFORE,
		device::psound::sound_key::END
	};

	/// 起動効果音
	static const uint8_t snd_launch_[] PROGMEM = {
		device::psound::sound_key::FOR, 3,
		device::psound::sound_key::A  + 12 * 7, 2,
		device::psound::sound_key::Q,           1,
		device::psound::sound_key::BEFORE,
		device::psound::sound_key::END
	};


	//-----------------------------------------------------------------//
	/*!
		@brief	発音
		@param[in]	no	発音番号
		@param[in]	prio	優先順位
	*/
	//-----------------------------------------------------------------//
	void music::request(id::type no, uint8_t prio)
	{
		const uint8_t* p = 0;

		switch(no) {
		case id::tetris_erase:
			p = snd_erase_;
			break;
		case id::tetris_fall:
			p = snd_fall_;
			break;
		case id::tetris_move:
			p = snd_move_;
			break;
		case id::tetris_rot:
			p = snd_rot_;
			break;
		case id::collect:
			p = snd_collect_;
			break;
		case id::count_up:
			p = snd_cup_;
			break;
		case id::count_down:
			p = snd_cdown_;
			break;
		case id::start:
			p = snd_start_;
			break;
		case id::launch:
			p = snd_launch_;
			break;
		case id::bgm_01:
			p = bgm_01_;
			break;
		}

		if(p) {
			ps_.play_P(p, prio);
		}
	}
}

