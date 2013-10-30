#pragma once
//=====================================================================//
/*!	@file
	@breif	スイッチ・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>

namespace system {

	class switch_input {

	public:
		typedef unsigned char	u8;

	private:
		u8	level_;
		u8	positive_;
		u8	negative_;

	public:
		static const u8 RIGHT_UP   = 0x02;
		static const u8 RIGHT_DOWN = 0x01;
		static const u8 LEFT_UP    = 0x04;
		static const u8 LEFT_DOWN  = 0x08;

		switch_input() : level_(0), positive_(0), negative_(0) { }

		void set_level(u8 level) {
			positive_ =  level & ~level_;
			negative_ = ~level &  level_;
			level_ = level;
		}

		u8 get_level() const { return level_; }
		u8 get_positive() const { return positive_; }
		u8 get_negative() const { return negative_; }
	};
}
