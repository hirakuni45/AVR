//=====================================================================//
/*!	@file
	@breif	液晶（LCD）ハードウェアーの制御@n
			KS108B LCD モジュール（128×64）@n
			※コントローラーが同等品なので、同じように扱える@n
			・サイキック販売版@n
			http://www.xikit.com/store/index.php?main_page=index&cPath=11@n
			・ストロベリーリナックス版(GDM12864H)@n
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
#include "lcdio.hpp"

namespace device {

	// ATMEGA328 を使う場合で、RXD,TXD を使う場合、データポートを全て利用できない
	// その為、効率が悪いが、データポートを二分割で利用する必要がある。
	static void INIT_PORT_DIRECTION_(void)
	{
		DDRB |= 0x30;
		DDRC |= 0x3f;
		DDRD |= 0xf0;
	}

	static inline void LCD_DATA_(unsigned char x)
	{
		PORTC = (PORTC & 0xf0) | (x & 0x0f);
		PORTD = (PORTD & 0x0f) | (x & 0xf0);
	}

	static inline void LCD_CS1_ENA_(void) { PORTB |=  0x10; }
	static inline void LCD_CS2_ENA_(void) { PORTB |=  0x20; }
	static inline void LCD_CS1_DIS_(void) { PORTB &= ~0x10; }
	static inline void LCD_CS2_DIS_(void) { PORTB &= ~0x20; }
	static inline void LCD_RS_LO_(void) { PORTC &= ~0x10; }
	static inline void LCD_RS_HI_(void) { PORTC |=  0x10; }
	static inline void LCD_E_LO_(void) { PORTC &= ~0x20; }
	static inline void LCD_E_HI_(void) { PORTC |=  0x20; }

	// at 19.6MHz
	static const int SETUP_TIME = 5;
	static const int HOLD_TIME = 22;

	// at 12MHz
	// static const int SETUP_TIME = 4;
	// static const int HOLD_TIME = 18;

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
	static void write_(unsigned char cmd)
	{
		LCD_DATA_(cmd);
		_delay_loop_1(SETUP_TIME);
		LCD_E_HI_();
		_delay_loop_1(HOLD_TIME);
		LCD_E_LO_();
		_delay_loop_1(HOLD_TIME);
	}


	//-----------------------------------------------------------------//
	/*!
		@breif	LCD ハードウェアー関係の初期化など
	*/
	//-----------------------------------------------------------------//
	void lcdio::init()
	{
		INIT_PORT_DIRECTION_();

		LCD_RS_LO_();
		LCD_E_LO_();
		LCD_CS1_DIS_();
		LCD_CS2_DIS_();
//		decoder_select(DECODE_LCD_RES);	// LCD reset assert
		_delay_ms(1);					// 1ms reset signal
//		decoder_disable();				// LCD reset negate
		_delay_ms(100);					// 100ms reset cycle

		LCD_RS_LO_();
		LCD_CS1_ENA_();
		write_(0x3e);	// Display OFF
		_delay_ms(1);
		write_(0xc0);	// Display start line 0
		_delay_ms(1);
		write_(0x3f);	// Display ON
		_delay_ms(10);
		LCD_CS1_DIS_();

		LCD_CS2_ENA_();
		write_(0x3e);	// Display OFF
		_delay_ms(1);
		write_(0xc0);	// Display start line 0
		_delay_ms(1);
		write_(0x3f);	// Display ON
		_delay_ms(10);
		LCD_CS2_DIS_();
	}


	//-----------------------------------------------------------------//
	/*!
		@breif	フレームバッファを転送
		@param[in]	p	フレームバッファのポインター(1024 バイト）
	*/
	//-----------------------------------------------------------------//
	void lcdio::copy(const uint8_t *p)
	{
		for(uint8_t j = 0; j < 8; ++j) {
			LCD_RS_LO_();
			LCD_CS1_ENA_();
			_delay_loop_1(SETUP_TIME);
			write_(0xb8 + j);		// set address X
			write_(0x40);			// set address Y

			LCD_RS_HI_();
			for(uint8_t i = 0; i < 64; ++i) {
				write_( *p );
				p++;
			}
			LCD_CS1_DIS_();

			LCD_RS_LO_();
			LCD_CS2_ENA_();
			_delay_loop_1(SETUP_TIME);
			write_(0xb8 + j);		// set address X
			write_(0x40);			// set address Y

			LCD_RS_HI_();
			for(uint8_t i = 0; i < 64; ++i) {
				write_( *p );
				p++;
			}
			LCD_CS2_DIS_();
		}
	}
}
