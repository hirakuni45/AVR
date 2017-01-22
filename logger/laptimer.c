//=====================================================================//
/*!	@file
	@breif	ラップタイマーの記録と表示関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "monograph.h"
#include "laptimer.h"
#include "mobj.h"
#include "mainloop.h"
#include "menu.h"
#include "time.h"
#include "utils.h"
#include "datalogger.h"
#include "psound.h"

// ラップタイム時の点滅時間
#define TRIGGER_LOOP_DELAY	60

// ピットアウト時の点滅時間
#define TRIGGER_LOOP_PITOUT	60	

// ラップ数の限度
#define LAP_COUNT_LIMIT		300

static time_t g_start_time;
static uint32_t g_lap_total;
static volatile uint32_t g_lap_time_ref;
static volatile uint32_t g_lap_best;
static volatile uint16_t g_lap_count;
static volatile uint8_t g_trg_loop;		// ラップトリガーで起動されるタイマー
static volatile char g_lap_enable;
static char g_lap_step;
static char g_lap_exit;
static uint8_t g_cancel_count;
static uint8_t g_lap_exit_active;
static uint8_t g_pit_out_count;

static short g_recall_pos;

static unsigned long g_lap_buff[LAP_COUNT_LIMIT];

static char g_save_enable = 1;

#if 0
static const prog_char g_lap_config_list[] = {
	"Remain Limit\0"
	"Fuel Cost\0"
	"Fuel Limit\0"
};
#endif

#if 0
//-----------------------------------------------------------------//
/*!
	@breif	ラップタイマーセットアップ初期化
*/
//-----------------------------------------------------------------//
void laptimer_setup_init(void)
{
	util_menu_init(g_lap_config_list, 0);
	install_task(laptimer_setup_main);
	g_setup_pos = -1;
}


//-----------------------------------------------------------------//
/*!
	@breif	ラップタイマーセットアップ初期化
*/
//-----------------------------------------------------------------//
void laptimer_setup_main(void)
{
	char i;

	i = util_menu_service(g_setup_pos);
	if(g_setup_pos < 0) {
		if(i >= 0) {
			g_setup_pos = i;
		}
	} else {
		if(g_switch_positive & SWITCH_BIT_LAP) {
			g_setup_pos = -1;
		}
	}

	if(util_get_cancel()) {
		install_task(laptimer_init);
	}
}
#endif


static const prog_char g_lap_menu_list[] = {
	"Start\0"
	"Recall\0"
	"Setup\0"
};


//-----------------------------------------------------------------//
/*!
	@breif	ラップタイマー表示初期化
*/
//-----------------------------------------------------------------//
void laptimer_init(void)
{
	util_menu_init(g_lap_menu_list, 0);
	util_menu_offset(0, 24);
	util_menu_size(128, 64 - 24);
	install_task(laptimer_menu);
}


//-----------------------------------------------------------------//
/*!
	@breif	ラップタイマーメニュー
*/
//-----------------------------------------------------------------//
void laptimer_menu(void)
{
	short	x, y;
	const struct tm *tmp;
	char i;

	tmp = get_time();
	x = 0;
	y = 0;
	monograph_draw_date(x, y, tmp);
	monograph_draw_string_P(x + (6 * 11), y, get_week_text(tmp->tm_wday));

	y += 12;
	monograph_draw_time(x, y, tmp);
	monograph_line(0, 24, 127, 24, 1);

	i = util_menu_service(-1);

	switch(i) {
	case 0:
		start_logging();
		g_lap_count = 0;
		g_lap_buff[0] = 0;
		g_lap_best = 0;
		g_lap_time_ref = get_laptimer();
		g_trg_loop = TRIGGER_LOOP_DELAY;
		g_lap_enable = 1;
		g_lap_step = 0;
		g_lap_exit_active = 0;
		g_lap_exit = 0;
		g_lap_total = 0;
		g_cancel_count = 0;
		g_pit_out_count = 0;
		g_start_time = mktime(tmp);
		install_task(laptimer_main);
		break;
	case 1:
		install_task(recall_init);
		break;
	case 2:
//		install_task(setup_init);
		break;
	default:
		break;
	}

	if(util_get_cancel()) {
		install_task(menu_init);
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	ラップタイマー・サービス（サーバー）@n
			・1/100 秒間隔の割り込みから呼ばれる。@n
			・ラップタイム記録
	@param[in]	pit	ピットフラグ
 */
//-----------------------------------------------------------------//
void laptimer_int_service(char pit)
{
	unsigned long t;

	if(g_lap_enable == 0) return;

	if(g_lap_count < LAP_COUNT_LIMIT) {
		t = get_laptimer() - g_lap_time_ref;
		g_lap_count++;
		g_lap_buff[g_lap_count] = (t << 1) | pit;
		t = (g_lap_buff[g_lap_count] >> 1) - (g_lap_buff[g_lap_count - 1] >> 1);
		if(pit == 0) {
			if(g_lap_best == 0) g_lap_best = t;
			else if(g_lap_best > t) g_lap_best = t;
		}
		g_trg_loop = TRIGGER_LOOP_DELAY;
///		psound_request(9 + 12 * 5, 5);
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	ラップタイマー表示メイン
*/
//-----------------------------------------------------------------//
void laptimer_main(void)
{
	unsigned char trg;
	time_t v;
	unsigned long a, b, l;

	// 経過時間
	g_lap_total = get_laptimer() - g_lap_time_ref;
	mobj_draw_time_hms_12(0, 0, g_lap_total / 100);

	if(g_lap_best) {
		mobj_draw_time_mst_12(128 - 51, 0, g_lap_best);
	}
	
	monograph_line(0, 13, 127, 13, 1);
	monograph_line(0, 14, 127, 14, 1);

	trg = 0;
	if(g_trg_loop) {
		g_trg_loop--;
		if((g_trg_loop & 7) > 2) trg = 1;
		if(g_lap_count == 0) {
			v = 0;
		} else {
			v = (g_lap_buff[g_lap_count] >> 1) - (g_lap_buff[g_lap_count - 1] >> 1);
		}
	} else {
		if(g_lap_count == 0) {
			v = get_laptimer() - g_lap_time_ref;
		} else {
			v = get_laptimer() - g_lap_time_ref - (g_lap_buff[g_lap_count] >> 1);
		}
		trg = 1;
	}

	if(g_lap_exit) {
		monograph_draw_string_P((128 - (6 * 18)) / 2, (64 - 12) / 2, PSTR("'UP + DOWN' to End"));
		if((g_switch_level & (SWITCH_BIT_UP | SWITCH_BIT_DOWN)) == (SWITCH_BIT_UP | SWITCH_BIT_DOWN)) {
			g_lap_enable = 0;		// ラップタイマーのサービス（割り込み内処理）を停止
			finsh_logging();
			install_task(laptimer_save);
		}
	} else {
		if(trg) {
			mobj_draw_time_32(8, 16, v);
		}
	}
	
	monograph_line(0, 49, 127, 49, 1);
	monograph_line(0, 50, 127, 50, 1);

	mobj_draw_number3_12(100, 52, g_lap_count);

	if(g_lap_step == 0) {
		// ピットイン中はプログレスバー表示をしない
		if((g_switch_level & SWITCH_BIT_PIT)) {
			if(g_switch_level & SWITCH_BIT_PIT) {
				monograph_draw_string_P(0, (64 - 12), PSTR("- Pit IN -"));
			}
		} else if(g_pit_out_count) {
			monograph_draw_string_P(0, (64 - 12), PSTR("- Pit OUT -"));
			--g_pit_out_count;
		} else {
			// プログレスバー表示
			if(g_lap_count) {
				// ピット・アウトした周は表示しない
				if((g_lap_buff[g_lap_count - 1] & 1) == 0) {
					b = get_laptimer() - g_lap_time_ref - (g_lap_buff[g_lap_count] >> 1);
					a = (g_lap_buff[g_lap_count] >> 1) - (g_lap_buff[g_lap_count - 1] >> 1);	// 1周前のタイム
					if(a > 0) {
						l = b * 90 / a;
						if(l >= 90) l = 90;
						trg = 1;
						if(l >= 70) {
							if((g_loop_count & 15) < 6) trg = 0;
						}
						if(trg) {
							monograph_line(0, 56,  l, 56, 1);
							monograph_line(0, 57,  l, 57, 1);
						}
					}
				}
				monograph_line(0, 58, 90, 58, 1);
			}
		}
	} else if(g_lap_exit == 0) {
		g_lap_exit_active--;
		if(g_lap_exit_active == 0) g_lap_step = 0;

	}

	// ピットアウトの検出
	if(g_switch_negative & SWITCH_BIT_PIT) {
		g_pit_out_count = TRIGGER_LOOP_PITOUT;
	}

	// キャンセルボタンのホールド
	if(g_switch_level & SWITCH_BIT_CANCEL) {
		++g_cancel_count;
		if(g_cancel_count >= 50) {
			g_lap_exit = 1;
		}
	} else {
		g_cancel_count = 0;
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	ラップデータセーブ
*/
//-----------------------------------------------------------------//
void laptimer_save(void)
{
	if(g_save_enable != 0 && g_lap_count > 0) {
		uint16_t *p1 = 0;
		eeprom_write_word(p1, g_lap_count);
		++p1;
		eeprom_write_word(p1, g_lap_count ^ 0xffff);
		++p1;
		uint32_t *p2 = (uint32_t*)p1;
		eeprom_write_dword(p2, (uint32_t)g_start_time);
		++p2;
		eeprom_write_dword(p2, (uint32_t)g_lap_total);
		++p2;
		int i;
		for(i = 0; i < g_lap_count; ++i) {
			eeprom_write_dword(p2, (uint32_t)g_lap_buff[i]);
			p2++;
		}
	}
	install_task(menu_init);
}


//-----------------------------------------------------------------//
/*!
	@breif	リコール表示初期化（ラップタイムテーブルの表示）
*/
//-----------------------------------------------------------------//
void recall_init(void)
{
	uint16_t *p1;
	uint16_t v;
	int i;
	uint32_t *p2;

	p1 = 0;
	g_lap_count = eeprom_read_word(p1);
	++p1;
	v  = eeprom_read_word(p1);
	++p1;
	if(g_lap_count == (v ^ 0xffff) && g_lap_count > 0) {
		p2 = (uint32_t*)p1;
		g_start_time = eeprom_read_dword(p2);
		++p2;
		g_lap_total = eeprom_read_dword(p2);
		++p2;
		g_lap_best = 0;
		for(i = 0; i < g_lap_count; ++i) {
			g_lap_buff[i] = eeprom_read_dword(p2);
			++p2;
			if(i > 0) {
				unsigned long t = (g_lap_buff[i] >> 1) - (g_lap_buff[i - 1] >> 1);
				if(g_lap_best == 0) g_lap_best = t;
				else if(g_lap_best > t) g_lap_best = t;
			}
		}
		install_task(recall_date);
	} else {
		install_task(recall_nodata);
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	リコールデータ無し
*/
//-----------------------------------------------------------------//
void recall_nodata(void)
{
	unsigned char	trg;
	short	x, y;

	x = (128 - (9 * 6)) / 2;
	y = (64 - 12) / 2;

	trg = g_loop_count;
	if((trg & 15) > 6) {
		monograph_draw_string_P(x, y, PSTR("No Data !"));
	}

	if(util_get_cancel()) {
		install_task(laptimer_init);
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	リコール表示メイン（ラップタイムの表示）
*/
//-----------------------------------------------------------------//
void recall_date(void)
{
	struct tm *tmp;
	short x, y;

	tmp = gmtime(&g_start_time);

	monograph_line(0, 13, 127, 13, 1);
	monograph_line(0, 14, 127, 14, 1);

	x = 0;
	y = 20;
	monograph_draw_date(x, y, tmp);
	monograph_draw_string_P(x + (6 * 11), y, get_week_text(tmp->tm_wday));
	y += 12;
	monograph_draw_time(x, y, tmp);

	monograph_line(0, 49, 127, 49, 1);
	monograph_line(0, 50, 127, 50, 1);

	if(g_switch_positive & SWITCH_BIT_LAP) {
		g_recall_pos = 1;
		install_task(recall_main);
	} else if(util_get_cancel()) {
		install_task(laptimer_init);
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	リコール表示メイン（ラップタイムの表示）
*/
//-----------------------------------------------------------------//
void recall_main(void)
{
	unsigned char	trg;

	mobj_draw_time_hms_12(0, 0, g_lap_total / 100);

	if(g_lap_best) {
		mobj_draw_time_mst_12(128 - 51, 0, g_lap_best);
	}

	monograph_line(0, 13, 127, 13, 1);
	monograph_line(0, 14, 127, 14, 1);

	unsigned long v = 0;
	char pitin = 0;
	if(g_recall_pos) {
		pitin = g_lap_buff[g_recall_pos] & 1;
		v = (g_lap_buff[g_recall_pos] >> 1) - (g_lap_buff[g_recall_pos - 1] >> 1);
	}
	mobj_draw_time_32(8, 16, v);
	
	monograph_line(0, 49, 127, 49, 1);
	monograph_line(0, 50, 127, 50, 1);

	if(pitin) {
		monograph_draw_string_P(0, 64 - 12, PSTR("Pit - IN"));
	}

	mobj_draw_number3_12(100 - 9 * 3 - 9, 52, g_lap_count);
	mobj_draw_slash_12(100 - 9, 52);
	trg = g_loop_count;
	if((trg & 15) > 6) {
		mobj_draw_number3_12(100, 52, g_recall_pos);
	}

	v = g_recall_pos;

	if(g_switch_positive & SWITCH_BIT_UP) {
		++g_recall_pos;
	}
	if(g_switch_positive & SWITCH_BIT_DOWN) {
		--g_recall_pos;
	}

	if(g_recall_pos < 1) g_recall_pos = 1;
	else if(g_recall_pos > g_lap_count) g_recall_pos = g_lap_count;
	if(v != g_recall_pos) {
		psound_request(9 + 12 * 4, 1);
	}

	if(util_get_cancel()) {
		install_task(laptimer_init);
	}
}


/* ----- End Of File "laptimer.c" ----- */
