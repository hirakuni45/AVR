//=====================================================================//
/*!	@file
	@breif	RTC-4543 ドライバー（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "rtc4543.h"
#include "share_port.h"

#include "xitoa.h"


#define RTC4543_PORT_INIT

#define RTC4543_CE_LO	{ decoder_disable(); }
#define RTC4543_CE_HI	{ decoder_select(DECODE_RTC4543); }
#define RTC4543_DATA_LO	{ share_pb1_lo(); }
#define RTC4543_DATA_HI	{ share_pb1_hi(); }
#define RTC4543_WR_LO	{ share_pa3_lo(); }
#define RTC4543_WR_HI	{ share_pa3_hi(); }
#define RTC4543_CLK_LO	{ share_pa4_lo(); }
#define RTC4543_CLK_HI	{ share_pa4_hi(); }

#define RTC4543_DATA	share_pb1_get()

#define RTC4543_DATA_IN 	{ share_pb1_inp(); }
#define RTC4543_DATA_OUT	{ share_pb1_out(); }

// 750ns delay loop at 19.6MHz
#define DELAY_750NS		20
// 150ns delay loop at 19.6MHz
#define DELAY_150NS		5

//-----------------------------------------------------------------//
/*!
	@breif	RTC の初期化
*/
 //-----------------------------------------------------------------//
void rtc4543_init()
{
	RTC4543_PORT_INIT;
	RTC4543_CE_LO;		// CE to negate '0'
}


static void rtc4543_read_sub(uint8_t *buff)
{
	unsigned char i, j, k;
	uint8_t a, b;

	RTC4543_DATA_IN;	///< DATA port input

	RTC4543_WR_LO;		// WR to negate '0' (Read mode)
	RTC4543_CLK_LO;		// CLK to '0'
	RTC4543_CE_HI;		// CE to assert '1'

	// wait for 0.75 micro sec.
	_delay_loop_1(DELAY_750NS);
	RTC4543_CLK_HI;

	for(i = 0; i < 7; ++i) {
		b = 0x01;
		if(i == 3) k = 4; else k = 8;
		a = 0;
		for(j = 0; j < k; ++j) {
			if(RTC4543_DATA) {
				a |= b;
			}
			b <<= 1;

			RTC4543_CLK_LO;		// CLK to '0'
			_delay_loop_1(DELAY_750NS);
			RTC4543_CLK_HI;		// CLK to '1'
			_delay_loop_1(DELAY_150NS);
		}
		*buff++ = a;
	}

	RTC4543_CE_LO;		// CE to negate '0'
}


//-----------------------------------------------------------------//
/*!
	@breif	RTC-4543SA レジスターの書き込み
	@param[in]	buff	書き込みデータ
*/
//-----------------------------------------------------------------//
void rtc4543_write(const uint8_t *buff)
{
	unsigned char i, j, k;
	uint8_t a;

	RTC4543_DATA_OUT;	///< DATA port output

	RTC4543_WR_HI;		// WR to assert '1' (Write mode)
	RTC4543_CLK_LO;		// CLK to '0'
	RTC4543_CE_HI;		// CE to assert '1'

	// wait for 0.75 micro sec.
	_delay_loop_1(DELAY_750NS);

	for(i = 0; i < 7; ++i) {
		if(i == 3) k = 4; else k = 8;
		a = *buff++;
		for(j = 0; j < k; ++j) {
			if(a & 1) {
				RTC4543_DATA_HI;
			} else {
				RTC4543_DATA_LO;
			}

			RTC4543_CLK_LO;		// CLK to '0'
			_delay_loop_1(DELAY_750NS);
			RTC4543_CLK_HI;		// CLK to '1'
			_delay_loop_1(DELAY_150NS - 1);

			a >>= 1;
		}
	}
	RTC4543_CE_LO;		// CE to negate '0'
}


//-----------------------------------------------------------------//
/*!
	@breif	RTC-4543SA レジスターの読み出し
	@param[in]	buff	読みだしたデータの保存先
*/
//-----------------------------------------------------------------//
void rtc4543_read(uint8_t *buff)
{
	uint8_t i;
	uint8_t t[7];

	do {
		rtc4543_read_sub(t);
		rtc4543_read_sub(buff);
		for(i = 0; i < 7; ++i) {
			if(t[i] != buff[i]) break;
		}
	} while(i != 7) ;
}


//-----------------------------------------------------------------//
/*!
	@breif	RTC-4543 レジスターから「time_tm」構造体に読み込む@n
			※ year カウンターは 100 までしかレンジが無い為、100@n
			年のオフセット（2000年）を追加する。
	@param[in]	tmp tm 構造体のポインター
*/
//-----------------------------------------------------------------//
void rtc4543_get_time(struct tm *tmp)
{
	unsigned char buf[7];

	rtc4543_read(buf);
																	// RTC4543  time_t
	tmp->tm_sec  = ((buf[0] >> 4) & 7)  * 10 + (buf[0] & 15);		// 0..59    0..59
	tmp->tm_min  = ((buf[1] >> 4) & 7)  * 10 + (buf[1] & 15);		// 0..59    0..59
	tmp->tm_hour = ((buf[2] >> 4) & 3)  * 10 + (buf[2] & 15);		// 0..23    0..23
	tmp->tm_wday =  (buf[3] & 7) - 1;								// 1..7     0..6
	tmp->tm_mday = ((buf[4] >> 4) & 3)  * 10 + (buf[4] & 15);		// 1..31    1..31
	tmp->tm_mon  = ((buf[5] >> 4) & 1)  * 10 + (buf[5] & 15) - 1;	// 1..12    0..11
	tmp->tm_year = ((buf[6] >> 4) & 15) * 10 + (buf[6] & 15) + 100;	// 0..99    100(2000)-199(2099)
//	xprintf(PSTR("Read week: %d\n"), tmp->tm_wday);
}


//-----------------------------------------------------------------//
/*!
	@breif	RTC-4543 レジスターに「time_tm」構造体から書き込む@n
			※ year は 100 までしかレンジが無い為、100 の余りが@n
			書き込まれる。
	@param[in]	tmp tm 構造体のポインター
*/
//-----------------------------------------------------------------//
void rtc4543_put_time(const struct tm *tmp)
{
	unsigned char buf[7];
	int v;

	buf[0] = ((tmp->tm_sec  / 10) << 4) | (tmp->tm_sec  % 10);
	buf[1] = ((tmp->tm_min  / 10) << 4) | (tmp->tm_min  % 10);
	buf[2] = ((tmp->tm_hour / 10) << 4) | (tmp->tm_hour % 10);

	buf[3] = tmp->tm_wday + 1;										// 曜日設定
	buf[4] = ((tmp->tm_mday / 10) << 4) | (tmp->tm_mday % 10);
	v = tmp->tm_mon + 1;
	buf[5] = ((v / 10) << 4) | (v % 10);
	v = tmp->tm_year % 100;
	buf[6] = ((v / 10) << 4) | (v % 10);

	rtc4543_write(buf);
}

