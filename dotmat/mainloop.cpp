//=====================================================================//
/*!	@file
	@brief	ATMEGA168 ドット・マトリックス・メイン@n
			Y-driver data lo:		PB0   @n
			Sound-Pulse:			PB1 (OC1A) @n
			Y-driver clk:			PB2   @n
			Y-driver /stb:			PB3   @n
			SW-C:					PB4   @n
			SW-D:					PB5   @n
			CERALOCK (20MHz):		PB6   @n
			CERALOCK (20MHz):		PB7   @n
			X-decoder-a(138:A):		PC0   @n
			X-decoder-b(138:B):		PC1   @n
			X-decoder-c(138:C):		PC2   @n
			X-decoder-d(138:G,/G):	PC3   @n
			/RESET:					PC6   @n
			Y-driver data hi:		PD0   @n
									PD1   @n
			/X-decoder-gate:    	PD2 (with 47K pull-up)  @n
			SW-A:					PD3   @n
			SW-B:					PD4   @n
			Sound-Volume:			PD5 (OC0B) @n
			Sound: (PB1 & PD5)->(RC filter)->(Amp) @n
			R: 2200, C: 0.1uF @n
			・CERALOCK はクリスタルの方がベター（ヒューズビットの書き換え注意）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <stdlib.h>
#include "mainloop.hpp"
#include "ledout.hpp"
#include "monograph.hpp"
#include "switch.hpp"
#include "psound.hpp"
#include "menu.hpp"
#include "task.hpp"

//-----------------------------------------------------------------//
/*!
	@brief	AVR C++ new オペレーター
	@param[in]	size	割り当てサイズ
	@return 獲得ポインター
*/
//-----------------------------------------------------------------//
void * operator new(size_t size)
{
	return malloc(size);
}

//-----------------------------------------------------------------//
/*!
	@brief	AVR C++ delete オペレーター
	@param[in]	ptr	廃棄ポインター
*/
//-----------------------------------------------------------------//
void operator delete(void * ptr)
{
	free(ptr);
}

//-----------------------------------------------------------------//
/*!
	@brief	AVR C++ guard コンテキスト
*/
//-----------------------------------------------------------------//
__extension__ typedef int __guard __attribute__((mode (__DI__)));

//-----------------------------------------------------------------//
/*!
	@brief	AVR C++ guard acquire
	@param[in]	g	コンテキスト
	@return ハンドル
*/
//-----------------------------------------------------------------//
extern "C" int __cxa_guard_acquire(__guard *g);

//-----------------------------------------------------------------//
/*!
	@brief	AVR C++ guard release
	@param[in]	g	コンテキスト
*/
//-----------------------------------------------------------------//
extern "C" void __cxa_guard_release (__guard *g);

//-----------------------------------------------------------------//
/*!
	@brief	AVR C++ guard abort
	@param[in]	g	コンテキスト
*/
//-----------------------------------------------------------------//
extern "C" void __cxa_guard_abort (__guard *g);

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
void __cxa_guard_abort (__guard *) {};


//-----------------------------------------------------------------//
/*!
	@brief	AVR C++ 純粋仮想関数定義
*/
//-----------------------------------------------------------------//
extern "C" void __cxa_pure_virtual(void);

void __cxa_pure_virtual(void) {};

static device::ledout ledout_;
static app::task task_;


//-----------------------------------------------------------------//
/*!
	@brief	ATMEGA168P タイマー２ 割り込みタスク（タイマー２比較一致）
*/
//-----------------------------------------------------------------//
ISR(TIMER2_COMPA_vect)
{
	ledout_.reflesh();
}


//-----------------------------------------------------------------//
/*!
	@brief	タイマー２割り込みタスクと同期を取る@n
			1/100 秒単位で発生する割り込み
*/
//-----------------------------------------------------------------//
static uint8_t sync_system_timer_()
{
	ledout_.sync();

	uint8_t sw = 0;
	if((PIND & 0x08) == 0) sw |= 1;	// PD3
	if((PIND & 0x10) == 0) sw |= 2;	// PD4
	if((PINB & 0x10) == 0) sw |= 4;	// PB4
	if((PINB & 0x20) == 0) sw |= 8;	// PB5
	return sw;
}


//-----------------------------------------------------------------//
/*!
	@brief	ATMEGA168P I/O などの初期化プロセス
*/
//-----------------------------------------------------------------//
static void system_initialize_()
{
// ポート初期化
	PORTB = 0b00110000;
	DDRB  = 0b00001101;

	PORTC = 0b00000000;
	DDRC  = 0b00001111;

	PORTD = 0b00011000;
	DDRD  = 0b00000101;

//	ACSR = 0b01000100;

// Timer2: 8 ビットタイマー設定 (CTC)
// 20000000 / 128 / 960[Hz]
// ドットマトリックス LED のダイナミックスキャン周期
	OCR2A  = 163-1;
	OCR2B  = 0;
	TCCR2A = 0b00000010;	// 比較、一致クリア(CTC)
	TCCR2B = 0b00000101;	// 1/128 分周

// タイマー２割り込みマスク制御
	TIMSK2 |= (1 << OCIE2A);
}


//-----------------------------------------------------------------//
/*!
	@brief	メイン・ループ
*/
//-----------------------------------------------------------------//
int main()
{
	// システム初期化
	system_initialize_();

	task_.init();

	// 割り込み許可〜
	sei();

	sync_system_timer_();
	ledout_.enable();	///< LED 表示許可

	task_.start<app::menu>();	///< メニュー・タスクの登録

	while(1) {
		graphics::monograph& mng = task_.at_monograph();
		ledout_.copy(mng.fb());	///< フレームバッファを LED スキャンバッファにコピー
		mng.clear(0);			///< フレームバッファを消去

		task_.service();		///< 各タスクサービス

		uint8_t sw = sync_system_timer_();	///< フレームの同期とスイッチのサンプリング
		task_.at_switch().set_level(sw);	///< スイッチの状態を設定
	}
}
