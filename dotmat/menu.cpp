//=====================================================================//
/*!	@file
	@brief	メニュー関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/pgmspace.h>
#include "menu.hpp"
#include "ktimer.hpp"
#include "tetris.hpp"
#include "timer.hpp"

namespace app {

	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void menu::init()
	{
		task_.music_.request(sound::music::id::launch, 1);
//		icon_no_ = task_.eeprom_.read(device::eeprom::slot::menu_pos);
//		if(icon_no_ > 3 || icon_no_ < 0) icon_no_ = 0;		
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void menu::service()
	{
		if(goto_) {
			--goto_;
			if(goto_ == 0) {
				if(icon_no_ == 0 || icon_no_ == 1) {
					if(icon_no_ == 0) task_.set_share(0);
					else task_.set_share(kt_min_);
					task_.start<ktimer>();
				} else if(icon_no_ == 2) task_.start<tetris>();
				else if(icon_no_ == 3) task_.start<timer>();
			}
			if((goto_ % 12) < 5) {
				uint8_t no = icon_no_;
				if(icon_no_ >= 1) --no;
				task_.dm_.draw_icon(0, 0, no);
				if(icon_no_ == 1) {
					task_.dm_.draw_3x5(2, 5, kt_min_);
				}
			}
			return;
		}

		const system::switch_input& swi = task_.swi_;
		int8_t no = icon_no_;
		if(swi.get_positive() & system::switch_input::bits::RIGHT_UP) {
			if(icon_no_ < 3) {
				++icon_no_;
				task_.music_.request(sound::music::id::tetris_move, 1);
			}
		}
		if(swi.get_positive() & system::switch_input::bits::LEFT_UP) {
			if(icon_no_) {
				--icon_no_;
				task_.music_.request(sound::music::id::tetris_move, 1);
			}
		}

		if(swi.last_touch(60 * 3)) {
///			task_.eeprom_.write(device::eeprom::slot::menu_pos, icon_no_);
		}

		if(icon_no_ != no) {
			if(icon_no_ < no) speed_ = 80000;
			else if(icon_no_ > no) speed_ =-80000; 
		}

		int32_t n = -icon_no_;
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

		task_.dm_.draw_icon((pos_ >> 16) + 0,  0, 0);
		task_.dm_.draw_icon((pos_ >> 16) + 16, 0, 0);
		task_.dm_.draw_icon((pos_ >> 16) + 32, 0, 1);
		task_.dm_.draw_icon((pos_ >> 16) + 48, 0, 2);

		if(icon_no_ == 1) {
			if(swi.get_positive() & system::switch_input::bits::LEFT_DOWN) {
				++kt_min_;
				task_.music_.request(sound::music::id::count_up, 1);
				if(kt_min_ > 5) {
					kt_min_ = 3;
				}
			}
		}

		task_.dm_.draw_3x5((pos_ >> 16) + 16 + 2, 5, kt_min_);

		if(swi.get_positive() & system::switch_input::bits::RIGHT_DOWN) {
			goto_ = 30;	// 0.5 sec.
			task_.music_.request(sound::music::id::collect, 1);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void menu::destroy()
	{
	}

}
