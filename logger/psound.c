//=====================================================================//
/*!	@file
	@breif	パルス・サウンド関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "psound.h"

// 1.059463094 : 2 ^ (1 / 12)

// octave-0
// ( 0) C    : 32.70319566257483


// octave-3 (center)
// ( 0) C    : 261.63 Hz		9393
// ( 1) C#/Db: 277.18 Hz		8866
// ( 2) D    : 293.66 Hz		8369
// ( 3) D#/Eb: 311.13 Hz		7899
// ( 4) E    : 329.63 Hz		7456
// ( 5) F    : 349.23 Hz		7037
// ( 6) F#/Gb: 369.99 Hz		6642
// ( 7) G    : 392.00 Hz		6269
// ( 8) G#/Ab: 415.30 Hz		5918
// ( 9) A    : 440.00 Hz		5585
// (10) A#/Bb: 466.16 Hz		5272
// (11) B    : 493.88 Hz		4976

static const prog_uint16_t g_sound12_para[] = {
	9393, 8866, 8369, 7899, 7456, 7037, 6642, 6269, 5918, 5585, 5272, 4976
};

static unsigned char g_ps_index;
static unsigned char g_ps_length;
static unsigned char g_ps_count;
static char g_ps_enable;
static const prog_uint8_t *g_music_player;

void psound_disable(void)
{
	TCCR1B = 0b00000000;
	TCCR1A = 0b00000000;
	TCCR1C = 0b00000000;
	g_ps_enable = 0;
}

static void psound_enable(unsigned char divide)
{
	TCCR1A = 0b01000000;
	TCCR1B = 0b00001000 | (divide & 7);
	TCCR1C = 0b10000000;	// FOC1A
}

//-----------------------------------------------------------------//
/*!
	@breif	パルスサウンド初期化
*/
//-----------------------------------------------------------------//
void psound_init(void)
{
	DDRD |= _BV(5);

/* 16 ビットタイマー設定 */
#if 0
	TCNT1  = 0;

	TCCR1A = 0b00000000;	// OC1A toggle (Compare Match)
	TCCR1B = 0b00000000;	// 一致クリア(CTC)、プリスケーラー(1/8)
							// 000: No clock source(Timer/Counter stopped)
							// 001: 1/1
							// 010: 1/8
							// 011: 1/64
							// 100: 1/256
							// 101: 1/1024
#endif
	psound_disable();
	OCR1A = 0;

	g_ps_length = 0;
	g_ps_count = 0;

	g_music_player = NULL;
}


static void set_frq(unsigned char index)
{
	unsigned short v;
	unsigned short oct;
	v = pgm_read_word_near(&g_sound12_para[index % 12]);
	oct = index / 12;
	OCR1A = (v >> oct) - 1;
	TCNT1  = 0;
}


//-----------------------------------------------------------------//
/*!
	@breif	パルスサウンドサービス(1/100 で呼ぶ）
*/
//-----------------------------------------------------------------//
void psound_service()
{
	if(g_ps_enable == 0) return;

	if(g_ps_count < g_ps_length) {
		if(g_ps_count == 0) {
			psound_enable(0b010);	// 1/8 divider
			set_frq(g_ps_index);
		}
		++g_ps_count;
	} else {
		if(g_music_player != NULL) {
			const prog_uint8_t *p = g_music_player;
			unsigned char idx = pgm_read_byte_near(p);
			if(idx) {
				++p;
				unsigned char len = pgm_read_byte_near(p);
				++p;
				psound_request(idx - 1, len);
				g_music_player = p;
				return;
			} else {
				g_music_player = NULL;
			}
		}
		psound_disable();
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	パルスサウンドリクエスト
	@param[in]	index	音階
	@param[in]	length	音長
*/
//-----------------------------------------------------------------//
void psound_request(unsigned char index, unsigned char length)
{
	g_ps_enable = 1;
	g_ps_index = index;
	g_ps_length = length;
	g_ps_count = 0;
}


//-----------------------------------------------------------------//
/*!
	@breif	パルスサウンド演奏
	@param[in]	music	音階と音長の組をテーブルにしたもの
*/
//-----------------------------------------------------------------//
void psound_play_P(const prog_uint8_t *music)
{
	if(music == NULL) return;

	unsigned char idx = *music++;
	unsigned char len = *music++;
	psound_request(idx - 1, len);
	g_music_player = music;
}


//-----------------------------------------------------------------//
/*!
	@breif	コンペアマッチに直接値を設定する。
	@param[in]	divider	ディバイダーの設定値
	@param[in]	cmpv	コンペアマッチに設定する値
*/
//-----------------------------------------------------------------//
void psound_direct(unsigned char divide, unsigned short cmpv)
{
	g_ps_enable = 0;
	psound_enable(divide);
	OCR1A = cmpv;
	TCNT1 = 0;
}


//-----------------------------------------------------------------//
/*!
	@breif	トグル出力の周波数を設定する
	@param[in]	freq	出力周波数(Hz)下位４ビット小数点
	@return 「０」なら正常終了
*/
//-----------------------------------------------------------------//
char psound_freq(unsigned long freq)
{
	unsigned long a, b, d;
	unsigned char divi;

	if(freq == 0) {
		psound_disable();
		return -1;
	}

	b = (F_CPU << 4) / freq;
	divi = 0;
	do {
		++divi;
		if(divi == 1) d = 1;
		else if(divi == 2) d = 8;
		else if(divi == 3) d = 64;
		else if(divi == 4) d = 256;
		else if(divi == 5) d = 1024;
		else {
			psound_disable();
			return 1;	// Over flow...
		}
		a = b / d;
	} while(a > 65536) ;

	psound_direct(divi, a - 1);

	return 0;
}


//-----------------------------------------------------------------//
/*!
	@breif	プリスケーラーの分周比を取得
	@return 分周比
*/
//-----------------------------------------------------------------//
int get_psound_divider(void)
{
	unsigned char a;

	a = TCCR1B & 0x7;
	if(a == 0) return 0;
	else if(a == 1) return 1;
	else if(a == 2) return 8;
	else if(a == 3) return 64;
	else if(a == 4) return 256;
	else if(a == 5) return 1024;

	return -1;
}


/* ----- End Of File "psound.c" ----- */


