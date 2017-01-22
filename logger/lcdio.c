//=====================================================================//
/*!	@file
	@breif	液晶（LCD）ハードウェアーの制御@n
			KS108B LCD モジュール（128×64）@n
			※コントローラーが同等品なので、同じように扱える@n
			・サイキック販売版@n
			http://www.xikit.com/store/index.php?main_page=index&cPath=11@n
			・ストロベリーリナックス版@n
			http://strawberry-linux.com/catalog/items?code=18028@n
			・秋月電子版@n
			http://akizukidenshi.com/catalog/items2.php?q=%22P-02159%22&s=name&p=1&r=0&page=@n
			※液晶を外した場合にもプログラムが停止しないようにする為@n
			ステータスの確認を行わないで、プログラムループで少し長めに待つ。@n
			「F_CPU」を必ず設定する事
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "share_port.h"
#include "lcdio.h"

//  74AC138 Y0 ---> LCD CS1
//  74AC138 Y1 ---> LCD CS2
//  74AC138 Y2 ---> LCD /RES

//	Low       ---> LCD R/W (Every 'Write' Mode)
//	PA3(37) : ---> LCD D/I(RS)
//	PA4(36) : ---> LCD E
//  PC0-PC7 : ---> LCD DB0-DB7

#define INIT_PORT_DIRECTION		{ DDRC = 0xff; }
#define LCD_DATA(x)				{ PORTC = x; }
#define LCD_RS_LO				{ share_pa3_lo(); }
#define LCD_RS_HI				{ share_pa3_hi(); }
#define LCD_E_LO				{ share_pa4_lo(); }
#define LCD_E_HI				{ share_pa4_hi(); }

// at 19.6MHz
#define SETUP_TIME		5
#define HOLD_TIME		22

/*------------------------------------------------------/
/	KS108B LCD Drive Interface Command					/
/......................................................./
/	RS: 0 ---> Ctrl register.							/
/	RS: 1 ---> Data register.							/
/......................................................./
/	Ctrl Register(write):								/
/		Display ON: 0x3f, OFF: 0x3e						/
/		Set Address Y: 0x40 + (0 to 63)					/
/		Set Address X: 0xB8 + (0 to 7)					/
/		Display Strat Line: 0xC0 + ( 0 to 63)			/
/	Ctrl Register(read):								/
/		B7 - BUSY (0: ready, 1:In operation)			/
/		B5 - Display On/Off (0: ON, 1: OFF)				/
/		B4 - Reset (0: Nomal, 1: Reset)					/
/......................................................./
/	Data Register(write):								/
/		Writes data into display data RAM.				/
/		After writing intruction, Y address is 			/
/		increased by 1 automatically.					/
/------------------------------------------------------*/
static void lcdio_write(unsigned char cmd)
{
	LCD_DATA(cmd);
	_delay_loop_1(SETUP_TIME);
	LCD_E_HI;
	_delay_loop_1(HOLD_TIME);
	LCD_E_LO;
	_delay_loop_1(HOLD_TIME);
}


//-----------------------------------------------------------------//
/*!
	@breif	LCD ハードウェアー関係の初期化など
*/
//-----------------------------------------------------------------//
void lcdio_init(void)
{
	INIT_PORT_DIRECTION;

	LCD_RS_LO;
	LCD_E_LO;
	decoder_select(DECODE_LCD_RES);	// LCD reset assert
	_delay_ms(1);					// 1ms reset signal
	decoder_disable();				// LCD reset negate
	_delay_ms(100);					// 100ms reset cycle

	LCD_RS_LO;
	decoder_select(DECODE_LCD_CS1);
	lcdio_write(0x3e);	// Display OFF
	_delay_ms(1);
	lcdio_write(0xc0);	// Display start line 0
	_delay_ms(1);
	lcdio_write(0x3f);	// Display ON
	_delay_ms(10);

	decoder_select(DECODE_LCD_CS2);
	lcdio_write(0x3e);	// Display OFF
	_delay_ms(1);
	lcdio_write(0xc0);	// Display start line 0
	_delay_ms(1);
	lcdio_write(0x3f);	// Display ON
	_delay_ms(10);

	decoder_disable();
}


//-----------------------------------------------------------------//
/*!
	@breif	フレームバッファを転送
	@param[in]	p	フレームバッファのポインター(1024 バイト）
*/
//-----------------------------------------------------------------//
void lcdio_copy(const unsigned char *p)
{
	unsigned char i, j;

	INIT_PORT_DIRECTION;

	for(j = 0; j < 8; ++j) {
		LCD_RS_LO;
		decoder_select(DECODE_LCD_CS1);
		_delay_loop_1(SETUP_TIME);
		lcdio_write(0xb8 + j);		// set address X
		lcdio_write(0x40);			// set address Y

		LCD_RS_HI;
		for(i = 0; i < 64; ++i) {
			lcdio_write( *p );
			p++;
		}

		LCD_RS_LO;
		decoder_select(DECODE_LCD_CS2);
		_delay_loop_1(SETUP_TIME);
		lcdio_write(0xb8 + j);		// set address X
		lcdio_write(0x40);			// set address Y

		LCD_RS_HI;
		for(i = 0; i < 64; ++i) {
			lcdio_write( *p );
			p++;
		}
	}

	decoder_disable();
}

/* ----- End Of File "lcdio.c" ----- */
