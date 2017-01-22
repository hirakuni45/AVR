//=====================================================================//
/*!	@file
	@breif	パルス入力関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "puls.h"
#include "config.h"

extern volatile uint32_t g_capture_value;
extern volatile uint8_t g_capture_overflow;
extern volatile uint32_t g_capture_count;
extern volatile uint8_t g_capture_sync;

volatile uint8_t g_spd_task_cnt;

uint16_t g_spd_kmh;
uint16_t g_eng_rpm;
uint16_t g_puls_frq;

static char g_puls_task;

//-----------------------------------------------------------------//
/*!
	@breif	パルス・サービス・初期化
*/
//-----------------------------------------------------------------//
void puls_service_init(void)
{
	g_spd_task_cnt = g_capture_sync;

	g_spd_kmh = 0;
	g_eng_rpm = 0;
	g_puls_frq = 0;
	g_puls_task = PULS_TASK_NONE;
}


//-----------------------------------------------------------------//
/*!
	@breif	パルス・サービス・メイン
*/
//-----------------------------------------------------------------//
void puls_service_main(void)
{
	if(g_spd_task_cnt != g_capture_sync) {	//  速度計算
		g_spd_task_cnt = g_capture_sync;

		if(g_puls_task == PULS_TASK_SPEED) {
			uint32_t cnt = g_capture_value;
			cnt *= (uint32_t)(g_pi_distance);
			if(cnt > 0) {
				// 基準クロック 19.6608MHz 1/64 --> 307200
				// １時間辺りの秒数 ---> 3600秒
				g_spd_kmh = (307200L * 3600L) / cnt / (uint32_t)g_pi_ppr_spd;
				if(g_spd_kmh > PULS_SPD_MAX) g_spd_kmh = PULS_SPD_MAX;	// SPEED limitter.
			}
		} else if(g_puls_task == PULS_TASK_TACHO) {	//  エンジン回転数
			uint32_t cnt = g_capture_value;
			if(cnt > 0) {
				// 基準クロック 19.6608MHz 1/64 ---> 307200
				// １分辺りの回転数 ---> 60倍
				g_eng_rpm = (307200L * 60L) / cnt / (uint32_t)g_pi_ppr_rpm;
				if(g_eng_rpm > PULS_RPM_MAX) g_eng_rpm = PULS_RPM_MAX;
			}
		} else if(g_puls_task == PULS_TASK_FRQ) {


		} else {
			g_spd_kmh = 0;
			g_eng_rpm = 0;
			g_puls_frq = 0;
		}
	} else {
		if(g_puls_task == PULS_TASK_SPEED) {
			if(g_capture_overflow >= 6) {
				g_spd_kmh = 0;
			}
		} else if(g_puls_task == PULS_TASK_TACHO) {
			if(g_capture_overflow >= 3) {
				g_eng_rpm = 0;
			}
		} else if(g_puls_task == PULS_TASK_FRQ) {


		} else {
			g_spd_kmh = 0;
			g_eng_rpm = 0;
			g_puls_frq = 0;
		}
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	パルス・タスク設定
	@param[in]	task	パルス・タスク ID
*/
//-----------------------------------------------------------------//
void puls_task_set(enum PULS_TASK task)
{
	g_puls_task = task;
}

