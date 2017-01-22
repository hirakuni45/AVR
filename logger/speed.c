//=====================================================================//
/*!	@file
	@breif	スピード表示関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "monograph.h"
#include "speed.h"
#include "mobj.h"
#include "mainloop.h"
#include "menu.h"
#include "time.h"
#include "mcp3208.h"
#include "eeprom_man.h"
#include "config.h"
#include "psound.h"
#include "puls.h"

extern volatile uint32_t g_capture_count;

//-----------------------------------------------------------------//
/*!
	@breif	移動距離を返す
	@return 距離[m]
*/
//-----------------------------------------------------------------//
static uint32_t get_distance(void)
{
	return g_capture_count / (uint32_t)g_pi_ppr_spd * (uint32_t)g_pi_distance / 1000;
}


#define SPEED_UPDATE_NUM	15

static uint16_t g_spd_buff[SPEED_UPDATE_NUM];
static uint8_t g_spd_buff_pos;

static uint32_t g_dis;

static uint8_t g_trip_index;
static uint8_t g_info_index;
static uint32_t g_trip[2];
static uint32_t g_distance;
static uint32_t g_distance_org[3];

static uint8_t g_trip_reset_hold;
static uint8_t g_trip_flash;

//-----------------------------------------------------------------//
/*!
	@breif	速度表示初期化
*/
//-----------------------------------------------------------------//
void speed_init(void)
{
	g_distance = eeprom_read_dword((uint32_t *)EEPROM_ID_TOTAL_DISTANCE);
	if((int32_t)(g_distance) == -1) {
		eeprom_write_dword((uint32_t *)EEPROM_ID_TOTAL_DISTANCE, 0);
	}
	g_trip[0] = eeprom_read_dword((uint32_t *)EEPROM_ID_TOTAL_TRIP_A);
	if((int32_t)(g_trip[0]) == -1) {
		eeprom_write_dword((uint32_t *)EEPROM_ID_TOTAL_TRIP_A, 0);
	}
	g_trip[1] = eeprom_read_dword((uint32_t *)EEPROM_ID_TOTAL_TRIP_B);
	if((int32_t)(g_trip[1]) == -1) {
		eeprom_write_dword((uint32_t *)EEPROM_ID_TOTAL_TRIP_B, 0);
	}

	g_trip_index = eeprom_read_byte((uint8_t *)EEPROM_ID_TRIP_INDEX) & 1;

	g_trip_reset_hold = 0;
	g_trip_flash = 0;

	g_dis = get_distance();
	g_distance_org[2] = g_distance_org[1] = g_distance_org[0] = g_dis;

	g_spd_buff_pos = 0;
	uint8_t i;
	for(i = 0; i < SPEED_UPDATE_NUM; ++i) {
		g_spd_buff[i] = g_spd_kmh;
	}

	puls_task_set(PULS_TASK_SPEED);
	psound_disable();
	setup_input_capture(1);

	install_task(speed_main);
}


static void save_speed_param(uint32_t dis)
{
	// メートル単位で記録
	eeprom_write_dword((uint32_t *)EEPROM_ID_TOTAL_TRIP_A, g_trip[0] + (dis - g_distance_org[0]));
	eeprom_write_dword((uint32_t *)EEPROM_ID_TOTAL_TRIP_B, g_trip[1] + (dis - g_distance_org[1]));
	eeprom_write_dword((uint32_t *)EEPROM_ID_TOTAL_DISTANCE, g_distance + (dis - g_distance_org[2]));
	eeprom_write_byte((uint8_t *)EEPROM_ID_TRIP_INDEX, g_trip_index);
}


//-----------------------------------------------------------------//
/*!
	@breif	速度表示メイン
*/
//-----------------------------------------------------------------//
void speed_main(void)
{
	uint32_t dis = get_distance();	// 走行距離（単位[m]）

	mobj_draw_cha_12(80 - 15, 0, g_trip_index + 'A');
	char f = 1;
	if(g_trip_flash) {
		--g_trip_flash;
		if((g_loop_count & 7) < 3) f = 0;
	}
	if(f) {
		mobj_draw_number4d1_12(80, 0, (g_trip[g_trip_index] + (dis - g_distance_org[g_trip_index])) / 100);
	}

	monograph_line(0, 13, 127, 13, 1);
	monograph_line(0, 14, 127, 14, 1);

	g_spd_buff[g_spd_buff_pos] = g_spd_kmh;
	++g_spd_buff_pos;
	if(g_spd_buff_pos >= SPEED_UPDATE_NUM) g_spd_buff_pos = 0;
	uint8_t i, j;
	uint16_t spd = 0;
	uint8_t cnt = 0;
	for(j = 0; j < SPEED_UPDATE_NUM; ++j) {
		short s = g_spd_buff[j];
		uint8_t c = 0;
		for(i = 0; i < SPEED_UPDATE_NUM; ++i) {
			if(s == g_spd_buff[i]) ++c;
		}
		if(c > cnt) { cnt = c; spd = s; }
	}
	mobj_draw_speed_32(35, 16, spd);

	monograph_line(0, 49, 127, 49, 1);
	monograph_line(0, 50, 127, 50, 1);

	const struct tm *sttm = get_time();
	time_t t = mktime(sttm);
	t += get_timezone_offset();
	mobj_draw_time_hms_12(0, 52, t);

	mobj_draw_number6_12(74, 52, (g_distance + (dis - g_distance_org[2])) / 1000);

	if(g_info_index & 1) {
		// 車体のバッテリー電圧
		uint16_t v = mcp3208_get(6);
		// 1/4 されているので、4 倍
		v *= 4;
		v /= 100;
		mobj_draw_volt_12(0, 0, v);
	} else {
		// 水温	163.83度 = 4095
		uint16_t v = mcp3208_get(5);
		v *= 4;
		v /= 10;
		mobj_draw_temp_12(0, 0, v);
	}

	if(g_switch_level & SWITCH_BIT_DOWN) {
		if(g_trip_reset_hold >= 100) {
			g_trip_reset_hold = 0;
			g_trip[g_trip_index] = 0;
			g_distance_org[g_trip_index] = dis;
			if(g_trip_index == 0) {
				eeprom_write_dword((uint32_t *)EEPROM_ID_TOTAL_TRIP_A, 0);
			} else {
				eeprom_write_dword((uint32_t *)EEPROM_ID_TOTAL_TRIP_B, 0);
			}
			g_trip_flash = 100;
		} else {
			++g_trip_reset_hold;
		}
	} else {
		g_trip_reset_hold = 0;
	}

	if(g_switch_positive & SWITCH_BIT_LAP) {
		++g_trip_index;
		g_trip_index &= 1;
	}
	if(g_switch_positive & SWITCH_BIT_UP) {
		++g_info_index;
		g_info_index &= 1;
	}

	if(g_switch_positive & SWITCH_BIT_CANCEL) {
		save_speed_param(dis);
		setup_input_capture(0);
		puls_task_set(PULS_TASK_NONE);
		install_task(menu_init);
	}
}


/* ----- End Of File "speed.c" ----- */
