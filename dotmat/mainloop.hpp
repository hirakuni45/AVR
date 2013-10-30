#pragma once
//=====================================================================//
/*!	@file
	@breif	ATMEGA88 ドット・マトリックス・メイン
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/sleep.h>

#if 0
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@breif	スイッチの状態ビット
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
enum SWITCH_BITS {
	SWITCH_BIT_LAP    = 0x01,	///< LAP    スイッチ・ビット
	SWITCH_BIT_CANCEL = 0x02,	///< CANCEL スイッチ・ビット
	SWITCH_BIT_UP     = 0x04,	///< UP     スイッチ・ビット
	SWITCH_BIT_DOWN   = 0x08,	///< DOWN   スイッチ・ビット
	SWITCH_BIT_PIT    = 0x10	///< PIT    スイッチ・ビット
};

extern uint8_t g_switch_level;
extern uint8_t g_switch_positive;
extern uint8_t g_switch_negative;

extern unsigned char g_loop_count;

extern uint8_t g_ext_battery;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if 0
	//-----------------------------------------------------------------//
	/*!
		@breif	フレーム・バッファの消去を制御
		@param[in]	val	「０」の場合消去しない、「０」以外は消去
	*/
	//-----------------------------------------------------------------//
	void set_fb_clear(char val);
#endif

	//-----------------------------------------------------------------//
	/*!
		@breif	新規タスク関数登録
		@param[in]	task	新規タスク関数アドレス
	*/
	//-----------------------------------------------------------------//
	void install_task(void (*task)(void));

#if 0
	//-----------------------------------------------------------------//
	/*!
		@breif	現在の時刻を返す
		@return	tm 構造体のポインター
	*/
	//-----------------------------------------------------------------//
	const struct tm *get_time(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	User Provided Timer Function for FatFs module@n
				This is a real time clock service to be called from@n
				FatFs module. Any valid time must be returned even if@n
				the system does not support a real time clock.
		@return	FatFs - module 用の時間データ
	*/
	//-----------------------------------------------------------------//
	DWORD get_fattime(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	fat date/time を struct tm 形式に変換@n
				曜日は計算されない
		@param[in]	date	FILEINFO.fdate 形式
		@param[in]	time	FILEINFO.fdtime 形式
		@param[in]	t		struct tm 形式
	*/
	//-----------------------------------------------------------------//
	void fattime_to_tm(WORD date, WORD time, struct tm *t);
#endif

#ifdef __cplusplus
};
#endif

