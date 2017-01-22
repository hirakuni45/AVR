//=====================================================================//
/*!	@file
	@breif	コンフィグレーション関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "utils.h"
#include "monograph.h"
#include "monitor.h"
#include "mobj.h"
#include "mainloop.h"
#include "config.h"
#include "rtc4543.h"
#include "time.h"
#include "speed.h"
#include "xitoa.h"
#include "menu.h"
#include "eeprom_man.h"
#include "psound.h"

uint8_t g_log_gps_enable;
uint8_t g_log_adc_enable;
uint8_t g_log_puls_enable;

uint8_t g_title_mode;
uint8_t g_startup_mode;
uint8_t g_kanji_mode;

uint8_t g_gps_interval;
uint8_t g_gps_auto_adjust;
uint8_t g_gps_ext_echo;

uint8_t g_pi_mode;
uint8_t g_pi_ppr_spd;
uint8_t g_pi_ppr_rpm;
uint16_t g_pi_distance;

uint8_t g_spd_slot;
uint16_t g_spd_length;

//-----------------------------------------------------------------//
/*!
	@breif	設定ロード
*/
//-----------------------------------------------------------------//
void load_configuration(void)
{
	g_log_gps_enable  = eeprom_read_byte((uint8_t *)EEPROM_ID_LOG_GPS_ENABLE) & 1;
	g_log_adc_enable  = eeprom_read_byte((uint8_t *)EEPROM_ID_LOG_ADC_ENABLE) & 1;
	g_log_puls_enable = eeprom_read_byte((uint8_t *)EEPROM_ID_LOG_PULS_ENABLE) & 1;

	g_title_mode   = eeprom_read_byte((uint8_t *)EEPROM_ID_TITLE_MODE) % 3;
	g_startup_mode = eeprom_read_byte((uint8_t *)EEPROM_ID_STARTUP_MODE) & 3;
	g_kanji_mode   = eeprom_read_byte((uint8_t *)EEPROM_ID_KANJI_MODE) & 1;

	g_gps_interval    = eeprom_read_byte((uint8_t *)EEPROM_ID_GPS_10HZ_ENABLE) & 1;
	g_gps_auto_adjust = eeprom_read_byte((uint8_t *)EEPROM_ID_GPS_AUTO_ADJUST) & 1;
	g_gps_ext_echo    = eeprom_read_byte((uint8_t *)EEPROM_ID_GPS_EXT_ECHO) & 1;

	g_pi_mode     = eeprom_read_byte((uint8_t *)EEPROM_ID_PLUS_INPUT_MODE) & 1;
	g_pi_ppr_spd  = eeprom_read_byte((uint8_t *)EEPROM_ID_PLUS_PER_RAD_SPD);
	if(g_pi_ppr_spd == 0xff) {	// EEPROM が初期化された場合の初期値(6: ６スロット)
		g_pi_ppr_spd = 6;
		eeprom_write_byte((uint8_t *)EEPROM_ID_PLUS_PER_RAD_SPD, g_pi_ppr_spd);
	}
	g_pi_ppr_rpm  = eeprom_read_byte((uint8_t *)EEPROM_ID_PLUS_PER_RAD_RPM);
	if(g_pi_ppr_rpm == 0xff) {	// EEPROM が初期化された場合の初期値(2: ４サイクルエンジン)
		g_pi_ppr_rpm = 2;
		eeprom_write_byte((uint8_t *)EEPROM_ID_PLUS_PER_RAD_RPM, g_pi_ppr_rpm);
	}
	g_pi_distance = eeprom_read_word((uint16_t *)EEPROM_ID_PLUS_RAD_DISTANCE);
	if(g_pi_distance == 0xffff) {	// EEPROM が初期化された場合の初期値(1357: 標準的な１７インチタイヤ)
		g_pi_distance = 1357;
		eeprom_write_word((uint16_t *)EEPROM_ID_PLUS_RAD_DISTANCE, g_pi_distance);
	}
}


static void eeprom_cmp_write_byte(uint8_t *eeprom_adr, uint8_t data)
{
	if(eeprom_read_byte(eeprom_adr) != data) {
		eeprom_write_byte(eeprom_adr, data);
	}
}


static void eeprom_cmp_write_word(uint16_t *eeprom_adr, uint16_t data)
{
	if(eeprom_read_word(eeprom_adr) != data) {
		eeprom_write_word(eeprom_adr, data);
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	設定セーブ
*/
//-----------------------------------------------------------------//
void save_configuration(void)
{
	eeprom_cmp_write_byte((uint8_t *)EEPROM_ID_LOG_GPS_ENABLE, g_log_gps_enable);
	eeprom_cmp_write_byte((uint8_t *)EEPROM_ID_LOG_ADC_ENABLE, g_log_adc_enable);
	eeprom_cmp_write_byte((uint8_t *)EEPROM_ID_LOG_PULS_ENABLE, g_log_puls_enable);

	eeprom_cmp_write_byte((uint8_t *)EEPROM_ID_TITLE_MODE, g_title_mode);
	eeprom_cmp_write_byte((uint8_t *)EEPROM_ID_STARTUP_MODE, g_startup_mode);
	eeprom_cmp_write_byte((uint8_t *)EEPROM_ID_KANJI_MODE, g_kanji_mode);

	eeprom_cmp_write_byte((uint8_t *)EEPROM_ID_GPS_10HZ_ENABLE, g_gps_interval);
	eeprom_cmp_write_byte((uint8_t *)EEPROM_ID_GPS_AUTO_ADJUST, g_gps_auto_adjust);
	eeprom_cmp_write_byte((uint8_t *)EEPROM_ID_GPS_EXT_ECHO, g_gps_ext_echo);

	eeprom_cmp_write_byte((uint8_t *)EEPROM_ID_PLUS_INPUT_MODE, g_pi_mode);
	eeprom_cmp_write_byte((uint8_t *)EEPROM_ID_PLUS_PER_RAD_SPD, g_pi_ppr_spd);
	eeprom_cmp_write_byte((uint8_t *)EEPROM_ID_PLUS_PER_RAD_RPM, g_pi_ppr_rpm);
	eeprom_cmp_write_word((uint16_t *)EEPROM_ID_PLUS_RAD_DISTANCE, g_pi_distance);
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@breif	データ・ロガー設定関係
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
static const prog_char g_no_yes_list[] = {
	"No\0"
	"Yes\0"
};

static const prog_char g_config_log_menu_list[] = {
	"GPS Log \0"
	"ADC Log \0"
	"Puls Log\0"
	"Save\0"
};

static void data_logger_config_main(void);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	データ・ロガー設定初期化
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void data_logger_config_init(void)
{
	util_menu_init(g_config_log_menu_list, 0);
	util_menu_centerring_disable(1);
	util_menu_offset(8 + 12, 0);

	install_task(data_logger_config_main);
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	データ・ロガー設定メイン
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void data_logger_config_main(void)
{
	char i = util_menu_service(-1);

	switch(i) {
	case 0:
		g_log_gps_enable ^= 1;
		break;
	case 1:
		g_log_adc_enable ^= 1;
		break;
	case 2:
		g_log_puls_enable ^= 1;
		break;
	case 3:
		save_configuration();
		install_task(config_init);
		break;
	default:
		break;
	}

	g_log_gps_enable &= 1;
	g_log_adc_enable &= 1;
	g_log_puls_enable &= 1;

	short x = 8 + util_menu_get_width() + 16;
	short y = 32 - (util_menu_get_height() / 2);

	const prog_char *p;
	p = get_prog_text_P(g_no_yes_list, g_log_gps_enable);
	monograph_draw_string_P(x, y, p);

	y += 12;
	p = get_prog_text_P(g_no_yes_list, g_log_adc_enable);
	monograph_draw_string_P(x, y, p);

	y += 12;
	p = get_prog_text_P(g_no_yes_list, g_log_puls_enable);
	monograph_draw_string_P(x, y, p);

	if(util_get_cancel()) {
		install_task(config_init);
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@breif	スタートアップ設定関係
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
static const prog_char g_config_startup_menu_list[] = {
	"Title\0"
	"Startup\0"
	"Kanji\0"
	"Save\0"
};

static const prog_char g_config_title_list[] = {
	"None\0"
	"Default\0"
	"User\0"
};

static const prog_char g_config_startup_list[] = {
	"Speed\0"
	"Taco\0"
	"Menu\0"
	"Lap\0"
};

static void config_system_main(void);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	システム設定の初期化
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void config_system_init(void)
{
	util_menu_init(g_config_startup_menu_list, 0);
	util_menu_centerring_disable(1);
	util_menu_offset(8 + 12, 0);

	install_task(config_system_main);
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	システム設定メイン
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void config_system_main(void)
{
	char i = util_menu_service(-1);

	switch(i) {
	case 0:
		++g_title_mode;
		break;
	case 1:
		++g_startup_mode;
		break;
	case 2:
		++g_kanji_mode;
		break;
	case 3:
		save_configuration();
		install_task(config_init);
		break;
	default:
		break;
	}

	if(g_title_mode >= 3) g_title_mode = 0;
	if(g_startup_mode >= 4) g_startup_mode = 0;
	g_kanji_mode &= 1;

	short x = 8 + util_menu_get_width() + 16;
	short y = 32 - (util_menu_get_height() / 2);

	const prog_char *p;
	p = get_prog_text_P(g_config_title_list, g_title_mode);
	monograph_draw_string_P(x, y, p);

	y += 12;
	p = get_prog_text_P(g_config_startup_list, g_startup_mode);
	monograph_draw_string_P(x, y, p);

	y += 12;
	p = get_prog_text_P(g_no_yes_list, g_kanji_mode);
	monograph_draw_string_P(x, y, p);

	if(util_get_cancel()) {
		install_task(config_init);
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@breif	日付の設定関係
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
static char g_step;
static char	g_pos;

static short get_value(struct tm *t, char pos)
{
	switch(pos) {
	case 0: return t->tm_year;
	case 1: return t->tm_mon;
	case 2: return t->tm_mday;
	case 3: return t->tm_hour;
	case 4: return t->tm_min;
	case 5: return t->tm_sec;
	default: return 0;
	}
}


static void set_value(struct tm *t, char pos, short val)
{
	switch(pos) {
	case 0: t->tm_year = val; break;
	case 1: t->tm_mon  = val; break;
	case 2: t->tm_mday = val; break;
	case 3: t->tm_hour = val; break;
	case 4: t->tm_min  = val; break;
	case 5: t->tm_sec  = val; break;
	default: break;
	}
}

static void config_date_main(void);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	日付の設定初期化
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void config_date_init(void)
{
	memcpy(get_tm(), get_time(), sizeof(struct tm));	// 現在の時刻をコピー
	install_task(config_date_main);
	g_pos = 0;
	g_step = 0;
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	日付の設定メイン
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void config_date_main(void)
{
	short x, y;
	char i;
	char len;
	short val, lo, hi;
	struct tm *tmp;
	time_t	tt;

	tmp = get_tm();

	char cursor;
	if(g_step == 0) {
		char pos = g_pos;
		if(g_switch_positive & SWITCH_BIT_UP) ++g_pos;
		if(g_switch_positive & SWITCH_BIT_DOWN) --g_pos;
		if(g_pos < 0) g_pos = 0;
		else if(g_pos >= 7) g_pos = 7 - 1;
		val = get_value(tmp, g_pos);
		cursor = '_';
		if(pos != g_pos) {
			psound_request(9 + 12 * 4, 1);
		}
	} else {
		cursor = 0;
	}

	if(g_pos == 0) {
		lo = 0;
		hi = 3999 - 1900;
	} else if(g_pos == 1) {
		lo = 0;
		hi = 11;
	} else if(g_pos == 2) {
		lo = 1;
		hi = get_mday(tmp->tm_year + 1900, tmp->tm_mon);
	} else if(g_pos == 3) {
		lo = 0;
		hi = 23;
	} else if(g_pos == 4) {
		lo = 0;
		hi = 59;
	} else if(g_pos == 5) {
		lo = 0;
		hi = 59;
	} else {
		lo = 0;
		hi = 0;
	}

	if(g_step == 1 && g_pos >= 0 && g_pos <= 5) {
		val = get_value(tmp, g_pos);
		if(g_switch_positive & SWITCH_BIT_UP) ++val;
		if(g_switch_positive & SWITCH_BIT_DOWN) --val;
		if(val < lo) val = lo;
		else if(val > hi) val = hi;
		set_value(tmp, g_pos, val);
	}

	val = get_mday(tmp->tm_year + 1900, tmp->tm_mon);
	if(tmp->tm_mday > val) tmp->tm_mday = val;

	x = (128 - (10 * 6)) / 2;
	y = (64 - (3 * 12) - (2 * 10)) / 2;
	monograph_draw_date(x, y, tmp);
	if(g_pos >= 0 && g_pos < 3) {
		if((g_loop_count & 15) > 6) {
			if(g_pos == 0) {
				len = 4;
			} else {
				x += 6 * 2;
				x += g_pos * 6 * 3;
				len = 2;
			}
			if(cursor) {
				for(i = 0; i < len; ++i) {
					monograph_draw_font(x, y, cursor);
					x += 6;
				}
			} else {
				monograph_frame(x - 2, y - 1, len * 6 + 2, 13, 1);
			}
		}
	}

	x = (128 - (8 * 6)) / 2;
	y += 10 * 2;
	monograph_draw_time(x, y, tmp);
	if(g_pos >= 3 && g_pos < 6) {
		if((g_loop_count & 15) > 6) {
			len  = 2;
			x += (g_pos - 3) * 6 * 3;
			if(cursor) {
				for(i = 0; i < len; ++i) {
					monograph_draw_font(x, y, cursor);
					x += 6;
				}
			} else {
				monograph_frame(x - 2, y - 1, len * 6 + 2, 13, 1);
			}
		}
	}

	x = 64 - ((7 * 6) / 2);
	y += 10 * 2;
	monograph_draw_string(x, y, "Setting");
	if(g_pos == 6) {
		if((g_loop_count & 15) > 6) {
			len  = 7;
			for(i = 0; i < len; ++i) {
				monograph_draw_font(x, y, cursor);
				x += 6;
			}
		}
	}

	if(g_switch_positive & SWITCH_BIT_LAP) {
		if(g_pos == 6) {
			tt = mktime(tmp);
			tmp = gmtime(&tt);
			rtc4543_put_time(tmp);
			install_task(config_init);
		} else {
			g_step ^= 1;
		}
	}

	if(util_get_cancel()) {
		install_task(config_init);
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@breif	G.P.S. の設定関係
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
static const prog_char g_gps_config_menu_list[] = {
	"Interval\0"
	"Time Adjust\0"
	"Ext Echo\0"
	"Save\0"
};

static const prog_char g_gps_interval_list[] = {
	"10Hz\0"
	"1Hz\0"
};

static void config_gps_main(void);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	G.P.S. の設定初期化
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void config_gps_init(void)
{
	util_menu_init(g_gps_config_menu_list, 0);
	util_menu_centerring_disable(1);
	util_menu_offset(8 + 12, 0);

	install_task(config_gps_main);
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	G.P.S. の設定メイン
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void config_gps_main(void)
{
	char i = util_menu_service(-1);

	switch(i) {
	case 0:
		g_gps_interval ^= 1;
		break;
	case 1:
		g_gps_auto_adjust ^= 1;
		break;
	case 2:
		g_gps_ext_echo ^= 1;
		break;
	case 3:
		save_configuration();
		install_task(config_init);
		break;
	default:
		break;
	}

	g_gps_interval &= 1;
	g_gps_auto_adjust &= 1;
	g_gps_ext_echo &= 1;

	short x = 8 + util_menu_get_width() + 16;
	short y = 32 - (util_menu_get_height() / 2);

	const prog_char *p;
	p = get_prog_text_P(g_gps_interval_list, g_gps_interval);
	monograph_draw_string_P(x, y, p);

	y += 12;
	p = get_prog_text_P(g_no_yes_list, g_gps_auto_adjust);
	monograph_draw_string_P(x, y, p);

	y += 12;
	p = get_prog_text_P(g_no_yes_list, g_gps_ext_echo);
	monograph_draw_string_P(x, y, p);

	if(util_get_cancel()) {
		install_task(config_init);
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@breif	Puls Input の設定関係
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
static const prog_char g_pi_config_menu_list[] = {
	"Mode\0"
	"P.P.R.\0"
	"Distance\0"
	"Save\0"
};

static const prog_char g_pi_mode_list[] = {
	"Speed\0"
	"R.P.M.\0"
};

static char g_distance_mode;

static void config_puls_input_main(void);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	Puls Input の設定初期化
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void config_puls_input_init(void)
{
	util_menu_init(g_pi_config_menu_list, 0);
	util_menu_centerring_disable(1);
	util_menu_offset(8 + 12, 0);

	g_distance_mode = 0;

	install_task(config_puls_input_main);
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	Puls Input の設定初期化
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void config_puls_input_main(void)
{
	char m = -1;
	if(g_distance_mode) {
		if(g_switch_positive & SWITCH_BIT_LAP) {
			psound_request(6 + 12 * 2, 4);
			g_distance_mode = 0;
		}
		short v = g_pi_distance;
		if(g_switch_positive & SWITCH_BIT_UP) ++v;
		if(g_switch_positive & SWITCH_BIT_DOWN) --v;
		if(g_pi_distance != v) {
			g_pi_distance = v % 2000;
			psound_request(6 + 12 * 3, 2);
		}
		m = 2;
	}

	char i = util_menu_service(m);

	switch(i) {
	case 0:
		++g_pi_mode;
		break;
	case 1:
		if(g_pi_mode) {
			++g_pi_ppr_rpm;
		} else {
			++g_pi_ppr_spd;
		}
		break;
	case 2:
		if(g_pi_mode == 0) {
			g_distance_mode = 1;	// 距離変更モードにエントリー
		}
		break;
	case 3:
		save_configuration();
		install_task(config_init);
		break;
	default:
		break;
	}

	g_pi_mode &= 1;
	if(g_pi_ppr_rpm > 4) g_pi_ppr_rpm = 1;
	if(g_pi_ppr_spd > 20) g_pi_ppr_spd = 1;

	short x = 8 + util_menu_get_width() + 16;
	short y = 32 - (util_menu_get_height() / 2);

	const prog_char *p;
	p = get_prog_text_P(g_pi_mode_list, g_pi_mode);
	monograph_draw_string_P(x, y, p);

	char nmb[8];

	y += 12;
	if(g_pi_mode & 1) {
		sprintf_P(nmb, PSTR("%d"), g_pi_ppr_rpm);
	} else {
		sprintf_P(nmb, PSTR("%d"), g_pi_ppr_spd);
	}
	monograph_draw_string(x, y, nmb);

	y += 12;
	if(g_pi_mode & 1) {
		monograph_draw_string_P(x, y, PSTR("----"));
	} else {
		sprintf_P(nmb, PSTR("%4d"), g_pi_distance);
		monograph_draw_string(x, y, nmb);
		if(g_distance_mode) {
			if((g_loop_count & 7) > 2) {
				util_draw_edit_box(x, y, 4 * 6, 12, 1);
			}
		}
	}

	if(util_get_cancel()) {
		install_task(config_init);
	}
}


//=================================================================//
/*!
	@breif	コンフィグ表示関係
*/
//=================================================================//
static const prog_char g_config_menu_list[] = {
	"Data Logger\0"
	"G.P.S.\0"
	"Puls Input\0"
	"System\0"
	"Time/Date\0"
};


//-----------------------------------------------------------------//
/*!
	@breif	コンフィグ表示初期化
*/
//-----------------------------------------------------------------//
void config_init(void)
{
	load_configuration();

	install_task(config_main);
	util_menu_init(g_config_menu_list, 0);
}


//-----------------------------------------------------------------//
/*!
	@breif	コンフィグ表示メイン
*/
//-----------------------------------------------------------------//
void config_main(void)
{
	char i = util_menu_service(-1);

	switch(i) {
	case 0:
		install_task(data_logger_config_init);
		break;
	case 1:
		install_task(config_gps_init);
		break;
	case 2:
		install_task(config_puls_input_init);
		break;
	case 3:
		install_task(config_system_init);
		break;
	case 4:
		install_task(config_date_init);
		break;
	default:
		break;
	}

	draw_gps_sd_status();

	if(util_get_cancel()) {
		install_task(menu_init);
	}
}


/* ----- End Of File "config.c" ----- */
