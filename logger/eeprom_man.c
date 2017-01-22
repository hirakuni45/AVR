//=====================================================================//
/*!	@file
	@breif	EEPROM マネージメント関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "eeprom_man.h"
#include "time.h"

static uint16_t g_eeprom_start;
static uint16_t g_eeprom_length;

//-----------------------------------------------------------------//
/*!
	@breif	EEPROM マネージャー初期化
	@param[in]	start	開始アドレス
	@param[in]	length	長さ
*/
//-----------------------------------------------------------------//
void eeprom_man_init(uint16_t start, uint16_t length)
{
	g_eeprom_start  = start;
	g_eeprom_length = length;
}


//-----------------------------------------------------------------//
/*!
	@breif	EEPROM マネージメント領域をフォーマットする
*/
//-----------------------------------------------------------------//
void eeprom_man_format(void)
{
	uint16_t i;
	uint8_t *p = (uint8_t *)g_eeprom_start;
	for(i = 0; i < g_eeprom_length; ++i) {
		eeprom_write_byte(p, 0xff);
		++p;
	}
}





#if 0
void eeprom_lap_store(time_t start_t, const time_t *lap_buffer, short count)
{
}
#endif
