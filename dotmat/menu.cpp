//=====================================================================//
/*!	@file
	@breif	メニュー関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/pgmspace.h>
#include "menu.hpp"
#include "tetris.hpp"

namespace app {

	static const uint8_t music_[] PROGMEM = {
		device::psound::sound_key::TEMPO,	190,

		device::psound::sound_key::D + 12 * 5, 28,
		device::psound::sound_key::Q,           4,
		device::psound::sound_key::D + 12 * 5, 28,
		device::psound::sound_key::Q,           4,
		device::psound::sound_key::E + 12 * 5, 28,
		device::psound::sound_key::Q,           4,
		device::psound::sound_key::Q,          16,
		device::psound::sound_key::D + 12 * 5,  8,
		device::psound::sound_key::E + 12 * 5,  8,
		device::psound::sound_key::Fs+ 12 * 5, 16,
		device::psound::sound_key::G + 12 * 5, 16,
		device::psound::sound_key::Fs+ 12 * 5, 16,
		device::psound::sound_key::E + 12 * 5,  8,
		device::psound::sound_key::D + 12 * 5, 24,
		device::psound::sound_key::Q,          32,

		device::psound::sound_key::A + 12 * 5, 28,
		device::psound::sound_key::Q,           4,
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

		device::psound::sound_key::D + 12 * 5, 28,
		device::psound::sound_key::Q,           4,
		device::psound::sound_key::D + 12 * 5, 28,
		device::psound::sound_key::Q,           4,
		device::psound::sound_key::E + 12 * 5, 28,
		device::psound::sound_key::Q,           4,
		device::psound::sound_key::Q,          16,
		device::psound::sound_key::D + 12 * 5,  8,
		device::psound::sound_key::E + 12 * 5,  8,
		device::psound::sound_key::Fs + 12 * 5, 16,
		device::psound::sound_key::G + 12 * 5, 16,
		device::psound::sound_key::Fs+ 12 * 5, 16,
		device::psound::sound_key::E + 12 * 5,  8,
		device::psound::sound_key::D + 12 * 5, 24,
		device::psound::sound_key::Q,          32,

		device::psound::sound_key::A + 12 * 5, 28,
		device::psound::sound_key::Q,           4,
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

	//-----------------------------------------------------------------//
	/*!
		@breif	初期化
	*/
	//-----------------------------------------------------------------//
	void menu::init()
	{
		task_.at_psound().play_P(music_);
		task_.at_psound().set_volume(0);
		task_.at_psound().set_fader(4);
		frame_ = 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@breif	サービス
	*/
	//-----------------------------------------------------------------//
	void menu::service()
	{
		int16_t n = (frame_ >> 2) & 7;
		int16_t x = n;
		int16_t y = n;
		int16_t w = 16 - n * 2;
		int16_t h = 16 - n * 2;
		task_.at_monograph().frame(x, y, w, h, 1);
		++frame_;
		if(frame_ >= 64) {
			task_.start<tetris>();
		}
		const system::switch_input& swi = task_.at_switch();
		if(swi.get_positive() & system::switch_input::RIGHT_DOWN) {
			task_.at_psound().play_P(music_);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@breif	廃棄
	*/
	//-----------------------------------------------------------------//
	void menu::destroy()
	{
	}

}
