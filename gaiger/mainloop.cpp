//=====================================================================//
/*!	@file
	@breif	ATMEGA328P ガイガーカウンター・メイン@n
	PB0       - @n
	PB1(OC1A) - BUZZER  @n
	PB2(OC1B) - @n
	PB3       - @n
	PB4       - @n
	PB5       - @n
	PB6       - 12MHz X-TAL @n
	PB7       - 12MHz X-TAL @n
	PC0       - @n
	PC1       - @n
	PC2       - LCD CS1 @n
	PC3       - LCD CS2 @n
	PC4       - LCD RS  @n
	PC5       - LCD E   @n
	PC6       - /RESET   @n
	PD0       - LCD DB0 @n
	PD1       - LCD DB1 @n
	PD2       - LCD DB2 @n
	PD3       - LCD DB3 @n
	PD4       - LCD DB4 @n
	PD5       - LCD DB5 @n
	PD6       - LCD DB6 @n
	PD7       - LCD DB7 @n
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "mainloop.hpp"
#include "monograph.hpp"
#include "lcdio.hpp"
#include "psound.hpp"

static void (*main_task_)() = 0;
static bool fb_clear_ = false;

static system::timer timer_;
static graphics::monograph monog_;
static device::psound psound_;

//-----------------------------------------------------------------//
/*!
	@breif	ATMEGA328P タイマー0 割り込みタスク（タイマー０比較一致）
*/
//-----------------------------------------------------------------//
ISR(TIMER2_COMPA_vect)
{
	timer_.service();
}


//-----------------------------------------------------------------//
/*!
	@breif	ATMEGA328P I/O などの初期化プロセス
*/
//-----------------------------------------------------------------//
static void system_initialize()
{
//	analog compalator status register
//	ACSR = 0b01000100;

// ポートプログラミング
//	PORTB = 0b00110000;
//	DDRB  = 0b00001111;

//	PORTC = 0b00000000;
//	DDRC  = 0b00001111;

//	PORTD = 0b00011000;
//	DDRD  = 0b00000100;

//	DDRD  = 0b00100000;
//	DDRE = 0x80;

// Timer0: 8 ビットタイマー設定 (高速PWM）
//	OCR0A = 128;
//	OCR0B = 8;
//	TCCR0A = 0b00100011;	// 高速 PWM 出力 OC1B (PB2)
//	TCCR0B = 0b00001001;	// 1/1 clock プリスケーラ－

// Timer1: 16 ビットタイマー設定
//	TCCR1A = 0b00000000;
//	TCCR1B = 0b11000011;	// 1/64 clock プリスケーラー

// Timer2: 8 ビットタイマー設定 (CTC)
// 19660800 / 1024 / 120[Hz]
    OCR2A = 160-1;
    OCR2B = 0;
    TCCR2A = 0b00000010;    // 比較、一致クリア(CTC)
    TCCR2B = 0b00000111;    // 1/1024 分周

// タイマー２割り込みマスク制御
	TIMSK2 |= (1 << OCIE2A);
}


//-----------------------------------------------------------------//
/*!
	@breif	メイン・ループ
*/
//-----------------------------------------------------------------//
int main(void)
{
	// システム初期化
	system_initialize();

	device::lcdio::init();

	monog_.init();
	monog_.clear(0x00);
//	monog_.line(0, 0, 127, 63, 1);
//	monog_.line(127, 0, 0, 63, 1);
	device::lcdio::copy(monog_.fb());

	psound_.init();

	// 割り込み許可〜
	sei();

	fb_clear_ = true;

	psound_.request(20, 10);

	while(1) {
		timer_.sync_draw();

		device::lcdio::copy(monog_.fb());

		if(fb_clear_) monog_.clear(0x00);

///		if(main_task_ != 0) (*main_task_)();

		psound_.service();
	}
}


//-----------------------------------------------------------------//
/*!
    @breif  新規タスク関数登録
    @param[in]  task    新規タスク関数アドレス
*/
//-----------------------------------------------------------------//
void install_task(void (*task)(void))
{
    main_task_ = task;
}

