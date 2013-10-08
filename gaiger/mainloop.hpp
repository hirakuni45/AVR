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

namespace sys {
	//-----------------------------------------------------------------//
	/*!
		@breif	新規タスク関数登録
		@param[in]	task	新規タスク関数アドレス
	*/
	//-----------------------------------------------------------------//
	void install_task(void (*task)());
}

