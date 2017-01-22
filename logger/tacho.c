//=====================================================================//
/*!	@file
	@breif	エンジン回転表示関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "tacho.h"
#include "monograph.h"
#include "speed.h"
#include "mobj.h"
#include "mainloop.h"
#include "menu.h"
#include "puls.h"
#include "psound.h"

//-----------------------------------------------------------------//
/*!
	@breif	エンジン回転表示初期化
*/
//-----------------------------------------------------------------//
void tacho_init(void)
{
	puls_task_set(PULS_TASK_TACHO);
	psound_disable();
	setup_input_capture(1);

	install_task(tacho_main);
}


//-----------------------------------------------------------------//
/*!
	@breif	エンジン回転表示メイン
*/
//-----------------------------------------------------------------//
void tacho_main(void)
{





	monograph_line(0, 13, 127, 13, 1);
	monograph_line(0, 14, 127, 14, 1);

	mobj_draw_number5_32(1, 16, g_eng_rpm);

	monograph_line(0, 49, 127, 49, 1);
	monograph_line(0, 50, 127, 50, 1);

	const struct tm *sttm = get_time();
	time_t t = mktime(sttm);
	t += get_timezone_offset();
	mobj_draw_time_hms_12(0, 52, t);





	if(g_switch_positive & SWITCH_BIT_CANCEL) {
		setup_input_capture(0);
		puls_task_set(PULS_TASK_NONE);
		install_task(menu_init);
	}

}


/* ----- End Of File "tacho.c" ----- */
