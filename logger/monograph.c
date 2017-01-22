//=====================================================================//
/*!	@file
	@breif	モノクロ・ビットマップ・グラフィックスの為のライブラリー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "monograph.h"
#include "config.h"

// 漢字フォントを表示させる場合有効にする
#define KANJI_FONTS

#ifdef KANJI_FONTS
#include "ff.h"
#endif

// フレームバッファの大きさ
#define FB_MAX_X	128
#define FB_MAX_Y	64

static unsigned char g_fb_pad[FB_MAX_X * FB_MAX_Y / 8];
static char g_inverse;

// 標準的 ASCII フォント
static const prog_char font6x12[] = {
#include "font6x12.h"
};

// プロポーショナル・フォントのテーブル
// I: -1, i: -2, l: -2, r: -1
static const prog_char font6x12_width[] = {
	4, 4, 5, 6, 6, 6, 6, 4, 5, 5, 6, 6, 4, 5, 4, 6,
	5, 4, 5, 5, 6, 5, 5, 5, 5, 5, 4, 4, 6, 5, 6, 5,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 5, 5, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 6,
	4, 6, 6, 6, 6, 6, 5, 6, 6, 3, 4, 6, 3, 6, 6, 6,
	6, 6, 5, 6, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 6, 6
};

#ifdef KANJI_FONTS

// KANJI フォントをキャッシュする数
#define KANJI_CASH_SIZE	16

typedef struct {
	unsigned char sjis_hi;
	unsigned char sjis_lo;
	unsigned char bitmap[18];
} KANJI_CASH;

static KANJI_CASH g_kanji_cash[KANJI_CASH_SIZE];
static unsigned char g_cash_first;

// sjis コードをリニア表に変換する。
// 上位バイト： 0x81 to 0x9f, 0xe0 to 0xef
// 下位バイト： 0x40 to 0x7e, 0x80 to 0xfc
static unsigned short sjis_to_liner(unsigned char up, unsigned char lo)
{
	unsigned short code;
	if(0x81 <= up && up <= 0x9f) {
		code = up - 0x81;
	} else if(0xe0 <= up && up <= 0xef) {
		code = (0x9f + 1 - 0x81) + up - 0xe0;
	} else {
		return 6;	// 無効コードの代表値
	}
	int loa = (0x7e + 1 - 0x40) + (0xfc + 1 - 0x80);
	if(0x40 <= lo && lo <= 0x7e) {
		code *= loa;
		code += lo - 0x40;
	} else if(0x80 <= lo && lo <= 0xfc) {
		code *= loa;
		code += 0x7e + 1 - 0x40;
		code += lo - 0x80;
	} else {
		return 6;	// 無効コードの代表値
	}
	return code;
}

const unsigned char *scan_kanji_bitmap(unsigned char sjis_hi, unsigned char sjis_lo)
{
	unsigned char i;
	for(i = 0; i < KANJI_CASH_SIZE; ++i) {
		KANJI_CASH *ca = &g_kanji_cash[i];
		if(ca->sjis_hi == 0 && ca->sjis_lo == 0) {
			g_cash_first = i;
		} else if(ca->sjis_hi == sjis_hi && ca->sjis_lo == sjis_lo) {
			return ca->bitmap;
		}
	}
	return NULL;
}


unsigned char *alloc_kanji_bitmap(unsigned char sjis_hi, unsigned char sjis_lo)
{
	unsigned char *p = g_kanji_cash[g_cash_first].bitmap;
	g_kanji_cash[g_cash_first].sjis_hi = sjis_hi;
	g_kanji_cash[g_cash_first].sjis_lo = sjis_lo;
	++g_cash_first;
	if(g_cash_first >= KANJI_CASH_SIZE) g_cash_first = 0;
	return p;
}


char read_kanji_bitmap(unsigned short pos, unsigned char *bitmap)
{
	FIL fl;
	FRESULT res = f_open(&fl, "/w_kj12.bin", FA_OPEN_EXISTING | FA_READ);
	if(res == 0) {
		f_lseek(&fl, (long)pos * 18);
		UINT rl;
		f_read(&fl, bitmap, 18, &rl);
		f_close(&fl);
		return 1;
	} else {
		return 0;
	}
}
#endif

//-----------------------------------------------------------------//
/*!
	@breif	初期化
	@return	フレームバッファのポインター
*/
//-----------------------------------------------------------------//
unsigned char *monograph_init(void)
{
#ifdef KANJI_FONTS
	int i;
	for(i = 0; i < KANJI_CASH_SIZE; ++i) {
		g_kanji_cash[i].sjis_lo = 0;
		g_kanji_cash[i].sjis_hi = 0;
	}
	g_cash_first = 0;
#endif

	g_inverse = 0;

	return g_fb_pad;
}


//-----------------------------------------------------------------//
/*!
	@breif	点を描画する
	@param[in]	x	開始点Ｘ軸を指定
	@param[in]	y	開始点Ｙ軸を指定
*/
//-----------------------------------------------------------------//
inline void monograph_point_set(short x, short y)
{
	if((unsigned short)x >= FB_MAX_X) return;
	if((unsigned short)y >= FB_MAX_Y) return;

	g_fb_pad[((y & 0xf8) << 4) + x] |= (1 << (y & 7));
}


//-----------------------------------------------------------------//
/*!
	@breif	点を消去する
	@param[in]	x	開始点Ｘ軸を指定
	@param[in]	y	開始点Ｙ軸を指定
*/
//-----------------------------------------------------------------//
inline void monograph_point_reset(short x, short y)
{
	if((unsigned short)x >= FB_MAX_X) return;
	if((unsigned short)y >= FB_MAX_Y) return;

	g_fb_pad[((y & 0xf8) << 4) + x] &= ~(1 << (y & 7));
}


//-----------------------------------------------------------------//
/*!
	@breif	四角を塗りつぶす
	@param[in]	x	開始位置 X
	@param[in]	y	開始位置 Y
	@param[in]	w	横幅 
	@param[in]	h	高さ
	@param[in]	c	カラー
*/
//-----------------------------------------------------------------//
void monograph_fill(short x, short y, short w, short h, char c)
{
	short i, j;

	if(c) {
		for(i=y; i<(y + h); ++i) {
			for(j=x; j<(x + w); ++j) {
				monograph_point_set(j, i);
			}
		}
	} else {
		for(i=y; i<(y + h); ++i) {
			for(j=x; j<(x + w); ++j) {
				monograph_point_reset(j, i);
			}
		}
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	全画面クリアをする
	@param[in]	c	クリアカラー
*/
//-----------------------------------------------------------------//
void monograph_clear(char c)
{
	monograph_fill(0, 0, FB_MAX_X, FB_MAX_Y, c);
}


//-----------------------------------------------------------------//
/*!
	@breif	線を描画する
	@param[in]	x1	開始点Ｘ軸を指定
	@param[in]	y1	開始点Ｙ軸を指定
	@param[in]	x2	終了点Ｘ軸を指定
	@param[in]	y2	終了点Ｙ軸を指定
	@param[in]	c	描画色
*/
//-----------------------------------------------------------------//
void monograph_line(short x1, short y1, short x2, short y2, char c)
{
	short	dx, sx;
	short	dy, sy;
	short	m;
	short	x, y;
	short	i;

	if(x2 >= x1) { dx = x2 - x1; sx = 1; } else { dx = x1 - x2; sx = -1; }
	if(y2 >= y1) { dy = y2 - y1; sy = 1; } else { dy = y1 - y2; sy = -1; }

	m = 0;
	x = x1;
	y = y1;
	if(dx > dy) {
		for(i = 0; i <= dx; i++) {
			m += dy;
			if(m >= dx) {
				m -= dx;
				y += sy;
			}
			if(c) monograph_point_set(x, y);
			else monograph_point_reset(x, y);
			x += sx;
		}
	} else {
		int	x = x1;
		for(i = 0; i <= dy; i++) {
			m += dx;
			if(m >= dy) {
				m -= dy;
				x += sx;
			}
			if(c) monograph_point_set(x, y);
			else monograph_point_reset(x, y);
			y += sy;
		}
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	フレームを描画する
	@param[in]	x	開始点Ｘ軸を指定
	@param[in]	y	開始点Ｙ軸を指定
	@param[in]	w	横幅
	@param[in]	h	高さ
	@param[in]	c	描画色
*/
//-----------------------------------------------------------------//
void monograph_frame(short x, short y, short w, short h, char c)
{
	short i;
	for(i = 0; i < w; ++i) {
		if(c) {
			monograph_point_set(x + i, y);
			monograph_point_set(x + i, y + h - 1);
		} else {
			monograph_point_reset(x + i, y);
			monograph_point_reset(x + i, y + h - 1);
		}
	}
	for(i = 0; i < h; ++i) {
		if(c) {
			monograph_point_set(x, y + i);
			monograph_point_set(x + w - 1, y + i);
		} else {
			monograph_point_reset(x, y + i);
			monograph_point_reset(x + w - 1, y + i);
		}
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	ビットマップイメージを描画する
	@param[in]	x	開始点Ｘ軸を指定
	@param[in]	y	開始点Ｙ軸を指定
	@param[in]	img	描画ソースのポインター
	@param[in]	w	描画ソースの幅
	@param[in]	h	描画ソースの高さ
*/
//-----------------------------------------------------------------//
void monograph_draw_image_P(short x, short y, const prog_char *img, unsigned char w, unsigned char h)
{
	unsigned char i, j, k, c;

	k = 1;
	c = pgm_read_byte_near(img);
	img++;

	for(i=0; i<h; i++) {
		short xx = x;
		for(j=0; j<w; j++) {
			if(c & k) monograph_point_set(xx, y);
			k <<= 1;
			if(k == 0) {
				k = 1;
				c = pgm_read_byte_near(img);
				img++;
			}
			xx++;
		}
		y++;
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	ビットマップイメージを描画する
	@param[in]	x	開始点Ｘ軸を指定
	@param[in]	y	開始点Ｙ軸を指定
	@param[in]	img	描画ソースのポインター
	@param[in]	w	描画ソースの幅
	@param[in]	h	描画ソースの高さ
*/
//-----------------------------------------------------------------//
void monograph_draw_image(short x, short y, const unsigned char *img, unsigned char w, unsigned char h)
{
	unsigned char i, j, k, c;

	k = 1;
	c = *img++;

	for(i=0; i<h; i++) {
		short xx = x;
		for(j=0; j<w; j++) {
			if(c & k) monograph_point_set(xx, y);
			k <<= 1;
			if(k == 0) {
				k = 1;
				c = *img++;
			}
			xx++;
		}
		y++;
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	モーションオブジェクトを描画する
	@param[in]	x	開始点Ｘ軸を指定
	@param[in]	y	開始点Ｙ軸を指定
	@param[in]	img	描画ソースのポインター
*/
//-----------------------------------------------------------------//
void monograph_draw_mobj_P(short x, short y, const prog_char *img)
{
	char w = pgm_read_byte_near(img);
	img++;
	char h = pgm_read_byte_near(img);
	img++;
	monograph_draw_image_P(x, y, img, w, h);
}

static unsigned char g_multi_byte_hi;

//-----------------------------------------------------------------//
/*!
	@breif	ascii フォントを描画する。
	@param[in]	x	開始点Ｘ軸を指定
	@param[in]	y	開始点Ｙ軸を指定
	@param[in]	code	ASCII コード（0x00 to 0x7F)
*/
//-----------------------------------------------------------------//
void monograph_draw_font(short x, short y, char code)
{
	if(g_multi_byte_hi) {
		unsigned char hi = g_multi_byte_hi;
		g_multi_byte_hi = 0;
		if(-12 >= x || x >= FB_MAX_X) return;

		if(g_kanji_mode == 0) {
			monograph_draw_image_P(x - 6, y, &font6x12[9 * 18], 6, 12);
			monograph_draw_image_P(x,     y, &font6x12[9 * 19], 6, 12);
			return;
		}

#ifdef KANJI_FONTS
		const unsigned char* bitmap = scan_kanji_bitmap(hi, (unsigned char)code);
		if(bitmap != NULL) {
			monograph_draw_image(x - 6, y, bitmap, 12, 12);
		} else {
			unsigned char *bitmap = alloc_kanji_bitmap(hi, (unsigned char)code);
			unsigned short pos = sjis_to_liner(hi, (unsigned char)code);
			if(read_kanji_bitmap(pos, bitmap)) {
				monograph_draw_image(x - 6, y, bitmap, 12, 12);
			} else {
				monograph_draw_image_P(x - 6, y, &font6x12[9 * 18], 6, 12);
				monograph_draw_image_P(x,     y, &font6x12[9 * 19], 6, 12);
			}
		}
#endif
	} else {
		if(code >= 0) {
		if(-12 >= x || x >= FB_MAX_X) return;
			monograph_draw_image_P(x, y, &font6x12[(code << 3) + code], 6, 12);
		} else if((unsigned char)code >= 0x81 && (unsigned char)code <= 0x9f) {
			g_multi_byte_hi = code;
		} else if((unsigned char)code >= 0xe0 && (unsigned char)code <= 0xef) {
			g_multi_byte_hi = code;
		} else {
			// 無効キャラクターの意味として
			g_multi_byte_hi = 0;
			if(-12 >= x || x >= FB_MAX_X) return;
			monograph_draw_image_P(x, y, &font6x12[(1 << 3)], 6, 12);
		}
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	ascii テキストを描画する。
	@param[in]	x	開始点Ｘ軸を指定
	@param[in]	y	開始点Ｙ軸を指定
	@param[in]	text	テキストのポインター
	@return 文字の終端座標（Ｘ）
*/
//-----------------------------------------------------------------//
short monograph_draw_string(short x, short y, const char *text)
{
	char code;

	while((code = *text++) != 0) {
		monograph_draw_font(x, y, code);
		x += 6;
	}
	g_multi_byte_hi = 0;
	return x;
}


//-----------------------------------------------------------------//
/*!
	@breif	ascii プログラム領域のテキストを描画する。
	@param[in]	x	開始点Ｘ軸を指定
	@param[in]	y	開始点Ｙ軸を指定
	@param[in]	text	テキストのポインター
	@return 文字の終端座標（Ｘ）
*/
//-----------------------------------------------------------------//
short monograph_draw_string_P(short x, short y, const char *text)
{
	char	code;

	while((code = pgm_read_byte_near(text)) != 0) {
		text++;
		monograph_draw_font(x, y, code);
		x += 6;
	}
	g_multi_byte_hi = 0;
	return x;
}


//-----------------------------------------------------------------//
/*!
	@breif	テキスト列を中心に描画する。
	@param[in]	text	テキストのポインター
*/
//-----------------------------------------------------------------//
void monograph_draw_string_center_P(const prog_char *text)
{
	int xx = 0;
	int yy = 12;
	char ch;
	int l = 0;
	const prog_char *p = text;
	while((ch = pgm_read_byte_near(p)) != 0) {
		++p;
		if(ch == '\n') {
			if(xx < l) xx = l;
			l = 0;
			yy += 12;
		} else {
			l += 6;
		}
	}
	if(xx < l) xx = l;
	--p;
	ch = pgm_read_byte_near(p);
	if(ch == '\n') yy -= 12;

	monograph_draw_string_P(64 - xx / 2, 32 - yy / 2, text);
}


//-----------------------------------------------------------------//
/*!
	@breif	日付表示 YYYY/MM/DD を行う
	@param[in]	x	開始点Ｘ軸を指定
	@param[in]	y	開始点Ｙ軸を指定
	@param[in]	tmp tm 構造体のポインター
*/
//-----------------------------------------------------------------//
void monograph_draw_date(short x, short y, const struct tm *tmp)
{
	char	text[16];

	sprintf_P(text, PSTR("%04d/%02d/%02d"),
		tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday);
 	monograph_draw_string(x, y, text);
}


//-----------------------------------------------------------------//
/*!
	@breif	時間表示 HH/MM/SS を行う
	@param[in]	x	開始点Ｘ軸を指定
	@param[in]	y	開始点Ｙ軸を指定
	@param[in]	tmp tm 構造体のポインター
*/
//-----------------------------------------------------------------//
void monograph_draw_time(short x, short y, const struct tm *tmp)
{
	char text[16];

	sprintf_P(text, PSTR("%02d:%02d.%02d"),
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
	monograph_draw_string(x, y, text);
}


/* ----- End Of File "monograph.c" ----- */
