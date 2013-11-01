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
		device::psound::sound_key::C + 1 + 12 * 2, 10,
		device::psound::sound_key::D + 1 + 12 * 2, 10,
		device::psound::sound_key::E + 1 + 12 * 2, 10,
		device::psound::sound_key::F + 1 + 12 * 2, 10,
		device::psound::sound_key::G + 1 + 12 * 2, 10,
		device::psound::sound_key::A + 1 + 12 * 2, 10,
		device::psound::sound_key::B + 1 + 12 * 2, 10,
		device::psound::sound_key::C + 1 + 12 * 3, 10,
		0
	};

	//-----------------------------------------------------------------//
	/*!
		@breif	初期化
	*/
	//-----------------------------------------------------------------//
	void menu::init()
	{
//		task_.at_psound().request(30, 100);
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
