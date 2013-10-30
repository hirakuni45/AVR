//=====================================================================//
/*!	@file
	@breif	ATMEGA88 ドット・マトリックス・メイン@n
			 Y-driver data lo: PB0
			 Y-driver data hi: PB1
			 Y-driver clk:     PB2
			 Y-driver /stb:    PB3
			 X-decoder-a:      PC0
			 X-decoder-b:      PC1
			 X-decoder-c:      PC2
			 X-decoder-d:      PC3
			/X-decoder-gate:   PD2 (with 10K pull-up)
			 SW-A:             PD3
			 SW-B:             PD4
			 SW-C:             PB4
			 SW-D:			   PB5
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "mainloop.hpp"
#include "ledout.hpp"
#include "monograph.hpp"
#include "switch.hpp"
#include "psound.hpp"
#include "tetris.hpp"

static volatile uint8_t sw_input_;
static device::ledout ledout_;
static device::psound psound_;
static graphics::monograph mng_;

static void (*main_task_)(void) = 0;

//-----------------------------------------------------------------//
/*!
	@breif	ATMEGA88P タイマー0 割り込みタスク（タイマー０比較一致）
*/
//-----------------------------------------------------------------//
ISR(TIMER2_COMPA_vect)
{
	ledout_.reflesh();
	ledout_.count();
}


//-----------------------------------------------------------------//
/*!
	@breif	タイマー２割り込みタスクと同期を取る@n
			1/100 秒単位で発生する割り込み
*/
//-----------------------------------------------------------------//
void sync_system_timer()
{
	ledout_.sync();

	uint8_t sw = 0;
	if((PIND & 0x08) == 0) sw |= 1;	// PD3
	if((PIND & 0x10) == 0) sw |= 2;	// PD4
	if((PINB & 0x10) == 0) sw |= 4;	// PB4
	if((PINB & 0x20) == 0) sw |= 8;	// PB5
	sw_input_ = sw;
}


//-----------------------------------------------------------------//
/*!
	@breif	ATMEGA88P I/O などの初期化プロセス
*/
//-----------------------------------------------------------------//
static void system_initialize()
{
//	ACSR = 0b01000100;

// ポートプログラミング
	PORTB = 0b00110000;
	DDRB  = 0b00001111;

	PORTC = 0b00000000;
	DDRC  = 0b00001111;

	PORTD = 0b00011000;
	DDRD  = 0b00000100;

//	DDRD  = 0b00100000;
//	DDRE = 0x80;

// Timer0: 8 ビットタイマー設定 (高速PWM）
//	OCR0A = 128;
//	OCR0B = 8;
//	TCCR0A = 0b00100011;	// 高速 PWM 出力 OC1B (PB2)
//	TCCR0B = 0b00001001;	// 1/1 clock プリスケーラ－

	psound_.init();

// Timer2: 8 ビットタイマー設定 (CTC)
// 20000000 / 128 / 960[Hz]
	OCR2A = 163-1;
	OCR2B = 0;
	TCCR2A = 0b00000010;	// 比較、一致クリア(CTC)
	TCCR2B = 0b00000101;	// 1/128 分周

// タイマー２割り込みマスク制御
	TIMSK2 |= (1 << OCIE2A);
}

/// static const prog_char message_[] PROGMEM = { "Romeo And Cinderella / Doriko / Strap / Hatune Miku  " };


//-----------------------------------------------------------------//
/*!
	@breif	メイン・ループ
*/
//-----------------------------------------------------------------//
int main()
{
	// システム初期化
	system_initialize();

	// 割り込み許可〜
	sei();

	sync_system_timer();
	PORTD &= ~0b00000100;	// Y-Decoder enable.

	system::switch_input swi;

	game::tetris tet(mng_);

	tet.init();

	while(1) {
		ledout_.copy(mng_.fb());

		 if(main_task_ != NULL) (*main_task_)();

		mng_.clear(0);

		tet.service(swi);

///		mng.draw_string_P(x, 2, message_);

///		++n;
//		if(n >= 5) --x;
///		if(swi.get_positive() & switch_input::RIGHT_DOWN) --x;
///		if(swi.get_positive() & switch_input::LEFT_DOWN) ++x;

///		{
///			n = 0;
///			short l = strlen_P(message_);
///			if(x < -(l * 6)) x = 16;
///		}

		sync_system_timer();
		swi.set_level(sw_input_);
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

