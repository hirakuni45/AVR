//=====================================================================//
/*!	@file
	@breif	ATmega1284P ベース制御基板メンイ
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/wdt.h>
#include "mainloop.h"
#include "uart.h"
#include "share_port.h"
#include "rtc4543.h"
#include "lcdio.h"
#include "mcp3208.h"
#include "monograph.h"
#include "mobj.h"
#include "fileio.h"
#include "title.h"
#include "menu.h"
#include "laptimer.h"
#include "lowbattery.h"
#include "gpsio.h"
#include "xitoa.h"
#include "datalogger.h"
#include "shell.h"
#include "eeprom_man.h"
#include "speed.h"
#include "psound.h"
#include "config.h"
#include "puls.h"

//.....................................................................//
/*
		Master clock: 19.6608MHz							@n
															@n
		PA0(40) : ---> 74AC138 A (1)						@n
		PA1(39) : ---> 74AC138 B (2)						@n
		PA2(38) : ---> 74AC138 C (3)						@n
		PA3(37) : ---> LCD D/I, RTC4543 WR,  MCP3208 Din	@n
		PA4(36) : ---> LCD E,   RTC4543 CLK, MCP3208 CLK	@n
		PA5(35) : 											@n
		PA6(34) : 											@n
		PA7(33) :											@n
															@n
		PB0( 1) : ---> Switch Common Gate					@n
		PB1( 2) : <--> RTC4543 DATA, MCP3208 Dout			@n
		PB2( 3) : <--- MMC/SD Insert (Active-L)				@n
		PB3( 4) : ---> MMC/SD Power  (Active-L)				@n
		PB4( 5) : ---> MMC/SD-DA3/CS						@n
		PB5( 6) : ---> MMC/SD-CMD/DI (MOSI)					@n
		PB6( 7) : <--- MMC/SD-DA0/DO (MISO)					@n
		PB7( 8) : ---> MMC/SD-CLK    (SCK)					@n
															@n
		PC0(22) : ---> LCD DB0, KEY-A,   SW-LAP				@n
		PC1(23) : ---> LCD DB1, KEY-B,   SW-Cancel			@n
		PC2(24) : ---> LCD DB2, KEY-C,   SW-Up				@n
		PC3(25) : ---> LCD DB3, KEY-IN0, SW-Down			@n
		PC4(26) : ---> LCD DB4, KEY-IN1, SW-Pitin			@n
		PC5(27) : ---> LCD DB5, KEY-IN2						@n
		PC6(28) : ---> LCD DB6, KEY-IN3						@n
		PC7(29) : ---> LCD DB7, KEY_IN4						@n
															@n
		PD0(14) : RXD0 (for serial terminal)				@n
		PD1(15) : TXD0 (for serial terminal)				@n
		PD2(16) : RXD1 (for GPS)							@n
		PD3(17) : TXD1 (for GPS)							@n
		PD4(18) : <--- Ext Battery Signal (10K at Power)	@n
		PD5(19) : ---> Puls Sound Output					@n
		PD6(20) : <--- Speed/RPM trigger input				@n
		PD7(21)	: ---> Battery Power Gate (Active Low)		@n
															@n
	74AC138 Decoder output sections:						@n
		74AC138(15) Y0 ---> LCD CS1 (15) [inverted]			@n
		74AC138(14) Y1 ---> LCD CS2 (16) [inverted]			@n
		74AC138(13) Y2 ---> LCD /RES (17)					@n
		74AC138(12) Y3 ---> MCP3208 /CS						@n
		74AC138(11) Y4 ---> RTC4543 CE [inverted]			@n
		74AC138(10) Y5 ---> VS1011e	MP3 decoder				@n
		74AC138(9)  Y6 ---> KEY-BOARD decoder				@n
		74AC138(7)  Y7 N.C. (Y0 to Y6 disable)
*/
//.....................................................................//

// 描画ループカウント
#define DRAW_LOOP_COUNT		3

static void (*g_main_task)(void) = NULL;

static unsigned char *g_fb;

static volatile uint32_t g_timer_count;
static volatile uint8_t g_sync_count;
static uint8_t g_draw_div;
static volatile uint8_t g_draw_count;

static volatile uint8_t g_switch_value;
uint8_t g_switch_level;
uint8_t g_switch_positive;
uint8_t g_switch_negative;

unsigned char g_loop_count;

static struct tm g_time_st;

static char g_fb_clear;

volatile uint32_t g_capture_value;
volatile uint8_t g_capture_overflow;
volatile uint32_t g_capture_count;
volatile uint8_t g_capture_sync;

static uint8_t g_powerdown_loop;

uint8_t g_ext_battery;

//-----------------------------------------------------------------//
/*!
	@breif	ラップタイマー値を取得
	@return 1/100 秒単位のラップタイマー値
*/
//-----------------------------------------------------------------//
uint32_t get_laptimer(void)
{
	return g_timer_count;
}


//-----------------------------------------------------------------//
/*!
	@breif	デバイスの基本的な初期化プロセス
*/
//-----------------------------------------------------------------//
static void system_initialize(void)
{
	g_ext_battery = 0;

	share_port_init();

/* ポート設定 */
	PORTD |= _BV(0);			// Switch Gate Negate(H)
	DDRB  |= _BV(0);			// PB0 output

	PORTD |= 0b00001010;		// TXD をプルアップで出力ノイズが出ないようにする。
	DDRD  |= _BV(5) | _BV(7);	// puls sound output, battery gate output

/* A/D 変換設定 */
//	ADMUX  = 0b11000111;	// 内部基準電圧 2.56V、ADC7 input
//	ADCSRA = 0b10000111;	// A/D Enable, 1/128 clock divider(153.6KHz)
//	DIDR0  = 0b10000000;	// デジタル入力禁止

/* 8 ビットタイマー設定 */
	OCR0A  = 192 - 1;		// 100.0Hz (OSC: 19.6608MHz)
	TCCR0A = 0b00000010;	// 比較、一致クリア(CTC)
	TCCR0B = 0b00000101;	// 比較、一致クリア(CTC), 1/1024プリスケーラー
	TIMSK0 |= (1 << OCIE0A);
}


//-----------------------------------------------------------------//
/*!
	@breif	タイマー０割り込みタスク@n
			８ビットタイマー割り込み（100Hz）
*/
//-----------------------------------------------------------------//
ISR(TIMER0_COMPA_vect)
{
	DDRC  = 0x00;	// PORT-C input;
	PORTC = 0xff;	// 全て pull-up（使ってるのは下位５ビット）

	// スイッチのゲートを「assert」して、結果が返るまで、少し時間がかかるので
	// 間で他のサービスを呼んで時間を稼ぐ
	PORTB &= ~_BV(0);	// Switch gate assert(L)

	++g_timer_count;
	++g_sync_count;

	mcp3208_int_service();

	disk_timerproc();

	uint8_t p = ~PINC;
	char pit =  0;
	if((p & SWITCH_BIT_PIT) != 0 && (g_switch_value & SWITCH_BIT_PIT) == 0) pit = 1;	// Positive
	else if((p & SWITCH_BIT_PIT) == 0 && (g_switch_value & SWITCH_BIT_PIT) != 0) pit = 2;	// Negative
	// ピットイン時は、ラップスイッチは無効
	if((p & SWITCH_BIT_PIT)==0) {
		if((p & SWITCH_BIT_LAP) != 0 && (g_switch_value & SWITCH_BIT_LAP) == 0) pit |= 2;	// Positive
	}
	if(pit) {
		laptimer_int_service(pit & 1);
	}
	g_switch_value = p;

	PORTB |= _BV(0);	// Switch gate negate(H)

	// 描画ループのカウンタ 100 / 3 Hz
	if(g_draw_div < DRAW_LOOP_COUNT) {
		++g_draw_div;
	} else {
		g_draw_div = 0;
		++g_draw_count;
	}

	// パルス・サウンド・サービス
	psound_service();

	g_ext_battery = PIND & _BV(4);
	if(g_ext_battery) {
		PORTD &= ~_BV(7);
	} else {
		PORTD |=  _BV(7);
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	インプット・キャプチャー・モードにする。
	@param[in]	enable 許可する場合「０」以外
*/
//-----------------------------------------------------------------//
void setup_input_capture(char enable)
{
// 16 ビットタイマー設定（インプットキャプチャー設定）
	if(enable) {
		TCCR1A = 0b00000000;
		TCCR1B = 0b11000011;	// 1/64 clock プリスケーラー
		TCCR1C = 0b00000000;
		TIMSK1 |= (1 << ICIE1) | (1 << TOIE1);
	} else {
		TIMSK1 = 0;
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	タイマー１割り込みタスク
			１６ビットタイマー・キャプチャー（１）
*/
//-----------------------------------------------------------------//
ISR(TIMER1_CAPT_vect)
{
	TCNT1 = TCNT1 - ICR1;
	g_capture_value = ICR1;
	g_capture_value |= (uint32_t)g_capture_overflow << 16;
	g_capture_overflow = 0;
	++g_capture_sync;
	++g_capture_count;
}


//-----------------------------------------------------------------//
/*!
	@breif	タイマー１割り込みタスク
			１６ビットタイマー・オーバーフロー（１）
*/
//-----------------------------------------------------------------//
ISR(TIMER1_OVF_vect)
{
	++g_capture_overflow;
}


//-----------------------------------------------------------------//
/*!
	@breif	タイマー割り込みタスクと同期を取る(100Hz)
*/
//-----------------------------------------------------------------//
void sync_timer(void)
{
	volatile uint8_t cnt;

	cnt = g_sync_count;
	while(g_sync_count == cnt) {
		sleep_mode();
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	タイマー割り込みタスクと同期を取る(100Hz)
*/
//-----------------------------------------------------------------//
void sync_draw(void)
{
	volatile uint8_t cnt;

	cnt = g_draw_count;
	while(g_draw_count == cnt) {
		sleep_mode();
	}
}

inline void uart_send_task(uint8_t ch) { uart_send(0, ch); }

//-----------------------------------------------------------------//
/*!
	@breif	制御メイン
*/
//-----------------------------------------------------------------//
int main(void)
{
	system_initialize();

	lcdio_init();
	g_fb = monograph_init();
	rtc4543_init();
	mcp3208_init();

	monograph_clear(0);
	lcdio_copy(g_fb);

	load_configuration();

	fileio_init();
	uart_init(0, 57600);	// Chanel-0: terminal for PC

	eeprom_man_init(EEPROM_LAP_STREAM_TOP, EEPROM_LAP_STREAM_LEN);
	psound_init();

	xfunc_out = uart_send_task;

	gpsio_init();

	g_capture_sync = 0;
	g_capture_value = 0;
	g_capture_overflow = 0;
	g_capture_count = 0;

	g_powerdown_loop = 0;

	// ウォッチ・ドッグ
	wdt_enable(WDTO_2S);

	// 割り込み有効～
	sei();

	shell_init();
	wdt_reset();

	datalogger_init();
	wdt_reset();

	install_task(title_init);

	puls_service_init();

	wdt_reset();

	// 描画ループ
	uint8_t rtc_cnt = 15;	// 初期早々に時間を読む
	while(1) {
		sync_draw();
		lcdio_copy(g_fb);

		// RTC のデータ読み込み、１秒辺り約５回
		if(rtc_cnt >= 7) {
			rtc4543_get_time(&g_time_st);
			rtc_cnt = 0;
		} else {
			++rtc_cnt;
		}

		uint8_t v = g_switch_level;
		g_switch_level = g_switch_value;
		g_switch_positive =  g_switch_level & ~v;
		g_switch_negative = ~g_switch_level &  v;

		// 以下のサービスは、優先度が高いので、画面同期の直ぐ後に実行
		// A/D サービス
		mcp3208_service(g_fd);

		// GPS サービス
		gpsio_service(g_fd);

		if(g_fb_clear) monograph_clear(0);

		if(g_main_task != NULL) (*g_main_task)();
		++g_loop_count;

		// パルス入力関係サービス
		puls_service_main();


		// SD カードのマウントなどのサービス
		fileio_service();

		// データ・ロガー・サービス
		datalogger_service();

		// shell サービス
		shell_service();

		// バッテリー管理
		if(get_battery() > BATTERY_LOW_LIMIT) {
			if((g_switch_level & (SWITCH_BIT_CANCEL | SWITCH_BIT_UP | SWITCH_BIT_DOWN))
			   == (SWITCH_BIT_CANCEL | SWITCH_BIT_UP | SWITCH_BIT_DOWN)) {
				++g_powerdown_loop;
				if(g_powerdown_loop > 99) {
					cli();
					set_sleep_mode(SLEEP_MODE_PWR_DOWN);
					while(1) {
						PORTD &= ~_BV(7);
						sleep_mode();
					}
				}
			} else {
				g_powerdown_loop = 0;
			}
		}

		wdt_reset();
	}

	return 0;
}


//-----------------------------------------------------------------//
/*!
	@breif	フレーム・バッファの消去を制御
	@param[in]	val	「０」の場合消去しない、「０」以外は消去
*/
//-----------------------------------------------------------------//
void set_fb_clear(char val)
{
	g_fb_clear = val;
}


//-----------------------------------------------------------------//
/*!
	@breif	新規タスク関数登録
	@param[in]	task	新規タスク関数アドレス
*/
//-----------------------------------------------------------------//
void install_task(void (*task)(void))
{
	g_main_task = task;
}


//-----------------------------------------------------------------//
/*!
	@breif	現在の時刻を返す
	@return	tm 構造体のポインター
*/
//-----------------------------------------------------------------//
const struct tm *get_time(void)
{
	return &g_time_st;
}


//-----------------------------------------------------------------//
/*!
	@breif	User Provided Timer Function for FatFs module@n
			This is a real time clock service to be called from@n
			FatFs module. Any valid time must be returned even if@n
			the system does not support a real time clock.
	@return	FatFs - module 用の時間データ
*/
//-----------------------------------------------------------------//
DWORD get_fattime(void)
{
#if 0
	return	((2010UL-1980) << 25)	// Year = 2010
			| (2UL << 21)			// Month = Feb
			| (9UL << 16)			// Day = 9
			| (22U << 11)			// Hour = 22
			| (30U << 5)			// Min = 30
			| (0U >> 1)				// Sec = 0
			;
#else
	struct tm *tmr;
	tmr = &g_time_st;
	return	  ((DWORD)(tmr->tm_year - 80) << 25)
			| ((DWORD)(tmr->tm_mon + 1) << 21)
			| ((DWORD)tmr->tm_mday << 16)
			| (WORD)(tmr->tm_hour << 11)
			| (WORD)(tmr->tm_min << 5)
			| (WORD)(tmr->tm_sec >> 1);
#endif
}


//-----------------------------------------------------------------//
/*!
	@breif	fat date/time を struct tm 形式に変換@n
			曜日は計算されない
	@param[in]	date	FILEINFO.fdate 形式
	@param[in]	time	FILEINFO.fdtime 形式
	@param[in]	t		struct tm 形式
*/
//-----------------------------------------------------------------//
void fattime_to_tm(WORD date, WORD time, struct tm *t)
{
	t->tm_year = (date >> 9) + 80;
	t->tm_mon  = ((date >> 5) & 0xf) - 1;
	t->tm_mday = (date & 0x1f);

	t->tm_hour = (time >> 11);
	t->tm_min  = (time >> 5) & 0x3f;
	t->tm_sec  = (time & 0x1f) << 1;
}


/* ----- End Of File "mainloop.c" ----- */
