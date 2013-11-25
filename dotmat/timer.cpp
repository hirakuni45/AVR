//=====================================================================//
/*!	@file
	@brief	タイマー・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "timer.hpp"
#include "menu.hpp"

namespace app {

	static const char week_str_[] PROGMEM = {
		'S', 'U', 'N',
		'M', 'O', 'N',
		'T', 'U', 'E',
		'W', 'E', 'D',
		'T', 'H', 'U',
		'F', 'R', 'I',
		'S', 'A', 'T'
	};


	void timer::draw_year_(int16_t ofs, const tm* t)
	{
		uint16_t y = t->tm_year + 1900;
		task_.at_draw().draw_3x5(13, ofs + 5, y % 10);
		y /= 10;
		task_.at_draw().draw_3x5( 9, ofs + 5, y % 10);
		y /= 10;
		task_.at_draw().draw_3x5( 5, ofs + 5, y % 10);
		y /= 10;
		task_.at_draw().draw_3x5( 1, ofs + 5, y % 10);
	}


	void timer::draw_date_(int16_t ofs, const tm* t)
	{
		uint8_t m = t->tm_mon + 1;
		if(m / 10) task_.at_draw().draw_3x5( 0, ofs + 2, m / 10);
		task_.at_draw().draw_3x5( 4, ofs + 2, m % 10);

		if(t->tm_mday / 10) task_.at_draw().draw_3x5( 9, ofs + 2, t->tm_mday / 10);
		task_.at_draw().draw_3x5(13, ofs + 2, t->tm_mday % 10);

		// 曜日
		short x = 0;
		const char* p = &week_str_[t->tm_wday * 3];
		for(uint8_t i = 0; i < 3; ++i) {
			char ch = pgm_read_byte_near(p);
			++p;
			x += task_.at_draw().draw_xx5(x, ofs + 9, ch);
			x += 1;
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


	void timer::display_()
	{
		time_ = task_.at_rtc().read();

		const system::switch_input& swi = task_.at_switch();
		int8_t no = page_;
		if(swi.get_positive() & system::switch_input::bits::LEFT_UP) {
			if(page_ < 2) {
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

		draw_year_((pos_ >> 16) +  0, t);
		draw_date_((pos_ >> 16) + 16, t);
		draw_time_((pos_ >> 16) + 32, t);

		// 下ボタン同時押しで、戻る
		if(swi.get_level() == (system::switch_input::bits::LEFT_DOWN
			| system::switch_input::bits::RIGHT_DOWN)) {
			mode_ = mode::ret_menu;
			task_.at_music().request(sound::music::id::tetris_rot, 1);
		}
		// 全ボタン同時押しが４秒続く場合
		if(swi.get_level() == (system::switch_input::bits::LEFT_UP
			| system::switch_input::bits::RIGHT_UP
			| system::switch_input::bits::LEFT_DOWN
			| system::switch_input::bits::RIGHT_DOWN)) {
			++set_count_;
			if(set_count_ >= 240) {
				mode_ = mode::setting_y;
				set_pos_ = 0;
			}
		} else {
			set_count_ = 0;
		}

	}


	// 年設定
	void timer::setting_y_()
	{
		tm* t = get_tm();
		draw_year_(0, t);
	}


	// 月日設定
	void timer::setting_md_()
	{
		tm* t = get_tm();
		draw_date_(0, t);
	}


	// 時間設定
	void timer::setting_t_()
	{
		tm* t = get_tm();
		draw_time_(0, t);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void timer::init()
	{
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void timer::service()
	{
		if(mode_ == mode::display) {
			display_();
		} else if(mode_ == mode::setting_y) {
			setting_y_();
		} else if(mode_ == mode::setting_md) {
			setting_md_();
		} else if(mode_ == mode::setting_t) {
			setting_t_();
		} else if(mode_ == mode::ret_menu) {
			// ボタンが両方離されたら戻る
			const system::switch_input& swi = task_.at_switch();
			if(!(swi.get_level() & system::switch_input::bits::LEFT_DOWN)) {
				if(!(swi.get_level() & system::switch_input::bits::RIGHT_DOWN)) {
					task_.start<menu>();
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
	void timer::destroy()
	{
	}

}

