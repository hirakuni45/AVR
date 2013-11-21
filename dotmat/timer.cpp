//=====================================================================//
/*!	@file
	@brief	タイマー・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "timer.hpp"
#include "menu.hpp"

namespace app {

	uint32_t timer::get_time_()
	{
		uint8_t tmp[4];
		tmp[0] = 0;
		twi_.send(tmp, 1);	// set read address
		twi_.recv(&tmp[0], 1);
		twi_.recv(&tmp[1], 1);
		twi_.recv(&tmp[2], 1);
		twi_.recv(&tmp[3], 1);

		uint32_t t = tmp[0] | (static_cast<uint32_t>(tmp[1]) << 8)
			| (static_cast<uint32_t>(tmp[2]) << 16)
			| (static_cast<uint32_t>(tmp[3]) << 24);
		return t;
	}

	void timer::draw_date_(int16_t ofs, uint32_t cnt)
	{
///		uint32_t day = cnt / (60L * 60L * 12L);
	}

	void timer::draw_time_(int16_t ofs, uint32_t cnt)
	{
		uint32_t hor = cnt / 60;
		uint32_t min = hor % 60;
		task_.at_draw().draw_3x5( 9, ofs + 0, min / 10);
		task_.at_draw().draw_3x5(13, ofs + 0, min % 10);

		hor /= 60;
		hor %= 24;
		task_.at_draw().draw_3x5( 0, ofs + 0, hor / 10);
		task_.at_draw().draw_3x5( 4, ofs + 0, hor % 10);

		uint32_t sec = cnt % 60;
		task_.at_draw().draw_7x10(0, ofs + 6, sec / 10);
		task_.at_draw().draw_7x10(9, ofs + 6, sec % 10);
	}



	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void timer::init()
	{
		twi_.init();

		uint8_t tmp[2];
		tmp[0] = 0x07;
		tmp[1] = 0x06;
		twi_.send(tmp, 2);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void timer::service()
	{
		time_ = get_time_();

		draw_time_(0, time_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void timer::destroy()
	{
	}

}

