//=====================================================================//
/*!	@file
	@breif	Low Battery 表示関係（ヘッダー）
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

#include "menu.h"
#include "lowbattery.h"

//-----------------------------------------------------------------//
/*!
	@breif	表示初期化
*/
//-----------------------------------------------------------------//
void lowbattery_init(void)
{
	install_task(lowbattery_main);
}


//-----------------------------------------------------------------//
/*!
	@breif	表示メイン
*/
//-----------------------------------------------------------------//
void lowbattery_main(void)
{
	char text[32];
///	unsigned long v = get_analog();
	unsigned long v = 0;
	int m, n;

	if((get_loop_count() & 15) > 6) {
		/// 2.56 * v / 1024
		v *= 1000;
		v /= 400;
		m = v % 1000;
		n = v / 1000;

		sprintf_P(text, PSTR("Low Battery: %d.%03d[V]"), n, m);
		monograph_draw_string(2, (64 - 12) / 2, text);
	}
}


/* ----- End Of File "lowbattery.c" ----- */


