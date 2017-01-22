//=====================================================================//
/*!	@file
	@breif	タイトル表示関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "monograph.h"
#include "title.h"
#include "mainloop.h"
#include "mobj.h"
#include "fileio.h"
#include "menu.h"
#include "speed.h"
#include "tacho.h"
#include "laptimer.h"
#include "config.h"

static uint8_t g_title_loop;

//-----------------------------------------------------------------//
/*!
	@breif	タイトル初期化
*/
//-----------------------------------------------------------------//
void title_init(void)
{
	set_fb_clear(0);

	if(g_title_mode == 0) {	// タイトルを表示しない
		g_title_loop = 0;
	} else if(g_title_mode == 1) {	// system title
		g_title_loop = 100;	// 約３秒
		mobj_draw_title();
	} else if(g_title_mode == 2) {	// user title
		g_title_loop = 100 + 15;	// 約３秒＋マウント待ち
	}
	install_task(title_main);
}


//-----------------------------------------------------------------//
/*!
	@breif	タイトル表示メイン
*/
//-----------------------------------------------------------------//
void title_main(void)
{
	if((g_switch_negative != 0) || (g_title_loop == 0)) {
		if(g_startup_mode == 0) {
			install_task(speed_init);
		} else if(g_startup_mode == 1) {
			install_task(tacho_init);
		} else if(g_startup_mode == 2) {
			install_task(menu_init);
		} else {
			install_task(laptimer_init);
		}
		set_fb_clear(1);
	} else {
		--g_title_loop;
		if(g_title_mode == 2) {
			if(g_title_loop == 100) {
				if(fileio_get_status() & FILEIO_DISK_MOUNT) {
					char name[16];
					strcpy_P(name, PSTR("title.bin"));
					FIL fd;
					if(f_open(&fd, name, FA_OPEN_EXISTING | FA_READ) == 0) {
						char img[64];
						int i;
						for(i = 0; i < 16; ++i) {
							UINT bw;
							if(f_read(&fd, img, 64, &bw) == 0) {
								monograph_draw_image(0, i * 4, (unsigned char *)img, 128, 4);
							}
						}
						f_close(&fd);
					} else {
						g_title_loop = 0;
					}
				} else {
					g_title_loop = 0;	// マウント出来ない場合
				}
			}
		}
	}
}


/* ----- End Of File "title.c" ----- */
