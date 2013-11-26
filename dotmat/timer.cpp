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


	void timer::draw_year_(int16_t ofs, uint16_t year)
	{
		int8_t n = -1;
		if((frame_count_ & 15) > 5) n = edit_pos_; 

		if(n != 0) task_.dm_.draw_3x5(13, ofs + 5, year % 10);
		year /= 10;
		if(n != 1) task_.dm_.draw_3x5( 9, ofs + 5, year % 10);
		year /= 10;
		if(n != 2) task_.dm_.draw_3x5( 5, ofs + 5, year % 10);
		year /= 10;
		if(n != 3) task_.dm_.draw_3x5( 1, ofs + 5, year % 10);
	}


	void timer::draw_date_(int16_t ofs, const tm& t)
	{
		int8_t n = -1;
		if((frame_count_ & 15) > 5) n = edit_pos_; 

		uint8_t m = t.tm_mon + 1;
		if(n != 0) {
			if(m / 10) task_.dm_.draw_3x5( 0, ofs + 2, m / 10);
			task_.dm_.draw_3x5( 4, ofs + 2, m % 10);
		}

		if(n != 1) {
			if(t.tm_mday / 10) task_.dm_.draw_3x5( 9, ofs + 2, t.tm_mday / 10);
			task_.dm_.draw_3x5(13, ofs + 2, t.tm_mday % 10);
		}

		// 曜日
		uint8_t w = 2;
		{
			const char* p = &week_str_[t.tm_wday * 3];
			for(uint8_t i = 0; i < 3; ++i) {
				uint8_t ch = pgm_read_byte_near(p);
				++p;
				w += task_.dm_.get_width_xx5(ch);
			}
		}
		short x = (16 - w) / 2; // センターリング
		const char* p = &week_str_[t.tm_wday * 3];
		for(uint8_t i = 0; i < 3; ++i) {
			uint8_t ch = pgm_read_byte_near(p);
			++p;
			x += task_.dm_.draw_xx5(x, ofs + 9, ch);
			x += 1;
		}
	}


	void timer::draw_time_(int16_t ofs, const tm& t)
	{
		int8_t n = -1;
		if((frame_count_ & 15) > 5) n = edit_pos_; 

		if(n != 3) task_.dm_.draw_3x5( 0, ofs + 0, t.tm_hour / 10);
		if(n != 2) task_.dm_.draw_3x5( 4, ofs + 0, t.tm_hour % 10);
		if(n != 1) task_.dm_.draw_3x5( 9, ofs + 0, t.tm_min / 10);
		if(n != 0) task_.dm_.draw_3x5(13, ofs + 0, t.tm_min % 10);

		if(n != 5) task_.dm_.draw_7x10(0, ofs + 6, t.tm_sec / 10);
		if(n != 4) task_.dm_.draw_7x10(9, ofs + 6, t.tm_sec % 10);
	}


	void timer::display_()
	{
		time_ = task_.rtc_.read();

		const system::switch_input& swi = task_.swi_;
		if(swi.get_positive() & system::switch_input::bits::LEFT_UP) {
			if(page_ < 2) {
				++page_;
				task_.music_.request(sound::music::id::tetris_move, 1);
			}
		}
		if(swi.get_positive() & system::switch_input::bits::LEFT_DOWN) {
			if(page_) {
				--page_;
				task_.music_.request(sound::music::id::tetris_move, 1);
			}
		}

		if(swi.last_touch(60 * 3)) {
			task_.eeprom_.write(device::eeprom::slot::timer_pos, page_);
		}

		if(page_ != page__) {
			if(page_ < page__) speed_ = 80000;
			else if(page_ > page__) speed_ =-80000; 
		}
		page__ = page_;

		int32_t n = -page_;
		n <<= 16;
		n *= 16;
		bool mi = false;
		if(n < pos_) mi = true;
		pos_ += speed_;
		speed_ *= 1024;
		speed_ /= 1030;
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

		draw_year_((pos_ >> 16) +  0, t->tm_year + 1900);
		draw_date_((pos_ >> 16) + 16, *t);
		draw_time_((pos_ >> 16) + 32, *t);

		// 下ボタン同時押しで、戻る
		if(swi.get_level() == (system::switch_input::bits::LEFT_DOWN
			| system::switch_input::bits::RIGHT_DOWN)) {
			mode_ = mode::ret_menu;
			task_.music_.request(sound::music::id::tetris_rot, 1);
		}
		// 全ボタン同時押しが４秒続く場合
		if(swi.get_level() == (system::switch_input::bits::LEFT_UP
			| system::switch_input::bits::RIGHT_UP
			| system::switch_input::bits::LEFT_DOWN
			| system::switch_input::bits::RIGHT_DOWN)) {
			++set_count_;
			if(set_count_ >= 240) {
				mode_ = mode::setting_y;
				edit_pos_ = 0;
				copy_tm(get_tm(), &tm_);
				task_.music_.request(sound::music::id::tetris_erase, 1);
			}
		} else {
			set_count_ = 0;
		}

	}


	// 年設定
	void timer::setting_y_()
	{
		uint16_t year = tm_.tm_year + 1900;
		draw_year_(0, year);

		uint16_t t = year;
		int8_t d = 0;
		if(edit_pos_ == 0) {
			d = t % 10;
		} else if(edit_pos_ == 1) {
			d = (t / 10) % 10;
		} else if(edit_pos_ == 2) {
			d = (t / 100) % 10;
		} else {
			d = (t / 1000) % 10;
		}
		const system::switch_input& swi = task_.swi_;
		if(swi.get_positive() & system::switch_input::bits::LEFT_UP) {
			task_.music_.request(sound::music::id::tetris_move, 1);
			++d;
		}
		if(swi.get_positive() & system::switch_input::bits::LEFT_DOWN) {
			task_.music_.request(sound::music::id::tetris_move, 1);
			--d;
		}
		if(d < 0) d = 9;
		else if(d >= 10) d = 0;
		if(edit_pos_ == 0) {
			uint16_t mod = t % 10;
			t -= mod;
			t +=  d;
		} else if(edit_pos_ == 1) {
			uint16_t mod = t % 100;
			t -= mod;
			t += static_cast<uint16_t>(d) * 10;
			t += mod % 10;
		} else if(edit_pos_ == 2) {
			uint16_t mod = t % 1000;
			t -= mod;
			t += static_cast<uint16_t>(d) * 100;
			t += mod % 100;
		} else {
			uint16_t mod = t % 10000;
			t -= mod;
			t += static_cast<uint16_t>(d) * 1000;
			t += mod % 1000;
		}
		tm_.tm_year = t - 1900;

		if(swi.get_positive() & system::switch_input::bits::RIGHT_UP) {
			task_.music_.request(sound::music::id::tetris_rot, 1);
			if(edit_pos_) --edit_pos_;
		}

		if(swi.get_positive() & system::switch_input::bits::RIGHT_DOWN) {
			task_.music_.request(sound::music::id::tetris_fall, 1);
			++edit_pos_;
			if(edit_pos_ >= 4) {
				mode_ = mode::setting_md;
				edit_pos_ = 0;
				// 新しい年号を使って各パラメーターを再計算
				time_t t = mktime(&tm_);
				gmtime(&t);
				copy_tm(get_tm(), &tm_);
			}
		}
	}


	// 月日設定
	void timer::setting_md_()
	{
		draw_date_(0, tm_);

		int8_t d = 0;
		if(edit_pos_ == 0) {
			d = tm_.tm_mon;
		} else if(edit_pos_ == 1) {
			d = tm_.tm_mday;
		}
		const system::switch_input& swi = task_.swi_;
		if(swi.get_positive() & system::switch_input::bits::LEFT_UP) {
			task_.music_.request(sound::music::id::tetris_move, 1);
			++d;
		}
		if(swi.get_positive() & system::switch_input::bits::LEFT_DOWN) {
			task_.music_.request(sound::music::id::tetris_move, 1);
			--d;
		}
		if(edit_pos_ == 0) {
			if(d < 0) d = 0;
			else if(d > 11) d = 11;
			tm_.tm_mon = d;
		} else if(edit_pos_ == 1) {
			if(d < 0) d = 0;
			else if(d > 31) d = 31;
			tm_.tm_mday = d;
		}
		{
			time_t t = mktime(&tm_);
			gmtime(&t);
			tm* m = get_tm();
			tm_.tm_wday = m->tm_wday; // 曜日のコピー
			tm_.tm_mday = m->tm_mday; // 日のコピー
		}

		if(swi.get_positive() & system::switch_input::bits::RIGHT_UP) {
			task_.music_.request(sound::music::id::tetris_rot, 1);
			if(edit_pos_) --edit_pos_;
		}

		if(swi.get_positive() & system::switch_input::bits::RIGHT_DOWN) {
			task_.music_.request(sound::music::id::tetris_fall, 1);
			++edit_pos_;
			if(edit_pos_ >= 2) {
				mode_ = mode::setting_t;
				edit_pos_ = 0;
			}
		}
	}


	// 時間設定
	void timer::setting_t_()
	{
		draw_time_(0, tm_);

		int8_t d = 0;
		if(edit_pos_ == 0) {
			d = tm_.tm_min % 10;
		} else if(edit_pos_ == 1) {
			d = tm_.tm_min / 10;
		} else if(edit_pos_ == 2) {
			d = tm_.tm_hour % 10;
		} else if(edit_pos_ == 3) {
			d = tm_.tm_hour / 10;
		} else if(edit_pos_ == 4) {
			d = tm_.tm_sec % 10;
		} else {
			d = tm_.tm_sec / 10;
		}
		const system::switch_input& swi = task_.swi_;
		if(swi.get_positive() & system::switch_input::bits::LEFT_UP) {
			task_.music_.request(sound::music::id::tetris_move, 1);
			++d;
		}
		if(swi.get_positive() & system::switch_input::bits::LEFT_DOWN) {
			task_.music_.request(sound::music::id::tetris_move, 1);
			--d;
		}
		if(edit_pos_ == 0) {
			if(d < 0) d = 9;
			else if(d > 9) d = 0;
			uint8_t mod = tm_.tm_min % 10;
			tm_.tm_min -= mod;
			tm_.tm_min += d;
		} else if(edit_pos_ == 1) {
			if(d < 0) d = 5;
			else if(d > 5) d = 0;
			uint8_t mod = tm_.tm_min % 100;
			tm_.tm_min -= mod;
			tm_.tm_min += d * 10;
			tm_.tm_min += mod % 10;
		} else if(edit_pos_ == 2) {
			if(d < 0) d = 9;
			else if(d > 9) d = 0;
			uint8_t mod = tm_.tm_hour % 10;
			tm_.tm_hour -= mod;
			tm_.tm_hour += d;
		} else if(edit_pos_ == 3) {
			if(d < 0) d = 2;
			else if(d > 2) d = 0;
			uint8_t mod = tm_.tm_hour % 100;
			tm_.tm_hour -= mod;
			tm_.tm_hour += d * 10;
			tm_.tm_hour += mod % 10;
		} else if(edit_pos_ == 4) {
			if(d < 0) d = 9;
			else if(d > 9) d = 0;
			uint8_t mod = tm_.tm_sec % 10;
			tm_.tm_sec -= mod;
			tm_.tm_sec += d;
		} else {
			if(d < 0) d = 5;
			else if(d > 5) d = 0;
			uint8_t mod = tm_.tm_sec % 100;
			tm_.tm_sec -= mod;
			tm_.tm_sec += d * 10;
			tm_.tm_sec += mod % 10;
		}

		if(swi.get_positive() & system::switch_input::bits::RIGHT_UP) {
			task_.music_.request(sound::music::id::tetris_rot, 1);
			if(edit_pos_) --edit_pos_;
		}

		if(swi.get_positive() & system::switch_input::bits::RIGHT_DOWN) {
			task_.music_.request(sound::music::id::tetris_fall, 1);
			++edit_pos_;
			if(edit_pos_ >= 6) {
				mode_ = mode::display;
				edit_pos_ = -1;
				// 新しい時間を RTC に書く
				time_t t = mktime(&tm_);
				task_.rtc_.write(t);				
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void timer::init()
	{
		page_ = task_.eeprom_.read(device::eeprom::slot::timer_pos);
		if(page_ > 3 || page_ < 0) page_ = 0;		
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
			const system::switch_input& swi = task_.swi_;
			if(!(swi.get_level() & system::switch_input::bits::LEFT_DOWN)) {
				if(!(swi.get_level() & system::switch_input::bits::RIGHT_DOWN)) {
					task_.start<menu>();
				}
			}
			// Ｘの表示
			task_.mng_.line(0, 0, 15, 15, 1);
			task_.mng_.line(15, 0, 0, 15, 1);
		}
		++frame_count_;
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

