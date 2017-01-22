#ifndef RTC4543_H
#define RTC4543_H
//=====================================================================//
/*!	@file
	@breif	RTC-4543 ドライバー（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <string.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------//
/*!
	@breif	RTC の初期化
*/
//-----------------------------------------------------------------//
void rtc4543_init();


//-----------------------------------------------------------------//
/*!
	@breif	RTC-4543SA レジスターの読み出し
	@param[in]	buff	読みだしたデータの保存先
*/
//-----------------------------------------------------------------//
void rtc4543_read(uint8_t *buff);


//-----------------------------------------------------------------//
/*!
	@breif	RTC-4543SA レジスターの書き込み
	@param[in]	buff	書き込みデータ
*/
//-----------------------------------------------------------------//
void rtc4543_write(const uint8_t *buff);


//-----------------------------------------------------------------//
/*!
	@breif	RTC-4543 レジスターから「time_tm」構造体に読み込む@n
			※ year カウンターは 100 までしかレンジが無い為、100@n
			年のオフセット（2000年）を追加する。
	@param[in]	tmp tm 構造体のポインター
*/
//-----------------------------------------------------------------//
void rtc4543_get_time(struct tm *tmp);


//-----------------------------------------------------------------//
/*!
	@breif	RTC-4543 レジスターに「time_tm」構造体から書き込む@n
			※ year は 100 までしかレンジが無い為、100 の余りが@n
			書き込まれる。
	@param[in]	tmp tm 構造体のポインター
*/
//-----------------------------------------------------------------//
void rtc4543_put_time(const struct tm *tmp);


#ifdef __cplusplus
};
#endif

#endif // RTC4543_H
