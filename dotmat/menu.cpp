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
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void menu::service()
	{
#if 0
		if(frame_ >= 64) {
///			task_.start<tetris>();
		}
		const system::switch_input& swi = task_.at_switch();
		if(swi.get_positive() & system::switch_input::bits::RIGHT_DOWN) {
			task_.at_psound().play_P(music_);
		}

		task_.at_draw().draw_3x5(0, 0, 0);
		task_.at_draw().draw_3x5(4, 0, 1);
		task_.at_draw().draw_3x5(8, 0, 2);
		task_.at_draw().draw_3x5(12, 0, 3);

		task_.at_draw().draw_7x10(0, 6, 9);
		task_.at_draw().draw_7x10(8, 6, 1);
#endif
		if(goto_) {
			--goto_;
			if(goto_ == 0) {
				if(icon_no_ == 0) task_.start<ktimer>();
				else if(icon_no_ == 1) task_.start<tetris>();
				else if(icon_no_ == 2) task_.start<timer>();
			}
			if((goto_ % 12) < 5) {
				task_.at_draw().draw_icon(0, 0, icon_no_);
			}
			return;
		}

		const system::switch_input& swi = task_.at_switch();
		int8_t no = icon_no_;
		if(swi.get_positive() & system::switch_input::bits::RIGHT_UP) {
			if(icon_no_ < 2) {
				++icon_no_;
				task_.at_music().request(sound::music::id::tetris_move, 1);
			}
		}
		if(swi.get_positive() & system::switch_input::bits::LEFT_UP) {
			if(icon_no_) {
				--icon_no_;
				task_.at_music().request(sound::music::id::tetris_move, 1);
			}
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

		task_.at_draw().draw_icon((pos_ >> 16) + 0,  0, 0);
		task_.at_draw().draw_icon((pos_ >> 16) + 16, 0, 1);
		task_.at_draw().draw_icon((pos_ >> 16) + 32, 0, 2);

		if(swi.get_positive() & system::switch_input::bits::RIGHT_DOWN) {
			goto_ = 60;	// 1.0 sec.
			task_.at_music().request(sound::music::id::collect, 1);
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
