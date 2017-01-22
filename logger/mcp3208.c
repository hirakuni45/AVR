//=====================================================================//
/*!	@file
	@breif	MCP3208(A/D Converter) の制御モジュール
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "mcp3208.h"
#include "mainloop.h"
#include "share_port.h"
#include "fileio.h"

//	AC138 Y3 - /CS
//	PA4 - CLK
//  PA3 - DOut
//  PB1 - DIn

// clock signal hold time at 19.6MHz
#define CLOCK_DELAY		10
#define SAMPLE_DELAY	15

#define	PORT_INIT

#define CS_LOW		{ decoder_select(DECODE_MCP3208); }
#define CS_HIGH		{ decoder_disable(); }
#define CLK_LOW		{ share_pa4_lo(); }
#define CLK_HIGH	{ share_pa4_hi(); }
#define DATA_LOW	{ share_pa3_lo(); }
#define DATA_HIGH	{ share_pa3_hi(); }
#define DATA_TEST	share_pb1_get()
#define DATA_TEST_INIT	share_pb1_inp()

static uint16_t g_analog_value[8];

#define ANALOG_BUFF_UNIT	7

typedef struct _fifo {
	uint8_t	put_pos;
	uint8_t	get_pos;
	uint16_t value[ANALOG_BUFF_UNIT * 7];
} ANALOG_T;

static volatile ANALOG_T g_fifo;
static volatile char g_fifo_enable;
static uint32_t g_count_org;
static uint8_t g_batt_count;
static uint16_t g_batt_voltage;

//-----------------------------------------------------------------//
/*!
	@breif	MCP3208 指定のチャネルをA/D変換
	@param[in]	chanel	チャネル
	@return 12ビットの変換データ
*/
//-----------------------------------------------------------------//
uint16_t read_chanel(unsigned char chanel)
{
	CS_LOW;
// start bit(H)
	DATA_HIGH;
	_delay_loop_1(CLOCK_DELAY);
	CLK_HIGH;
	_delay_loop_1(CLOCK_DELAY);
	CLK_LOW;
// single(H)/diff(L)
	DATA_HIGH;
	_delay_loop_1(CLOCK_DELAY);
	CLK_HIGH;
	_delay_loop_1(CLOCK_DELAY);
	CLK_LOW;
// chanel bit 2
	if(chanel & 4) { DATA_HIGH; } else { DATA_LOW; }
	_delay_loop_1(CLOCK_DELAY);
	CLK_HIGH;
	_delay_loop_1(CLOCK_DELAY);
	CLK_LOW;
// chanel bit 1
	if(chanel & 2) { DATA_HIGH; } else { DATA_LOW; }
	_delay_loop_1(CLOCK_DELAY);
	CLK_HIGH;
	_delay_loop_1(CLOCK_DELAY);
	CLK_LOW;
// chanel bit 0
	if(chanel & 1) { DATA_HIGH; } else { DATA_LOW; }
	_delay_loop_1(CLOCK_DELAY);
	CLK_HIGH;
	_delay_loop_1(CLOCK_DELAY);
	CLK_LOW;
// sample wait dummy
	_delay_loop_1(CLOCK_DELAY);
	CLK_HIGH;
	_delay_loop_1(CLOCK_DELAY);
	CLK_LOW;
// null bit
	_delay_loop_1(CLOCK_DELAY);
	CLK_HIGH;
	_delay_loop_1(CLOCK_DELAY);
	CLK_LOW;

	uint16_t v = 0;
	char i;
	for(i = 0; i < 12; ++i) {
		_delay_loop_1(CLOCK_DELAY);
		CLK_HIGH;
		_delay_loop_1(CLOCK_DELAY);
		v <<= 1;
		if(DATA_TEST) v |= 1;
		CLK_LOW;
	}

	CS_HIGH;

	return v;
}


//-----------------------------------------------------------------//
/*!
	@breif	MCP3208 ハードウェアー関係の初期化など
*/
//-----------------------------------------------------------------//
void mcp3208_init(void)
{
	PORT_INIT;

	g_fifo.put_pos = 0;
	g_fifo.get_pos = 0;

	g_fifo_enable = 0;

	g_batt_count = 0;
	g_batt_voltage = 0;
}


//-----------------------------------------------------------------//
/*!
	@breif	MCP3208 割り込みから呼ばれる
*/
//-----------------------------------------------------------------//
void mcp3208_int_service()
{
	uint8_t i;

	DATA_TEST_INIT;

	CLK_LOW;
	_delay_loop_2(CLOCK_DELAY);
	for(i = 0; i < 8; ++i) {
		g_analog_value[i] = read_chanel(i);
		_delay_loop_1(SAMPLE_DELAY);
	}

	if(g_fifo_enable == 0) return;

	uint8_t pos = g_fifo.put_pos;
	for(i = 0; i < 7; ++i) {
		g_fifo.value[pos * 7 + i] = g_analog_value[i];
	}
	++pos;
	if(pos >= ANALOG_BUFF_UNIT) pos = 0;
	g_fifo.put_pos = pos;
}


//-----------------------------------------------------------------//
/*!
	@breif	MCP3208 全チャネル A/D 変換サービス
	@param[in]	fd	ファイル・ディスクリプタ
*/
//-----------------------------------------------------------------//
void mcp3208_service(FIL *fd)
{
	uint8_t i;

	if(fd != NULL) {
		uint8_t pos = g_fifo.get_pos;
		while(g_fifo.put_pos != pos) {
			char txt[64];
			sprintf_P(txt, PSTR("%%ADC,"));
			UINT bw;
			f_write(fd, txt, strlen(txt), &bw);
			for(i = 0; i < 7; ++i) {
				sprintf_P(txt, PSTR(",%d"), g_fifo.value[pos * 7 + i]);
				f_write(fd, txt, strlen(txt), &bw);
			}
			sprintf_P(txt, PSTR("\r\n"));
			f_write(fd, txt, strlen(txt), &bw);

			++pos;
			if(pos >= ANALOG_BUFF_UNIT) pos = 0;
		}
		g_fifo.get_pos = pos;
	}

	// バッテリー電圧のサンプリング
	++g_batt_count;
	if(g_batt_count >= 66) {
		g_batt_count = 0;
		g_batt_voltage = g_analog_value[7];
	}
	if(g_batt_voltage < g_analog_value[7]) g_batt_voltage = g_analog_value[7];
}


//-----------------------------------------------------------------//
/*!
	@breif	MCP3208 の A/D 変換データを得る
	@param[in]	chanel	A/D 変換チャネル番号（0〜7）
	@return		A/D 値（0〜4095）
*/
//-----------------------------------------------------------------//
uint16_t mcp3208_get(char chanel)
{
	return g_analog_value[chanel & 7];
}


//-----------------------------------------------------------------//
/*!
	@breif	A/D 変換 FIFO サービスの許可
	@param[in]	enable	許可の場合「０」以外
*/
//-----------------------------------------------------------------//
void mcp3208_fifo_enable(char enable)
{
	if(enable == 0) {
		g_fifo.get_pos = g_fifo.put_pos = 0;
	} else {
		g_count_org = get_laptimer();
	}
	g_fifo_enable = enable;
}


//-----------------------------------------------------------------//
/*!
	@breif	バッテリー電圧の取得
	@return	バッテリー電圧
*/
//-----------------------------------------------------------------//
uint16_t get_battery(void)
{
	return g_batt_voltage;
}

/* ----- End Of File "mcp3208.c" ----- */
