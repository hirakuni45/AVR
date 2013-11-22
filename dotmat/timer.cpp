//=====================================================================//
/*!	@file
	@brief	タイマー・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "timer.hpp"
#include "menu.hpp"

namespace app {

	// RTC から時間（秒）を読み出す
	time_t timer::get_time_()
	{
		uint8_t tmp[4];
		tmp[0] = 0;
		twi_.send(tmp, 1);	// set read address
		twi_.recv(&tmp[0], 1);
		twi_.recv(&tmp[1], 1);
		twi_.recv(&tmp[2], 1);
		twi_.recv(&tmp[3], 1);

		time_t t = tmp[0] | (static_cast<time_t>(tmp[1]) << 8)
			| (static_cast<time_t>(tmp[2]) << 16)
			| (static_cast<time_t>(tmp[3]) << 24);
		return t;
	}

	void timer::draw_date_(int16_t ofs, const tm* t)
	{
		uint16_t y = t->tm_year + 1900;
		task_.at_draw().draw_3x5(13, ofs + 0, y % 10);
		y /= 10;
		task_.at_draw().draw_3x5( 9, ofs + 0, y % 10);
		y /= 10;
		task_.at_draw().draw_3x5( 5, ofs + 0, y % 10);
		y /= 10;
		task_.at_draw().draw_3x5( 1, ofs + 0, y % 10);

		++frame_;
		if(frame_ >= (60 * 15)) {
			frame_ = 0;
			dsp_date_ = !dsp_date_;
		}

		if(dsp_date_) {
			uint8_t m = t->tm_mon + 1;
			if(m / 10) task_.at_draw().draw_3x5( 0, ofs + 8, m / 10);
			task_.at_draw().draw_3x5( 4, ofs + 8, m % 10);

			if(t->tm_mday / 10) task_.at_draw().draw_3x5( 9, ofs + 8, t->tm_mday / 10);
			task_.at_draw().draw_3x5(13, ofs + 8, t->tm_mday % 10);
		} else {
			// 曜日
			task_.at_draw().draw_3x5(7, ofs + 8, t->tm_wday);
		}
	}

	void timer::draw_time_(int16_t ofs, const tm* t)
	{
		task_.at_draw().draw_3x5( 0, ofs + 0, t->tm_hour / 10);
		task_.at_draw().draw_3x5( 4, ofs + 0, t->tm_hour % 10);
		task_.at_draw().draw_3x5( 9, ofs + 0, t->tm_min / 10);
		task_.at_draw().draw_3x5(13, ofs + 0, t->tm_min % 10);

		task_.at_draw().draw_7x10(0, ofs + 6, t->tm_sec / 10);
		task_.at_draw().draw_7x10(9, ofs + 6, t->tm_sec % 10);
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
		tmp[0] = 0x07;		// DS1371 制御レジスター
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

		const system::switch_input& swi = task_.at_switch();
		int8_t no = page_;
		if(swi.get_positive() & system::switch_input::bits::LEFT_UP) {
			if(page_ < 1) {
				++page_;
				task_.at_music().request(sound::music::id::tetris_move, 1);
			}
		}
		if(swi.get_positive() & system::switch_input::bits::LEFT_DOWN) {
			if(page_) {
				--page_;
				task_.at_music().request(sound::music::id::tetris_move, 1);
			}
		}
		if(page_ != no) {
			if(page_ < no) speed_ = 80000;
			else if(page_ > no) speed_ =-80000; 
		}

		int32_t n = -page_;
		n <<= 16;
		n *= 16;
		bool mi = false;
		if(n < pos_) mi = true;
		pos_ += speed_;
		speed_ *= 1024;
		speed_ /= 1050;
		if(mi) {
			if(n >= pos_) {
				speed_ = 0;
				pos_ = n;
			}
		} else {
			if(n <= pos_) {
				speed_ = 0;
				pos_ = n;
			}
		}

		tm* t = gmtime(&time_);

		draw_date_((pos_ >> 16) +  0, t);
		draw_time_((pos_ >> 16) + 16, t);
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

