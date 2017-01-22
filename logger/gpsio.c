//=====================================================================//
/*!	@file
	@breif	GPS 入出力関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "xitoa.h"
#include "uart.h"
#include "fileio.h"
#include "gpsio.h"
#include "rtc4543.h"
#include "config.h"

// 内部 RTC への時間合わせが必要なサテライトの数
#define TIME_ADJUST_SATELLITES	1

static uint8_t g_recv_no_count;
static uint8_t g_line_pos;
static char g_line_buff[256];
static char g_gps_status;
static uint8_t g_satellites;
static time_t g_rmc_time;
static char g_time_adjust;
static char g_gps_setup;

static int get_dec1(char ch)
{
	if('0' <= ch && ch <= '9') {
		return ch - '0';
	}
	return 0;
}

static int get_dec2(const char *text)
{
	int v = 0;
	v = get_dec1(*text++);
	v *= 10;
	v += get_dec1(*text);
	return v;
}

static int get_dec(const char *text)
{
	int v = 0;
	char ch;
	while((ch = *text++) != 0) {
		if(ch >= '0' && ch <= '9') {
			v *= 10;
			v += ch - '0';
		} else {
			break;
		}
	}
	return v;
}

static const char *get_param(const char *text, uint8_t idx)
{
	char ch;
	while((ch = *text++) != 0) {
		if(ch == ',') {
			--idx;
			if(idx == 0) return text;
		}
	}
	return NULL;
}

// GPGSV を取得して、人工衛星の数など解析
static void get_gpgsv(const char *line)
{
	if(strncmp_P(line, PSTR("$GPGSV"), 6) != 0) return;

	line += 6;

	const char *p;
	p = get_param(line, 3);	// Satellites Used
	if(p != NULL) {
		g_satellites = get_dec(p);
	}
}


// GPRMC を取得して、時間と日付を取得
static void get_gprmc(const char *line)
{
	if(strncmp_P(line, PSTR("$GPRMC"), 6) != 0) {
		return;
	}

	line += 6;

	const char *p;
	p = get_param(line, 1);	// UTC time hhmmss
	struct tm tt;
	if(p != NULL) {
		tt.tm_hour = get_dec2(p);
		p += 2;
		tt.tm_min  = get_dec2(p);
		p += 2;
		tt.tm_sec  = get_dec2(p);
	}
	p = get_param(line, 9);	// UTC data ddmmyy
	if(p != NULL) {
		tt.tm_mday = get_dec2(p);
		p += 2;
		tt.tm_mon  = get_dec2(p) - 1;
		p += 2;
		tt.tm_year = get_dec2(p) + 100;
	}

	// タイムゾーン偏差を加える
	g_rmc_time = mktime(&tt) + get_timezone_offset();
}

static char gps_data_analize(const char *line)
{
	if(line[0] != '$') {
		goto gps_error;
	}
	line++;

	char ch;
	uint8_t sum = 0;
	uint8_t sum_val = 0;
	char sum_zone = 0;
	while((ch = *line++) != 0) {
		if(sum_zone) {
			sum_val <<= 4;
			if(ch >= '0' && ch <= '9') sum_val |= ch - '0';
			else if(ch >= 'A' && ch <= 'F') sum_val |= ch - 'A' + 10;
			else {
				goto gps_error;
			}
		} else if(ch == '*') {
			sum_zone = 1;
		} else {
			sum ^= ch;
		}
	}

	if(sum_zone) {
//		printf("org sum: %02X\n", sum_val);
//		printf("cal sum: %02X\n", sum);
//		printf("\n");
		if(sum == sum_val) {
			return 1;
		}
	}
  gps_error:
	return 0;
}


//-----------------------------------------------------------------//
/*!
	@breif	GPS 初期化
 */
//-----------------------------------------------------------------//
void gpsio_init(void)
{
	uart_init(1, 9600);		// Chanel-1: SUP500F GPS default baudrate

	g_recv_no_count = 0;
	g_line_pos = 0;
	g_gps_status = 0;

	if(g_gps_auto_adjust & 1) {
		g_time_adjust = 0;
	} else {
		g_time_adjust = 1;	// GPS による時間合わせをしない
	}

	g_gps_setup = 0;
}


//-----------------------------------------------------------------//
/*!
	@breif	GPS サービス
	@param[in]	fd	ファイル・ディスクリプタ
 */
//-----------------------------------------------------------------//
void gpsio_service(FIL *fd)
{
	// SUP500F ボーレート設定プロセス
	if(g_gps_setup == 0) {
		gpsio_set_baudrate(BPS_57600);
		++g_gps_setup;
		return;
	} else if(g_gps_setup == 1) {
		++g_gps_setup;
		return;
	} else if(g_gps_setup == 2) {
		cli();
		uart_init(1, 57600);
		sei();
		++g_gps_setup;
		return;
	} else if(g_gps_setup == 3) {
		if(g_gps_interval == 0) {
			gpsio_set_rate10();
		}
		++g_gps_setup;
		return;
	}

	// 受信データが無い状態が１秒以上続いたら、ステータス・リセット
	if(uart_recv_length(1) == 0) {
		++g_recv_no_count;
		if(g_recv_no_count > 100) {	// 3 秒間データが無い場合はステータスをリセット
			g_recv_no_count = 0;
			g_gps_status = 0;
		}
		return;
	} else {
		g_recv_no_count = 0;
	}

	// GPS からのデータ読み込み
	while(uart_recv_length(1) > 0) {
		uint8_t ch = uart_recv(1);
		if(ch >= 0x20) {
			g_line_buff[g_line_pos] = (char)ch;
			++g_line_pos;
		} else if(ch == 0x0d) {
			g_line_buff[g_line_pos] = 0;

			if(gps_data_analize(g_line_buff)) {
				get_gpgsv(g_line_buff);
				if(g_satellites > 1) {
					get_gprmc(g_line_buff);
				} else {
					g_rmc_time = 0;
				}
				// ステータスは、サテライトの数が１より多く、RMC パラメーターが取得されている場合に有効とする
				if(g_satellites > 1 && g_rmc_time > 0) {
					g_gps_status = 1;

					if(fd != NULL) {
						UINT bw;
						f_write(fd, g_line_buff, strlen(g_line_buff), &bw);
						char crlf[2];
						crlf[0] = '\r';
						crlf[1] = '\n';
						f_write(fd, crlf, 2, &bw);
					}

					// 時間合わせは、サテライトの数が「TIME_ADJUST_SATELLITES」以上である事
					if(g_time_adjust == 0 && g_gps_status != 0 && g_satellites >= TIME_ADJUST_SATELLITES) {
						struct tm *sttm = gmtime(&g_rmc_time);
						rtc4543_put_time(sttm);
						if(g_gps_ext_echo) {
							xprintf(PSTR("Adjust RTC for G.P.S. "));
							xprint_date(sttm);
							uart_send_crlf(0);
						}
						g_time_adjust = 1;
					}
				}
			}
			g_line_pos = 0;
		}

		// ターミナルにエコーバックする場合
		if(g_gps_ext_echo) {
			uart_send(0, ch);
		}
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	GPS 状態の取得
	@return 「０」以外なら、GPS データ取得正常、「０」ならエラー
 */
//-----------------------------------------------------------------//
char gpsio_get_status(void)
{
	return g_gps_status;
}


//-----------------------------------------------------------------//
/*!
	@breif	GPS で取得した標準時間を返す@n
			GPS がアクティブでは無い場合「０」が返される。
	@return 時間（秒）
 */
//-----------------------------------------------------------------//
time_t gpsio_get_time(void)
{
	if(g_gps_status) {
		return g_rmc_time;
	} else {
		return 0;
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	GPS で取得した人工衛星の数を返す
	@return フォローしている人工衛星の数
 */
//-----------------------------------------------------------------//
uint8_t gpsio_get_satellites(void)
{
	if(g_gps_status) {
		return g_satellites;
	} else {
		return 0;
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	SUP500F のボーレートを設定
	@param[in]	bpsno	ボーレート番号
 */
//-----------------------------------------------------------------//
void gpsio_set_baudrate(enum SUP500F_BAUD bpsno)
{
	uart_send(1, 0xA0);
	uart_send(1, 0xA1);
	uart_send(1, 0x00);

	uart_send(1, 0x04);		// メッセージ数

	uart_send(1, 0x05);		// メッセージID
	uart_send(1, 0x00);		// COM port 0  以外はない
	uart_send(1, bpsno); 	// 0: 4800, 1: 9600, 2: 19200, 3: 38400, 4: 57600, 5: 115200
	uart_send(1, 0x00);		// 0: update to SRAM, 1: update to both SRAM & FLASH
	uint8_t sum = 0x05 ^ bpsno;
	uart_send(1, sum);

	uart_send(1, 0x0D);
	uart_send(1, 0x0A);
}


//-----------------------------------------------------------------//
/*!
	@breif	SUP500F の１０Ｈｚ測位の設定@n
			※事前にボーレートを高く設定する必要がある
 */
//-----------------------------------------------------------------//
void gpsio_set_rate10(void)
{
	uart_send(1, 0xA0);
	uart_send(1, 0xA1);
	uart_send(1, 0x00);

	uart_send(1, 0x03);

	uart_send(1, 0x0E);
	uart_send(1, 0x0A);
	uart_send(1, 0x00);
	uart_send(1, 0x04);

	uart_send(1, 0x0D);
	uart_send(1, 0x0A);
}


/* ----- End Of File "gpsio.c" ----- */
