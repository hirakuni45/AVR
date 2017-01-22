//=====================================================================//
/*!	@file
	@breif	ファイル入出力関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "xitoa.h"
#include "fileio.h"
#include "diskio.h"
#include "mainloop.h"

#define _INSERT_DELAY	10	// カード認識ディレイフレーム数
#define _OPEN_MAX		8	// 同時オープン数(5)

static uint8_t g_fileio_sts;
static uint8_t g_insert_count;
static FATFS g_fatfs;

//-----------------------------------------------------------------//
/*!
	@breif	ファイルシステム・初期化
 */
//-----------------------------------------------------------------//
void fileio_init(void)
{
	g_fileio_sts = 0;
	g_insert_count = 0;
}


//-----------------------------------------------------------------//
/*!
	@breif	日付を使ってディレクトリーを作成
	@param[in]	path	日付名のパスを返す
	@return	成功した場合「０」、失敗したら「-1」
*/
//-----------------------------------------------------------------//
int mkdir_date(char *path)
{
	const struct tm *sttm = get_time();
	int year = sttm->tm_year + 1900;
	if(year < 2011 || sttm->tm_mday <= 0 || sttm->tm_mday > 31 || sttm->tm_mon >= 12) {
		strcpy_P(path, PSTR("20XX-XX-XX"));
	} else {
		sprintf_P(path, PSTR("%04d-%02d-%02d"), year, sttm->tm_mon + 1, sttm->tm_mday);
	}
	return f_mkdir(path);
}


//-----------------------------------------------------------------//
/*!
	@breif	ファイルシステム・サービス@n
			※毎フレーム（一定のインターバルで）呼び出す事。@n
 */
//-----------------------------------------------------------------//
void fileio_service(void)
{
	DSTATUS st = disk_status(0);
	uint8_t a = g_fileio_sts;
	if((st & STA_NODISK) == 0) {
		g_fileio_sts |=  FILEIO_INS_LEVEL;
	} else {
		g_fileio_sts &= ~FILEIO_INS_LEVEL;
	}

	// 刺さった瞬間を作る
	if((a & FILEIO_INS_LEVEL) == 0 && (g_fileio_sts & FILEIO_INS_LEVEL) != 0) {
		g_fileio_sts |=  FILEIO_INS_POSITIVE;
	} else {
		g_fileio_sts &= ~FILEIO_INS_POSITIVE;
	}

	// 抜かれた瞬間を作る
	if((a & FILEIO_INS_LEVEL) != 0 && (g_fileio_sts & FILEIO_INS_LEVEL) == 0) {
		g_fileio_sts |=  FILEIO_INS_NEGATIVE;
	} else {
		g_fileio_sts &= ~FILEIO_INS_NEGATIVE;
	}

	// カードが抜かれたら、電源を切る
	if((g_fileio_sts & FILEIO_INS_LEVEL) == 0) {
		BYTE cmd[1];
		cmd[0] = 0;
		disk_ioctl(0, CTRL_POWER, cmd);
		g_fileio_sts &= ~(FILEIO_DISK_INIT | FILEIO_DISK_MOUNT);
		g_insert_count = 0;
		return;
	}

	// カードが刺さった！
	if(g_fileio_sts & FILEIO_INS_POSITIVE) {
		g_insert_count = _INSERT_DELAY;
	}

	// 刺さった状態が「_INSERT_DELAY」フレーム持続したら初期化を行う
	if((g_fileio_sts & FILEIO_INS_LEVEL) != 0 && g_insert_count > 0) {
		g_insert_count--;
		if(g_insert_count == 0) {
			st = disk_initialize(0);
			if((st & STA_NOINIT) == 0) {
				g_fileio_sts |= FILEIO_DISK_INIT;
				return;
			}
		}
	}

	// 初期化が終わっていたら、マウントを行う
	if((g_fileio_sts & FILEIO_DISK_INIT) != 0 && (g_fileio_sts & FILEIO_DISK_MOUNT) == 0) {
		if(FR_OK == f_mount(0, &g_fatfs)) {
			g_fileio_sts |= FILEIO_DISK_MOUNT;
		} else {	// マウントに失敗した場合の挙動
//			g_insert_count = _INSERT_DELAY;
//			g_fileio_sts &= ~(FILEIO_DISK_INIT | FILEIO_DISK_MOUNT);
			g_fileio_sts |= FILEIO_DISK_NG;
		}
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	状況を返す
	@return FILEIO ステータス
*/
//-----------------------------------------------------------------//
enum FILEIO_STS fileio_get_status(void)
{
	return g_fileio_sts;
}


/* ----- End Of File "fileio.c" ----- */
