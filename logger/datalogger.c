//=====================================================================//
/*!	@file
	@breif	データ・ロガー関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "xitoa.h"
#include "monograph.h"
#include "mobj.h"
#include "mainloop.h"
#include "menu.h"
#include "mcp3208.h"
#include "datalogger.h"
#include "config.h"
#include "eeprom_man.h"
#include "fileio.h"

static char g_log_task;
static FIL g_fil;
static int g_log_count;
static uint32_t g_log_frame;
static char g_root_path[16];

FIL *g_fd;

static int get_n(const char *t)
{
	int v = 0;
	char ch;
	while((ch = *t++) != 0) {
		if(ch >= '0' && ch <= '9') {
			v *= 10;
			v += ch - '0';
		} else {
			break;
		}
	}
	return v;
}


//-----------------------------------------------------------------//
/*!
	@breif	ルートパスの作成
	@param[in]	path	生成したパスの格納先
	@param[in]	base	ベース名
	@param[in]	count	インデックス
*/
//-----------------------------------------------------------------//
void create_root_path(char *path, const char *base, int count)
{
	sprintf_P(path, PSTR("%s/%s_%03d.LOG"), g_root_path, base, count);
}


//-----------------------------------------------------------------//
/*!
	@breif	データ・ロガー・サーバー初期化
*/
//-----------------------------------------------------------------//
void datalogger_init(void)
{
	g_log_task = LOG_NONE;
	g_log_count = 1;
	g_root_path[0] = 0;
	g_fd = NULL;
}


//-----------------------------------------------------------------//
/*!
	@breif	データ・ロガー・サーバー・サービス
*/
//-----------------------------------------------------------------//
void datalogger_service(void)
{
	if((fileio_get_status() & FILEIO_DISK_MOUNT) == 0) return;

	if(g_log_task == LOG_FILE_OPEN) {
		char path[40];
		if(g_log_gps_enable | g_log_adc_enable | g_log_puls_enable) {
			sprintf_P(path, PSTR("%s/GPAD_%03d.LOG"), g_root_path, g_log_count);
			if(f_open(&g_fil, path, FA_CREATE_ALWAYS | FA_WRITE) != 0) {
				g_log_task = LOG_NONE;
			} else {
				g_fd = &g_fil;
				if(g_log_adc_enable) {
					mcp3208_fifo_enable(1);
				}
				g_log_task = LOG_FILE_WRITE;
				g_log_frame = 0;
			}
		}
	} else if(g_log_task == LOG_FILE_CLOSE) {
		if(g_fd != NULL) {
			f_close(g_fd);
			g_fd = NULL;
		}
		g_log_task = LOG_NONE;
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	データ・ロガー・サービス・リクエスト
	@param[in]	task	タスク識別
*/
//-----------------------------------------------------------------//
void datalogger_request(enum LOG_TASK task)
{
	g_log_task = task;

	if(task == LOG_FILE_OPEN) { // 新規フォルダーとファイル名のインデックスなどを用意する
		g_root_path[0] = 0;
		if(mkdir_date(g_root_path)) {
			// 一番最後の番号を取得
			FILINFO fno;
#if _USE_LFN
			char lfn[_MAX_LFN + 1];
			fno.lfname = lfn;
			fno.lfsize = sizeof(lfn);
#endif
			DIR d;
			if(f_opendir(&d, g_root_path) == FR_OK) {
				for( ; ; ) {
					FRESULT res = f_readdir(&d, &fno);
#if _USE_LFN
					const char *fn = *fno.lfname ? fno.lfname : fno.fname;
#else
					const char *fn = fno.fname;
#endif
					if(res != FR_OK || fn[0] == 0) break;
					if(fn[0] == '.') continue;
					if(strncmp_P(fn, PSTR("GPAD_"), 5) == 0) {
						int n = get_n(fn + 5) + 1;
						if(g_log_count < n) g_log_count = n;
					}
				}
			}
		} else {
			g_log_count = 1;
		}
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	ロギングの開始手続き
*/
//-----------------------------------------------------------------//
void start_logging(void)
{
	datalogger_request(LOG_FILE_OPEN);
}


//-----------------------------------------------------------------//
/*!
	@breif	ロギングの終了手続き
*/
//-----------------------------------------------------------------//
void finsh_logging(void)
{
	datalogger_request(LOG_FILE_CLOSE);
}



/* ----- End Of File "datalogger.c" ----- */


