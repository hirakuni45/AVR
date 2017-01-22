#ifndef SHARE_PORT_H
#define SHARE_PORT_H
//=====================================================================//
/*!	@file
	@breif	シェア・ポート制御（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>


//	PA0(40) : ---> 74AC138 A
//	PA1(39) : ---> 74AC138 B
//	PA2(38) : ---> 74AC138 C
//  PA3(37) : ---> LCD D/I, RTC4543 WR,  MCP3208 Din
//  PA4(36) : ---> LCD E,   RTC4543 CLK, MCP3208 CLK

#define DECODE_LCD_CS1	0
#define DECODE_LCD_CS2	1
#define DECODE_LCD_RES	2
#define DECODE_MCP3208	3
#define DECODE_RTC4543	4

#ifdef __cplusplus
extern "C" {
#endif

	static inline void share_port_init(void) { DDRA |= 0b00011111; }
	static inline void decoder_select(uint8_t no) { PORTA |= 7; PORTA &= 0xf8 | no; }
	static inline void decoder_disable(void) { PORTA |= 7; }

	static inline void share_pa3_lo(void) { PORTA &= ~_BV(3); }
	static inline void share_pa3_hi(void) { PORTA |=  _BV(3); }

	static inline void share_pa4_lo(void) { PORTA &= ~_BV(4); }
	static inline void share_pa4_hi(void) { PORTA |=  _BV(4); }

	static inline void share_pb1_out(void) { DDRB |=  _BV(1); }
	static inline void share_pb1_inp(void) { DDRB &= ~_BV(1); }
	static inline void share_pb1_lo(void) { PORTB &= ~_BV(1); }
	static inline void share_pb1_hi(void) { PORTB |=  _BV(1); }
	static inline uint8_t share_pb1_get(void) { return PINB & _BV(1); }

#ifdef __cplusplus
};
#endif

#endif
