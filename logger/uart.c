//=====================================================================//
/*!	@file
	@breif	AVR シリアル入出力の為のクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "uart.h"
#include "xitoa.h"

// F_CPU を必ず設定の事

// FIFO バッファのサイズ
#define FIFO_BUFF_SIZE	250

// UART のチャネル数
#define UART_NUM	2

typedef struct _fifo {
	uint8_t	idx_w;
	uint8_t	idx_r;
	uint8_t	count;
	uint8_t	buff[FIFO_BUFF_SIZE];
} FIFO;

static volatile FIFO g_txfifo[UART_NUM], g_rxfifo[UART_NUM];

//-----------------------------------------------------------------//
/*!
	@breif	UART の初期化@n
			初期化では、リセットの初期化に依存するので、それを変更@n
			した場合は、適切な値を設定する事。@n
			※通常、８ビット、１ストップビット、パリティ無し
	@param[in]	chn		UART チャネル
	@param[in]	baud	ボーレート
*/
//-----------------------------------------------------------------//
void uart_init(uint8_t chn, unsigned long baud)
{
	uint16_t val;

	if(chn == 0) {			// Chanel-0 UART
		PORTD &= 0b11111110;
		PORTD |= 0b00000010;
		DDRD  |= 0b00000010;
	} else if(chn == 1) {	// Chanel-1 UART
		PORTD &= 0b11111011;
		PORTD |= 0b00001000;
		DDRD  |= 0b00001000;
	}

	g_rxfifo[chn].idx_r = 0;
	g_rxfifo[chn].idx_w = 0;
	g_rxfifo[chn].count = 0;

	g_txfifo[chn].idx_r = 0;
	g_txfifo[chn].idx_w = 0;
	g_txfifo[chn].count = 0;

	// 四捨五入を行う為、DIVIDER は 1/2 で計算する
	val = (uint32_t)F_CPU / ((uint32_t)baud * (uint32_t)4);
	if(val & 1) {
		// 0.5 以上の場合、(-1)をしない。
		val >>= 1;
	} else {
		// 0.5 以下の場合、通常の(-1)
		val >>= 1;
		val--;
	}

	if(chn == 0) {
		if(val >= 4096) {
			val >>= 1;
		} else {
			UCSR0A |= 1 << (U2X0);	// double speed
		}

		--val;
		UBRR0H = (val >> 8) & 0x0f;
		UBRR0L = val & 0xff;

		UCSR0B = _BV(RXEN0) | _BV(RXCIE0) | _BV(TXEN0);
	} else if(chn == 1) {
		if(val >= 4096) {
			val >>= 1;
		} else {
			UCSR1A |= 1 << (U2X1);	// double speed
		}

		--val;
		UBRR1H = (val >> 8) & 0x0f;
		UBRR1L = val & 0xff;

		UCSR1B = _BV(RXEN1) | _BV(RXCIE1) | _BV(TXEN1);
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	レシーブステータスを得る
	@param[in]	chn		UART チャネル
	@return ステータス情報
*/
//-----------------------------------------------------------------//
enum UART_RECV_STATUS uart_get_status(uint8_t chn)
{
	if(chn == 0) {
		return UCSR0A;
	} else if(chn == 1) {
		return UCSR1A;
	}
	return 0;
}


//-----------------------------------------------------------------//
/*!
	@breif	現在のレシーブデータサイズを得る
	@param[in]	chn	UART チャネル
	@return レシーブバッファに溜まっているデータ長を返す
*/
//-----------------------------------------------------------------//
uint8_t uart_recv_length(uint8_t chn)
{
	return g_rxfifo[chn].count;
}


//-----------------------------------------------------------------//
/*!
	@breif	データを受信する
	@param[in]	chn	UART チャネル
	@return 取り出したレシーブデータ
*/
//-----------------------------------------------------------------//
uint8_t uart_recv(uint8_t chn)
{
	uint8_t d, i;

	i = g_rxfifo[chn].idx_r;
	while(g_rxfifo[chn].count == 0) ;
	d = g_rxfifo[chn].buff[i++];
	cli();
	g_rxfifo[chn].count--;
	sei();
	if(i >= sizeof(g_rxfifo[chn].buff)) {
		i = 0;
	}
	g_rxfifo[chn].idx_r = i;

	return d;
}


//-----------------------------------------------------------------//
/*!
	@breif	データを送信する
	@param[in]	chn	UART チャネル
	@param[in]	ch	送信するデータ
*/
//-----------------------------------------------------------------//
void uart_send(uint8_t chn, uint8_t ch)
{
	uint8_t i = g_txfifo[chn].idx_w;
	while(g_txfifo[chn].count >= sizeof(g_txfifo[chn].buff)) ;
	g_txfifo[chn].buff[i] = ch;
	++i;
	cli();
	g_txfifo[chn].count++;
	if(chn == 0) UCSR0B = _BV(RXEN0) | _BV(RXCIE0) | _BV(TXEN0) | _BV(UDRIE0);
	else if(chn == 1) UCSR1B = _BV(RXEN1) | _BV(RXCIE1) | _BV(TXEN1) | _BV(UDRIE1);
	sei();
	if(i >= sizeof(g_txfifo[chn].buff)) {
		i = 0;
	}
	g_txfifo[chn].idx_w = i;
}


//-----------------------------------------------------------------//
/*!
	@breif	文字列の送信（プログラムエリア専用）
	@param[in]	chn	UART チャネル
	@param[in]	text	送信する文字列
*/
//-----------------------------------------------------------------//
void uart_send_text_p(uint8_t chn, const char* text)
{
	char ch;

	while((ch = pgm_read_byte_near(text)) != 0) {
		uart_send(chn, (uint8_t)ch);
		text++;
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	文字列の送信
	@param[in]	chn		UART チャネル
	@param[in]	text	送信する文字列
*/
//-----------------------------------------------------------------//
void uart_send_text(uint8_t chn, const char* text)
{
	char ch;

	while((ch = *text) != 0) {
		uart_send(chn, (uint8_t)ch);
		text++;
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	CR/LF の送信
	@param[in]	chn	UART チャネル
*/
//-----------------------------------------------------------------//
void uart_send_crlf(uint8_t chn)
{
	uart_send(chn, '\n');
	uart_send(chn, '\r');
}


#if 0
//-----------------------------------------------------------------//
/*!
	@breif	HexDecimal の送信
	@param[in]	value	値
	@param[in]	num		表示桁数
*/
//-----------------------------------------------------------------//
void uart_send_hexadecimal(unsigned long value, int num)
{
	uint8_t chs[8];
	uint8_t ch;
	uint8_t i;

	for(i = 0; i < num; ++i) {
		ch = value & 0xf;
		if(ch < 10) ch += '0';
		else ch += 'A' - 10;
		chs[i] = ch;
		value >>= 4;
	}
	while(i > 0) {
		--i;
		uart_send(chs[i]);
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	HexDecimal 文字列から数値への変換
	@param[in]	text	16進の文字列
	@return 値
*/
//-----------------------------------------------------------------//
unsigned long hexadecimal_to_value(const char *text)
{
	unsigned long val;
	char ch;

	val = 0;
	while((ch = *text++) != 0) {
		val <<= 4;
		if(ch >= '0' && ch <= '9') {
			val |= ch - '0';
		} else if(ch >= 'A' && ch <= 'F') {
			val |= ch - 'A' + 10;
		} else if(ch >= 'a' && ch <= 'f') {
			val |= ch - 'a' + 10;
		} else {
			break;
		}
	}
	return val;
}
#endif


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@breif	UART0 RX interrupt
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
ISR(USART0_RX_vect)
{
	uint8_t d, n, i;

	d = UDR0;
	n = g_rxfifo[0].count;
	if(n < sizeof(g_rxfifo[0].buff)) {
		g_rxfifo[0].count = ++n;
		i = g_rxfifo[0].idx_w;
		g_rxfifo[0].buff[i++] = d;
		if(i >= sizeof(g_rxfifo[0].buff)) {
			i = 0;
		}
		g_rxfifo[0].idx_w = i;
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@breif	UART0 UDRE interrupt
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
ISR(USART0_UDRE_vect)
{
	uint8_t n = g_txfifo[0].count;
	if(n) {
		--n;
		g_txfifo[0].count = n;
		uint8_t i = g_txfifo[0].idx_r;
		UDR0 = g_txfifo[0].buff[i];
		++i;
		if(i >= sizeof(g_txfifo[0].buff)) {
			i = 0;
		}
		g_txfifo[0].idx_r = i;
	}
	if(n == 0) {
		UCSR0B = _BV(RXEN0) | _BV(RXCIE0) | _BV(TXEN0);
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@breif	UART1 RX interrupt
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
ISR(USART1_RX_vect)
{
	uint8_t d, n, i;

	d = UDR1;
	n = g_rxfifo[1].count;
	if(n < sizeof(g_rxfifo[1].buff)) {
		g_rxfifo[1].count = ++n;
		i = g_rxfifo[1].idx_w;
		g_rxfifo[1].buff[i++] = d;
		if(i >= sizeof(g_rxfifo[1].buff)) {
			i = 0;
		}
		g_rxfifo[1].idx_w = i;
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@breif	UART1 UDRE interrupt
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
ISR(USART1_UDRE_vect)
{
	uint8_t n = g_txfifo[1].count;
	if(n) {
		--n;
		g_txfifo[1].count = n;
		uint8_t i = g_txfifo[1].idx_r;
		UDR1 = g_txfifo[1].buff[i];
		++i;
		if(i >= sizeof(g_txfifo[1].buff)) {
			i = 0;
		}
		g_txfifo[1].idx_r = i;
	}
	if(n == 0) {
		UCSR1B = _BV(RXEN1) | _BV(RXCIE1) | _BV(TXEN1);
	}
}


/* ----- End Of File "uart.c" ----- */
