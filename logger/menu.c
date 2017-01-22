//=====================================================================//
/*!	@file
	@breif	メニュー表示関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "menu.h"
#include "utils.h"
#include "laptimer.h"
#include "monitor.h"
#include "config.h"
#include "fileio.h"
#include "gpsio.h"
#include "speed.h"
#include "tacho.h"
#include "config.h"

static const prog_char g_menu_list_rpm[] = {
	"Lap Timer\0"
	"R.P.M.\0"
	"Status\0"
	"Configuration\0"
};

static const prog_char g_menu_list_spd[] = {
	"Lap Timer\0"
	"Speed\0"
	"Status\0"
	"Configuration\0"
};

//-----------------------------------------------------------------//
/*!
	@breif	GPS/SD ステータス表示
*/
//-----------------------------------------------------------------//
void draw_gps_sd_status(void)
{
	// SD カードがマウントされたら、'□' を表示
	if(fileio_get_status() & FILEIO_DISK_MOUNT) {
		monograph_draw_font(128-8, 64-12, 2);
	}

	// GPS 受信がされていれば、サテライト数を表示
	uint8_t n = gpsio_get_satellites();
	if(n) {
		if(n > 9) n += 'A' - 10;
		else n += '0';
		monograph_draw_font(128-8, 13, n);
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	メニュー表示初期化
*/
//-----------------------------------------------------------------//
void menu_init(void)
{
	if(g_pi_mode) {
		util_menu_init(g_menu_list_rpm, 1);
	} else {
		util_menu_init(g_menu_list_spd, 1);
	}
	install_task(menu_main);
}


//-----------------------------------------------------------------//
/*!
	@breif	メニュー表示メイン
*/
//-----------------------------------------------------------------//
void menu_main(void)
{
	char	i;

	i = util_menu_service(-1);

	switch(i) {
	case 0:
		install_task(laptimer_init);
		break;
	case 1:
		if(g_pi_mode) {
			install_task(tacho_init);
		} else {
			install_task(speed_init);
		}
		break;
	case 2:
		install_task(monitor_init);
		break;
	case 3:
		install_task(config_init);
		break;
	default:
		break;
	}

	draw_gps_sd_status();
}

/* ----- End Of File "menu.c" ----- */
