//=====================================================================//
/*!	@file
	@brief	キッチン・タイマー・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "ktimer.hpp"
#include "menu.hpp"

namespace app {

	void ktimer::draw_time_(uint16_t cnt)
	{
		uint16_t hor = cnt / 60;
		uint16_t min = hor % 60;
		task_.at_draw().draw_3x5( 9, 0, min / 10);
		task_.at_draw().draw_3x5(13, 0, min % 10);

		hor /= 60;
		task_.at_draw().draw_3x5( 1, 0, hor % 10);
		task_.at_draw().draw_3x5( 5, 0, 10);

		uint16_t sec = cnt % 60;
		task_.at_draw().draw_7x10(0, 6, sec / 10);
		task_.at_draw().draw_7x10(9, 6, sec % 10);
	}

	void ktimer::setting_()
	{
		const system::switch_input& swi = task_.at_switch();

		if(swi.get_level() & system::switch_input::bits::LEFT_DOWN) {
			if(swi.get_level() & system::switch_input::bits::RIGHT_DOWN) {
				task_.start<menu>();
				return;
			}
		}

		bool up = false;
		bool dn = false;
		if(swi.get_level() & system::switch_input::bits::RIGHT_UP) {
			up = true;
		}
		if(swi.get_level() & system::switch_input::bits::LEFT_UP) {
			dn = true;
		}
		if((up || dn) && jump_ < 255) {
			++jump_;
		}
		if(!up && !dn) {
			jump_ = 0;
			acc_ = 0;
		}
		// 1 秒以上押し続けた場合
		uint16_t a = down_count_;
		if(jump_ >= 60) {
			if(acc_) {
				--acc_;
			} else if(down_count_ < (30 * 60)) { // 30 分以内 1 分刻み
				down_count_ /= 60;	// 秒を切り捨てる
				down_count_ *= 60;
				if(up) down_count_ += 60;
				if(dn) down_count_ -= 60;
				acc_ = 20;
			} else if(down_count_ < (60 * 60)) { // 1 時間以内 5 分刻み
				if(up) down_count_ += 5 * 60;
				if(dn) down_count_ -= 5 * 60;
				acc_ = 30; // リピートは少しづつ遅く・・
			} else { // 1 時間以上 15 分刻み
				if(up) down_count_ += 15 * 60;
				if(dn) down_count_ -= 15 * 60;
				acc_ = 40; // リピートは少しづつ遅く・・
			}
		}

		if(swi.get_positive() & system::switch_input::bits::RIGHT_UP) {
			++down_count_;
		}
		if(swi.get_positive() & system::switch_input::bits::LEFT_UP) {
			--down_count_;
		}
		if(down_count_ >= 60000) {
			down_count_ = 0;
		} else if(down_count_ >= (9 * 60 * 60)) {
			down_count_ = 9 * 60 * 60 - 1;
		}
		if(a > down_count_) {
			task_.at_music().request(sound::music::id::count_down, 1);
		} else if(a < down_count_) {
			task_.at_music().request(sound::music::id::count_up, 1);
		}

		if(swi.get_negative() & system::switch_input::bits::LEFT_DOWN) {
			down_count_ = 0;
			task_.at_music().request(sound::music::id::tetris_rot, 1);
		}
		if(swi.get_negative() & system::switch_input::bits::RIGHT_DOWN) {
			if(down_count_) {
				mode_ = mode::count;
				sec_ = 60;
				counter_ = down_count_;
				task_.at_music().request(sound::music::id::start, 1);
			}
		}

		draw_time_(down_count_);
	}

	void ktimer::count_()
	{
		const system::switch_input& swi = task_.at_switch();
		if(swi.get_positive() & system::switch_input::bits::RIGHT_DOWN) {
			pause_ = !pause_;
			task_.at_music().request(sound::music::id::tetris_rot, 1);
		}

		if(!pause_ && counter_) {
			if(sec_) {
				--sec_;
			} else {
				--counter_;
				sec_ = 60;
				if(counter_ == 0) {
					task_.at_music().request(sound::music::id::bgm_01, 0);
					mode_ = mode::blink;
					sec_ = 150;
				}
			}
		}

		// 同時押しで、戻る
		if(swi.get_level() & system::switch_input::bits::LEFT_DOWN) {
			if(swi.get_level() & system::switch_input::bits::RIGHT_DOWN) {
				mode_ = mode::sync_setting;
				task_.at_music().request(sound::music::id::tetris_rot, 1);
			}
		}

		if(!pause_) {
			int16_t n = (frame_ >> 2) & 7;
			int16_t x = n;
			int16_t y = n;
			int16_t w = 16 - n * 2;
			int16_t h = 16 - n * 2;
			if(frame_ & 1) {
				task_.at_monograph().frame(x, y, w, h, 1);
			}
		}
		++frame_;

		bool draw = true;
		if(pause_) {
			if((frame_ & 0x1f) < 10) draw = false;
		}

		if(draw) draw_time_(counter_);
	}


	void ktimer::blink_()
	{
		if(sec_) {
			--sec_;
		} else {
			mode_ = mode::setting;
		}
		if((sec_ % 12) < 5) {
			draw_time_(down_count_);
		}

		const system::switch_input& swi = task_.at_switch();
		if(swi.get_positive()) {
			mode_ = mode::setting;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void ktimer::init()
	{
		if(task_.get_share()) {
			down_count_ = static_cast<uint16_t>(task_.get_share()) * 60;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void ktimer::service()
	{
		if(mode_ == mode::setting) {
			setting_();
		} else if(mode_ == mode::count) {
			count_();
		} else if(mode_ == mode::blink) {
			blink_();
		} else if(mode_ == mode::sync_setting) {
			// ボタンが両方離されたら戻る
			const system::switch_input& swi = task_.at_switch();
			if(!(swi.get_level() & system::switch_input::bits::LEFT_DOWN)) {
				if(!(swi.get_level() & system::switch_input::bits::RIGHT_DOWN)) {
					mode_ = mode::setting;
				}
			}
			// Ｘの表示
			task_.at_monograph().line(0, 0, 15, 15, 1);
			task_.at_monograph().line(15, 0, 0, 15, 1);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void ktimer::destroy()
	{
	}

}

