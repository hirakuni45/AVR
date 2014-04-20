#pragma once
//=====================================================================//
/*!	@file
	@breif	ATMEGA328 ガイガーカウンター・メイン
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <string.h>
#include "format.hpp"

namespace system {

	class output {
	public:
		void operator() () {

		}
	};

	utils::format<output>	format_;

	//-----------------------------------------------------------------//
	/*!
		@breif	タイマー同期
	*/
	//-----------------------------------------------------------------//
	class timer {
		volatile uint8_t timer_count_;
		volatile uint8_t draw_count_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@breif	コンストラクター
		*/
		//-----------------------------------------------------------------//
		timer() : timer_count_(0), draw_count_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@breif	タイマー・サービス
		*/
		//-----------------------------------------------------------------//
		void service()
		{
			// draw_count_ <- 120 / 4 (30Hz)
			if((timer_count_ & 3) == 0) ++draw_count_;
			++timer_count_;
		}


		//-----------------------------------------------------------------//
		/*!
			@breif	描画の同期
		*/
		//-----------------------------------------------------------------//
		void sync_draw()
		{
			volatile uint8_t cnt = draw_count_;
			while(draw_count_ == cnt) ;
		}



	};

	//-----------------------------------------------------------------//
	/*!
		@breif	新規タスク関数登録
		@param[in]	task	新規タスク関数アドレス
	*/
	//-----------------------------------------------------------------//
	void install_task(void (*task)());
}

