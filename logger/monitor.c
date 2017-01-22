//=====================================================================//
/*!	@file
	@breif	モニター表示関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "monograph.h"
#include "mobj.h"
#include "mainloop.h"
#include "utils.h"
#include "menu.h"
#include "monitor.h"
#include "mcp3208.h"
#include "gpsio.h"
#include "fileio.h"
#include "filer.h"
#include "psound.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@breif	A/D Converter 生データ表示
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
static void monitor_adc_main(void)
{
	char i;
	for(i = 0; i < 8; ++i) {
		uint16_t v = mcp3208_get(i);
		char text[32];
		sprintf_P(text, PSTR("%d: %d"), i, v);
		short x, y;
		if(i & 1) x = 64; else x = 0;
		y = (i >> 1) * 16 + 2;
		monograph_draw_string(x, y, text);
	}

	if(util_get_cancel()) {
		install_task(monitor_init);
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@breif	GPS のステータス等表示
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
static void monitor_gps_main(void)
{
	uint8_t n = gpsio_get_satellites();
	if(n == 0) {
		if((g_loop_count & 15) > 3) {
			monograph_draw_string_center_P(PSTR("GPS Not Ready"));
		}
	} else {
		short x = 0;
		short y = 0;

		time_t t = gpsio_get_time();
		if(t > 0) {
			struct tm *sttm = gmtime(&t);
			monograph_draw_date(x, y, sttm);
			monograph_draw_string_P(x + (6 * 11), y, get_week_text(sttm->tm_wday));
			y += 12;
			monograph_draw_time(x, y, sttm);
			y += 12;
		} else {
			monograph_draw_string_P(x, y, PSTR("G.P.S. time not ready"));
			y += 24;
		}

		monograph_draw_string_P(x, y, PSTR("Satellites:"));
		char	text[8];
		sprintf_P(text, PSTR("%d"), n);
		monograph_draw_string(x + 6 * 12, y, text);
	}

	if(util_get_cancel()) {
		install_task(monitor_init);
	}
}


static void monitor_puls_main(void);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@breif	パルス入力モニター初期化
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
static void monitor_puls_init(void)
{


	psound_disable();
	setup_input_capture(1);

	install_task(monitor_puls_main);
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@breif	パルス入力モニター・メイン
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
static void monitor_puls_main(void)
{





	if(util_get_cancel()) {
		setup_input_capture(0);
		install_task(monitor_init);
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@breif	モニター（ステータス）関係
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
static const prog_char g_monitor_menu_list[] = {
	"A/D Converter\0"
	"G.P.S.\0"
	"File System\0"
	"Puls Input\0"
};


//-----------------------------------------------------------------//
/*!
	@breif	モニター表示初期化
*/
//-----------------------------------------------------------------//
void monitor_init(void)
{
	install_task(monitor_main);
	util_menu_init(g_monitor_menu_list, 0);
}


//-----------------------------------------------------------------//
/*!
	@breif	モニター表示メイン
*/
//-----------------------------------------------------------------//
void monitor_main(void)
{
	char i = util_menu_service(-1);

	switch(i) {
	case 0:
		install_task(monitor_adc_main);
		break;
	case 1:
		install_task(monitor_gps_main);
		break;
	case 2:
		install_task(filer_init);
		break;
	case 3:
		install_task(monitor_puls_init);
		break;
	default:
		break;
	}

	draw_gps_sd_status();

	if(util_get_cancel()) {
		install_task(menu_init);
	}
}


/* ----- End Of File "monitor.c" ----- */


