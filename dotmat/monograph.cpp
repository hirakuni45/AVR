//=====================================================================//
/*!	@file
	@brief	モノクローム・グラフィックス・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/pgmspace.h>
#include "monograph.hpp"

#ifdef KANJI_FONTS
#include "ff.h"
#endif

// 標準的 ASCII フォント
#ifdef LCD128X64
static const uint8_t font6x12_[] PROGMEM = {
#include "font6x12.h"
};
#endif

#if 0
// プロポーショナル・フォントのテーブル
// I: -1, i: -2, l: -2, r: -1
static const char font6x12_width[] PROGMEM = {
	4, 4, 5, 6, 6, 6, 6, 4, 5, 5, 6, 6, 4, 5, 4, 6,
	5, 4, 5, 5, 6, 5, 5, 5, 5, 5, 4, 4, 6, 5, 6, 5,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 5, 5, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 6,
	4, 6, 6, 6, 6, 6, 5, 6, 6, 3, 4, 6, 3, 6, 6, 6,
	6, 6, 5, 6, 5, 6, 6, 6, 6, 6, 6, 5, 5, 5, 6, 6
};
#endif

namespace graphics {

#ifdef KANJI_FONTS
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
		return 0;
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
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void monograph::init()
	{
#ifdef KANJI_FONTS
		for(uint8_t i = 0; i < KANJI_CASH_SIZE; ++i) {
			kanji_cash_[i].sjis_lo = 0;
			kanji_cash_[i].sjis_hi = 0;
		}
		cash_first_ = 0;
#endif
#ifdef LCD128X64
		font_width_ = 6;
		font_height_ = 12;
#endif
		multi_byte_hi_ = 0;

	}



	//-----------------------------------------------------------------//
	/*!
		@brief	点を描画する
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
	*/
	//-----------------------------------------------------------------//
	void monograph::point_set(int16_t x, int16_t y)
	{
		if(static_cast<uint16_t>(x) >= fb_width_) return;
		if(static_cast<uint16_t>(y) >= fb_height_) return;
#ifdef LED16X16
		fb_[((x & 8) >> 3) + (y << 1)] |= (1 << (x & 7));
#endif
#ifdef LCD128X64
		fb_[((y & 0xf8) << 4) + x] |= (1 << (y & 7));
#endif
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	点を消去する
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
	*/
	//-----------------------------------------------------------------//
	void monograph::point_reset(int16_t x, int16_t y)
	{
		if(static_cast<uint16_t>(x) >= fb_width_) return;
		if(static_cast<uint16_t>(y) >= fb_height_) return;
#ifdef LED16X16
		fb_[((x & 8) >> 3) + (y << 1)] &= ~(1 << (x & 7));
#endif
#ifdef LCD128X64
		fb_[((y & 0xf8) << 4) + x] &= ~(1 << (y & 7));
#endif
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	四角を塗りつぶす
		@param[in]	x	開始位置 X
		@param[in]	y	開始位置 Y
		@param[in]	w	横幅 
		@param[in]	h	高さ
		@param[in]	c	カラー
	*/
	//-----------------------------------------------------------------//
	void monograph::fill(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t c)
	{
		if(c) {
			for(int16_t i = y; i < (y + h); ++i) {
				for(int16_t j = x; j < (x + w); ++j) {
					point_set(j, i);
				}
			}
		} else {
			for(int16_t i = y; i < (y + h); ++i) {
				for(int16_t j = x; j < (x + w); ++j) {
					point_reset(j, i);
				}
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	全画面クリアをする
		@param[in]	c	クリアカラー 0:クリア 0!=:塗る
	*/
	//-----------------------------------------------------------------//
	void monograph::clear(uint8_t c)
	{
		fill(0, 0, fb_width_, fb_height_, c);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	線を描画する
		@param[in]	x1	開始点Ｘ軸を指定
		@param[in]	y1	開始点Ｙ軸を指定
		@param[in]	x2	終了点Ｘ軸を指定
		@param[in]	y2	終了点Ｙ軸を指定
		@param[in]	c	描画色
	*/
	//-----------------------------------------------------------------//
	void monograph::line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t c)
	{
		int16_t dx;
		int16_t dy;
		int16_t sx;
		int16_t sy;
		if(x2 >= x1) { dx = x2 - x1; sx = 1; } else { dx = x1 - x2; sx = -1; }
		if(y2 >= y1) { dy = y2 - y1; sy = 1; } else { dy = y1 - y2; sy = -1; }

		int16_t m = 0;
		int16_t x = x1;
		int16_t y = y1;
		if(dx > dy) {
			for(int16_t i = 0; i <= dx; i++) {
				if(c) point_set(x, y);
				else point_reset(x, y);
				m += dy;
				if(m >= dx) {
					m -= dx;
					y += sy;
				}
				x += sx;
			}
		} else {
			for(int16_t i = 0; i <= dy; i++) {
				if(c) point_set(x, y);
				else point_reset(x, y);
				m += dx;
				if(m >= dy) {
					m -= dy;
					x += sx;
				}
				y += sy;
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	フレームを描画する
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	w	横幅
		@param[in]	h	高さ
		@param[in]	c	描画色
	*/
	//-----------------------------------------------------------------//
	void monograph::frame(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t c)
	{
		for(int16_t i = 0; i < w; ++i) {
			if(c) {
				point_set(x + i, y);
				point_set(x + i, y + h - 1);
			} else {
				point_reset(x + i, y);
				point_reset(x + i, y + h - 1);
			}
		}
		for(int16_t i = 0; i < h; ++i) {
			if(c) {
				point_set(x, y + i);
				point_set(x + w - 1, y + i);
			} else {
				point_reset(x, y + i);
				point_reset(x + w - 1, y + i);
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ビットマップイメージを描画する
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	img	描画ソースのポインター
		@param[in]	w	描画ソースの幅
		@param[in]	h	描画ソースの高さ
	*/
	//-----------------------------------------------------------------//
	void monograph::draw_image_P(int16_t x, int16_t y, const uint8_t* img, uint8_t w, uint8_t h)
	{
		uint8_t k = 1;
		uint8_t c = static_cast<uint8_t>(pgm_read_byte_near(img));
		++img;

		for(uint8_t i = 0; i < h; ++i) {
			int16_t xx = x;
			for(uint8_t j = 0; j < w; ++j) {
				if(c & k) point_set(xx, y);
				k <<= 1;
				if(k == 0) {
					k = 1;
					c = static_cast<uint8_t>(pgm_read_byte_near(img));
					++img;
				}
				++xx;
			}
			++y;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ビットマップイメージを描画する
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	img	描画ソースのポインター
		@param[in]	w	描画ソースの幅
		@param[in]	h	描画ソースの高さ
	*/
	//-----------------------------------------------------------------//
	void monograph::draw_image(int16_t x, int16_t y, const uint8_t* img, uint8_t w, uint8_t h)
	{
		uint8_t k = 1;
		uint8_t c = *img++;

		for(uint8_t i = 0; i < h; ++i) {
			int16_t xx = x;
			for(uint8_t j = 0; j < w; ++j) {
				if(c & k) point_set(xx, y);
				k <<= 1;
				if(k == 0) {
					k = 1;
					c = *img++;
				}
				++xx;
			}
			++y;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	モーションオブジェクトを描画する
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	img	描画ソースのポインター
	*/
	//-----------------------------------------------------------------//
	void monograph::draw_mobj_P(int16_t x, int16_t y, const uint8_t* img)
	{
		uint8_t w = static_cast<uint8_t>(pgm_read_byte_near(img));
		++img;
		uint8_t h = static_cast<uint8_t>(pgm_read_byte_near(img));
		++img;
		draw_image_P(x, y, img, w, h);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ascii フォントを描画する。
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	code	ASCII コード（0x00 to 0x7F)
	*/
	//-----------------------------------------------------------------//
	void monograph::draw_font(int16_t x, int16_t y, char code)
	{
#ifdef KANJI_FONTS
		if(multi_byte_hi_) {
			uint8_t hi = multi_byte_hi_;
			multi_byte_hi_ = 0;
			if(-KANJI_FONT_WIDTH >= x || x >= fb_width_) return;

			if(kanji_mode_ == 0) {
				draw_image_P(x - FONT_WIDTH, y, &font6x12_[9 * 18], 6, 12);
				draw_image_P(x, y, &font6x12_[9 * 19], 6, 12);
				return;
			}

			const uint8_t* bitmap = scan_kanji_bitmap(hi, static_cast<uint8_t>(code));
			if(bitmap != 0) {
				draw_image(x - FONT_WIDTH, y, bitmap, KANJI_FONT_WIDTH, KANJI_FONT_HWIGHT);
			} else {
				uint8_t* bitmap = alloc_kanji_bitmap(hi, static_cast<uint8_t>(code));
				uint16_t pos = sjis_to_liner(hi, static_cast<uint8_t>(code));
				if(read_kanji_bitmap(pos, bitmap)) {
					draw_image(x - FONT_WIDTH, y, bitmap, KANJI_FONT_WIDTH, KANJI_FONT_HEIGHT);
				} else {
					draw_image_P(x - FONT_WIDTH, y, &font6x12_[9 * 18], FONT_WIDTH, FONT_HEIGHT);
					draw_image_P(x, y, &font6x12_[9 * 19], FONT_WIDTH, FONT_HEIGHT);
				}
			}
		} else
#endif
		{
#ifdef LCD128X64
			if(code >= 0) {
				if(-FONT_WIDTH >= x || static_cast<uint16_t>(x) >= fb_width_) {
					return;
				}
				draw_image_P(x, y, &font6x12_[(code << 3) + code], FONT_WIDTH, FONT_HEIGHT);
			} else if(static_cast<uint8_t>(code) >= 0x81
				   && static_cast<uint8_t>(code) <= 0x9f) {
				multi_byte_hi_ = code;
			} else if(static_cast<uint8_t>(code) >= 0xe0
				   && static_cast<uint8_t>(code) <= 0xef) {
				multi_byte_hi_ = code;
			} else {
				// 無効キャラクターの意味として
				multi_byte_hi_ = 0;
				if(-FONT_WIDTH >= x || static_cast<uint16_t>(x) >= fb_width_) return;
				draw_image_P(x, y, &font6x12_[(1 << 3)], FONT_WIDTH, FONT_HEIGHT);
			}
#endif
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ascii テキストを描画する。
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	text	テキストのポインター
		@return 文字の終端座標（Ｘ）
	*/
	//-----------------------------------------------------------------//
	int16_t monograph::draw_string(int16_t x, int16_t y, const char* text)
	{
		char code;

		while((code = *text++) != 0) {
			draw_font(x, y, code);
			x += font_width_;
		}
		multi_byte_hi_ = 0;
		return x;
	}


	//-----------------------------------------------------------------//
	/*!
 		@brief	ascii プログラム領域のテキストを描画する。
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	text	テキストのポインター
		@return 文字の終端座標（Ｘ）
	*/
	//-----------------------------------------------------------------//
	int16_t monograph::draw_string_P(int16_t x, int16_t y, const char* text)
	{
		char code;
		while((code = pgm_read_byte_near(text)) != 0) {
			++text;
			draw_font(x, y, code);
			x += font_width_;
		}
		multi_byte_hi_ = 0;
		return x;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	テキスト列を中心に描画する。
		@param[in]	text	テキストのポインター
	*/
	//-----------------------------------------------------------------//
	void monograph::draw_string_center_P(const char* text)
	{
		int16_t xx = 0;
		int16_t yy = font_height_;
		char ch;
		int16_t l = 0;
		const char* p = text;
		while((ch = pgm_read_byte_near(p)) != 0) {
			p++;
			if(ch == '\n') {
				if(xx < l) xx = l;
				l = 0;
				yy += font_height_;
			} else {
				l += font_width_;
			}
		}
		if(xx < l) xx = l;
		--p;
		ch = pgm_read_byte_near(p);
		if(ch == '\n') yy -= font_height_;

		draw_string_P(64 - xx / 2, 32 - yy / 2, text);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	１６進数を表示する
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	value	値
		@param[in]	num	表示数
		@param[in]	zero_suppress	ゼロサプレスを行う場合「true」
		@return 文字の終端座標（Ｘ）
	*/
	//-----------------------------------------------------------------//
	int16_t monograph::draw_hexadecimal(int16_t x, int16_t y, uint32_t value, uint8_t num, bool zero_suppress)
	{
#if 0
		char text[10];
		char form[8];
		char* p = form;
		*p++ = '%';
		if(zero_suppress) *p++ = '0';
		while(num > 0) {
			*p++ = (num % 10) + '0';
			num /= 10; 
		}
		*p++ = 'X';
		*p++ = 0;
		sprintf(text, form, value);
		return draw_string(x, y, text);
#endif
return 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	１０進数を表示する
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	value	値
		@param[in]	num	表示数
		@param[in]	zero_suppress	ゼロサプレスを行う場合「true」
		@param[in]	sign	符号表示を行う場合「true」
		@return 文字の終端座標（Ｘ）
	*/
	//-----------------------------------------------------------------//
	int16_t monograph::draw_decimal(int16_t x, int16_t y, int32_t value, uint8_t num, bool zero_suppress, bool sign)
	{
#if 0
		char text[12];
		char form[8];
		char* p = form;
		*p++ = '%';
		if(sign) *p++ = '+';
		if(zero_suppress) *p++ = '0';
		while(num > 0) {
			*p++ = (num % 10) + '0';
			num /= 10; 
		}
		*p++ = 'd';
		*p++ = 0;
		sprintf(text, form, value);
		return draw_string(x, y, text);
#endif
return 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	水平レベルを表示
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	w	横幅
		@param[in]	h	高さ
		@param[in]	l	レベル
	*/
	//-----------------------------------------------------------------//
	void monograph::draw_holizontal_level(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t l)
	{
		frame(x, y, w, h, 1);
	  	if(w <= 2 || h <= 2) return;
		++x;
		h -= 2;
		++y;
		w -= 2;
		for(uint8_t j = 0; j < h; ++j) {
			for(uint8_t i = 0; i < w; ++i) {
				if(i < l) {
					if((i ^ j) & 1) {
						point_set(x + i, y + j);
					} else {
						point_reset(x + i, y + j);
					}
				} else if(i == l && i != 0) {
					point_set(x + i, y + j);
				} else {
					point_reset(x + i, y + j);
				}
			}
		}
	}


#ifdef TIME_FNC
	//-----------------------------------------------------------------//
	/*!
		@brief	日付表示 YYYY/MM/DD を行う
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	t tm 構造体
		@return 文字の終端座標（Ｘ）
	*/
	//-----------------------------------------------------------------//
	int16_t monograph::draw_date(int16_t x, int16_t y, const tm& t)
	{
		char text[16];

		sprintf_P(text, PSTR("%04d/%02d/%02d"),
			t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
 		return draw_string(x, y, text);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	時間表示 HH/MM/SS を行う
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	t	tm 構造体
		@return 文字の終端座標（Ｘ）
	*/
	//-----------------------------------------------------------------//
	void monograph::draw_time(int16_t x, int16_t y, const tm& t)
	{
		char text[16];

		sprintf_P(text, PSTR("%02d:%02d.%02d"),
			t.tm_hour, t.tm_min, t.tm_sec);
		draw_string(x, y, text);
	}
#endif

}

/* ----- End Of File "monograph.cpp" ----- */
