//=====================================================================//
/*!	@file
	@breif	ファイルの表示と選択関係（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "utils.h"
#include "monitor.h"
#include "mainloop.h"
#include "monograph.h"
#include "fileio.h"
#include "filer.h"
#include "psound.h"

// スクロールするまでの時間（フレーム数）
#define SCROLL_DELAY	45

static char g_root_path[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
static char g_file_name[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
static DIR g_dir;
static char g_filer_task;
static short g_files_ofs;
static short g_files_pos;
static short g_files_num;
static uint8_t g_path_level;
static char g_file_attr;
static uint8_t g_scroll_delay;
static short g_scroll_pos;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	ファイル数をカウント（シーク）
	@param[in]	seek	「０」の場合全体のカウント、「０」以外の場合シーク
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static short files_index_seek(short seek)
{
	FILINFO fno;
#if _USE_LFN
	static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
	fno.lfname = lfn;
	fno.lfsize = sizeof(lfn);
#endif
	short n = 0;
	for( ; ; ) {
		FRESULT res = f_readdir(&g_dir, &fno);
		if(res != FR_OK || fno.fname[0] == 0) break;
		if(fno.fname[0] == '.') continue;
		++n;
		if(seek) {
			--seek;
			if(seek == 0) break;
		}
	}
	return n;
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	ファイル名の描画
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void draw_file_name(short x, short y, const char *name, short ofs)
{
	monograph_frame(x + 6 + 1, y,     128 - 6 - 2, 12 + 4, 1);
	monograph_frame(x + 6,     y + 1, 128 - 6,     12 + 2, 1);
	monograph_draw_string(x + 6 + 3 - ofs, y + 2, name);
	monograph_fill(x, y + 2, 6, 12, 0);
	if(g_file_attr) {
		monograph_draw_font(x, y + 2, '/');
	}
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	ディレクトリーのリスト表示
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static char files_list(short x, short y, char n)
{
	g_file_name[0] = 0;
	g_file_attr = 0;
	FILINFO fno;
#if _USE_LFN
	static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
	fno.lfname = lfn;
	fno.lfsize = sizeof(lfn);
#endif
	char i = 0;
	while(i < n) {
		FRESULT res = f_readdir(&g_dir, &fno);
		if(res != FR_OK) {
			return -1;	// エラー
		}
		if(fno.fname[0] == '.') continue;
		if(fno.fname[0] == 0) {
			return 0;	// 終端
		}
#if _USE_LFN
		const char *fn = *fno.lfname ? fno.lfname : fno.fname;
#else
		const char *fn = fno.fname;
#endif
		if(g_files_pos == (g_files_ofs + i)) {
			strcpy(g_file_name, fn);
			if(fno.fattrib & AM_DIR) {
				g_file_attr = 1;
			}
			draw_file_name(x, y, fn, 0);
			y += 12 + 4;
		} else {
			if(fno.fattrib & AM_DIR) {
				monograph_draw_font(x, y, '/');
			}
			monograph_draw_string(x + 6 + 2 + 1, y, fn);
			y += 12;
		}
		++i;
	}
	return 1;
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	SD カードがマウントされていない
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void filer_no_mount(void)
{
	if((fileio_get_status() & FILEIO_DISK_MOUNT) == 0) {
		if((g_loop_count & 15) > 3) {
			monograph_draw_string_center_P(PSTR("SD-Card No Mount"));
		}
	} else {
		// マウントされたら初期化プロセスからやり直し
		install_task(filer_init);
	}

	if(util_get_cancel()) {
		install_task(monitor_init);
	}
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	ディレクトリーが開けない場合
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void dir_open_error(void)
{
	if((g_loop_count & 15) > 3) {
		monograph_draw_string_center_P(PSTR("Directory Open Error"));
	}

	if(util_get_cancel()) {
		set_fb_clear(1);
		install_task(monitor_init);
	}
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	ファイルが開けない場合
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void file_open_error(void)
{
	if((g_loop_count & 15) > 3) {
		monograph_draw_string_center_P(PSTR("File Open Error"));
	}

	if(util_get_cancel()) {
		set_fb_clear(1);
		install_task(filer_init);
	}
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	ファイルが無い場合
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void filer_no_file(void)
{
	if((g_loop_count & 15) > 3) {
		monograph_draw_string_center_P(PSTR("No File"));
	}

	if(util_get_cancel()) {
		set_fb_clear(1);
		install_task(monitor_init);
	}
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	ファイル・リスト再描画
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void redraw_list_request(void)
{
	g_scroll_delay = SCROLL_DELAY;
	g_scroll_pos = 0;
	psound_request(9 + 12 * 4, 1);
	f_readdir(&g_dir, NULL);		// インデックスを巻き戻す
	if(g_files_ofs) {
		files_index_seek(g_files_ofs);	// オフセット分インデックスを移動
	}
	g_filer_task = 2;
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
/*!
	@breif	ファイルの情報を表示
*/
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
static void info_file(void)
{
	if((fileio_get_status() & FILEIO_DISK_MOUNT) == 0) {
		set_fb_clear(1);
		install_task(filer_no_mount);
		return;
	}

	short x = 0;
	short y = 0;
	short l = strlen(g_file_name) * 6;
	short limit = 128;
	short ofs = 0;
	if(g_scroll_delay) {
		--g_scroll_delay;
	} else {
		if(l >= limit) {
			if(g_loop_count & 1) ++g_scroll_pos;
			if(g_scroll_pos >= l) g_scroll_pos = -limit;
			ofs = g_scroll_pos;
			monograph_fill(x, y, 128, 12, 0);
		}
	}
	monograph_draw_string(x - ofs, y, g_file_name);
	monograph_line(0, 13, 127, 13, 1);
	monograph_line(0, 14, 127, 14, 1);

	if(g_filer_task == 0) {
		char path[_MAX_LFN];

		strcpy(path, g_root_path);
		strcat_P(path, PSTR("/"));
		strcat(path, g_file_name);

		FILINFO fno;
#if _USE_LFN
		static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
		fno.lfname = lfn;
		fno.lfsize = sizeof(lfn);
#endif
		if(f_stat(path, &fno) != FR_OK) {
			set_fb_clear(1);
			install_task(file_open_error);
			return;
		}

		struct tm sttm;
		fattime_to_tm(fno.fdate, fno.ftime, &sttm);
		time_t t = mktime(&sttm);
		struct tm *tmp = gmtime(&t);

		y += 16;
		monograph_draw_date(x, y, tmp);
		monograph_draw_string_P(x + (6 * 11), y, get_week_text(tmp->tm_wday));
		y += 14;
		monograph_draw_time(x, y, tmp);
		y += 14;
		char text[16];
		sprintf_P(text, PSTR("%ld [bytes]"), fno.fsize);
		monograph_draw_string(x, y, text);

		g_filer_task = 1;
	}

	if(util_get_cancel()) {
		redraw_list_request();
		install_task(filer_main);
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	表示初期化
*/
//-----------------------------------------------------------------//
void filer_init(void)
{
	set_fb_clear(0);	// フレームバッファクリアしない

	g_root_path[0] = 0;

	g_filer_task = 0;

	g_files_ofs = 0;
	g_files_pos = 0;
	g_files_num = 0;
	g_path_level = 0;
	g_scroll_delay = 0;
	g_scroll_pos = 0;

	g_file_name[0] = 0;
	g_file_attr = 0;

	install_task(filer_main);
}


//-----------------------------------------------------------------//
/*!
	@breif	表示メイン
*/
//-----------------------------------------------------------------//
void filer_main(void)
{
	if((fileio_get_status() & FILEIO_DISK_MOUNT) == 0) {
		set_fb_clear(1);
		install_task(filer_no_mount);
		return;
	}

	short pos, ofs;
	switch(g_filer_task) {
	case 0:	// ディレクトリーをオープン
		g_files_ofs = 0;
		g_files_pos = 0;
		g_files_num = 0;
		g_file_name[0] = 0;
		g_file_attr = 0;
		if(f_opendir(&g_dir, g_root_path) == FR_OK) {
			++g_filer_task;
		} else {
			set_fb_clear(1);
			install_task(dir_open_error);
			return;
		}
		break;
	case 1:	// ファイル数をカウント
		g_files_num = files_index_seek(0);
		if(g_files_num == 0) {
			set_fb_clear(1);
			install_task(filer_no_file);
			return;
		}
		f_readdir(&g_dir, NULL);	// インデックスを巻き戻す
		++g_filer_task;
		break;
	case 2:	// ５ファイルだけ表示
		monograph_clear(0);		// 画面を消去して書きなおす
		files_list(0, 0, 5);
		++g_filer_task;
		break;
	case 3:	// スイッチ入力
		pos = g_files_pos;
		if(g_switch_positive & SWITCH_BIT_DOWN) {
			++pos;
		}
		if(g_switch_positive & SWITCH_BIT_UP) {
			--pos;
		}
		if(pos < 0) pos = 0;
		else if(pos >= g_files_num) {
			pos = g_files_num - 1;
		}
		ofs = g_files_ofs;
		if(pos < g_files_ofs) --ofs;
		else if(pos >= (g_files_ofs + 5)) ++ofs;
		if(ofs < 0) ofs = 0;
		else if(ofs >= g_files_num) {
			ofs = g_files_num - 1;
		}
		if((pos != g_files_pos) || (ofs != g_files_ofs)) {
			g_files_pos = pos;
			g_files_ofs = ofs;
			redraw_list_request();
		}
		if(g_scroll_delay) {
			--g_scroll_delay;
		} else {
			short l = strlen(g_file_name) * 6;
			short limit = 128 - 6 - 2 - 2 - 1;
			if(l >= limit) {
				if(g_loop_count & 1) ++g_scroll_pos;
				if(g_scroll_pos >= l) g_scroll_pos = -limit;
				short x = 0;
				short y = (g_files_pos - g_files_ofs) * 12;
				monograph_fill(x, y + 2, 127, 12, 0);
				draw_file_name(x, y, g_file_name, g_scroll_pos);
			}
		}
		break;
	default:
		break;
	}

	if(g_switch_positive & SWITCH_BIT_LAP) {
		if(g_file_name[0] != 0) {
			g_scroll_delay = SCROLL_DELAY;
			psound_request(6 + 12 * 2, 4);
			if(g_file_attr) {	// ディレクトリーの場合
				strcat_P(g_root_path, PSTR("/"));
				strcat(g_root_path, g_file_name);
				++g_path_level;
				g_filer_task = 0;
			} else {	// ファイル選択の場合
				monograph_clear(0);
				g_scroll_pos = 0;
				g_filer_task = 0;
				install_task(info_file);
			}
		}
	}

	if(g_path_level) {
		if(g_switch_positive & SWITCH_BIT_CANCEL) {
			psound_request(3 + 12 * 2, 4);
			char *p = strrchr(g_root_path, '/');
			if(p != NULL) {
				*p = 0;
				--g_path_level;
				g_filer_task = 0;
			}
		}
	} else {
		if(util_get_cancel()) {
			set_fb_clear(1);
			install_task(monitor_init);
		}
	}
}


/* ----- End Of File "filer.c" ----- */


