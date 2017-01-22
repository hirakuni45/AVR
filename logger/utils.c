//=====================================================================//
/*!	@file
	@breif	色々なユーティリテー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "utils.h"
#include <string.h>
#include <string.h>
#include "monograph.h"
#include "mainloop.h"
#include "mcp3208.h"
#include "psound.h"
#include "mobj.h"

static char g_menu_pos;
static char g_menu_num;
static char g_menu_ofsx;
static char g_menu_ofsy;
static char g_centerring;
static unsigned char g_menu_w;
static unsigned char g_menu_h;
static char g_menu_batt;
static unsigned char g_menu_text_max;
static const prog_char *g_menu_str;
static unsigned char g_sepos_x;
static unsigned char g_sepos_y;
static uint8_t g_menu_repeat_delay;
static char g_menu_repeat;

// 文字/数字入力時のメタ・キャラクター・コード
#define EDIT_CHAR_CODE		0x01

// リピートまでの時間差
#define REPEAT_COUNT_DELAY	20

//-----------------------------------------------------------------//
/*!
	@breif	プログラム領域に置かれたテキスト（「0」ターミネート@n
			された複数アイテム）のアイテム数を数える。@n
			例：３つのアイテムがある場合	@n
				const prog_char* list = {	@n
					"Item1\0"				@n
					"Item2\0"				@n
					"Item3\0"				@n
				};							@n
			※↑の場合、３が帰る。			
	@param[in]	str	プログラム・エリアのポインター
	@return		アイテム数を返す
*/
//-----------------------------------------------------------------//
short scan_prog_text_num_P(const prog_char *str)
{
	const prog_char *p;
	short	i;

	for (p = str, i = 0; pgm_read_byte_near(p) != 0; i++) {
		while(pgm_read_byte_near(p++));
	}
	return i;
}


//-----------------------------------------------------------------//
/*!
	@breif	プログラム領域に格納たテキスト（「0」ターミネート	@n
			された複数アイテム）の指定したアイテムを取り出す。	@n
				const prog_char* list = {						@n
					"Item1\0"									@n
					"Item2\0"									@n
					"Item3\0"									@n
				};												@n
			※↑の場合、no:1 の答えとして、"Item2" のプログラム	@n
			領域ポインターが返る。								
	@param[in]	str	プログラム・エリアのポインター
	@param[in]	no	取り出す位置
	@return		プログラムエリアのポインター					@n
				※アイテム数より大きな位置を指定した場合、「0」	@n
				ターミネートされたプログラム領域のポインターが	@n
				返る。
*/
//-----------------------------------------------------------------//
const prog_char *get_prog_text_P(const prog_char *str, uint8_t no)
{
	const prog_char *p;
	uint8_t i;

	for(p = str, i = 0; i != no && pgm_read_byte_near(p); i++) {
		while(pgm_read_byte_near(p++));
	}
	return p;
}


//-----------------------------------------------------------------//
/*!
	@breif	ユーティリティー・メニューの初期化@n
			・複数文字列を縦に並べて表示し、上下ボタンで選択させる@n
			インターフェースを持った、メニュー選択形式。
	@param[in]	str	プログラム領域に格納たテキスト（「0」ターミ@n
					ネートされた複数アイテム）のポインター
	@param[in]	batt	バッテリーインジケーターを表示する場合「1」
*/
//-----------------------------------------------------------------//
void util_menu_init(const prog_char *str, char batt)
{
	char	i;
	const prog_char *p;
	unsigned char	max;

	g_menu_pos = 0;
	g_menu_num = scan_prog_text_num_P(str);
	g_menu_text_max = 0;
	for(i = 0; i < g_menu_num; ++i) {
		p = get_prog_text_P(str, i);
		max = strlen_P(p);
		if(g_menu_text_max < max) g_menu_text_max = max;
	}
	g_menu_str = str;

	g_menu_batt = batt;
	if(batt) {
		g_menu_ofsx = 0;
		g_menu_ofsy = 12;

		g_menu_w = 128;
		g_menu_h = 64 - 12;
	} else {
		g_menu_ofsx = 0;
		g_menu_ofsy = 0;

		g_menu_w = 128;
		g_menu_h = 64;
	}

	g_centerring = 0;

	g_menu_repeat_delay = 0;
	g_menu_repeat = -1;
}


//-----------------------------------------------------------------//
/*!
	@breif	メニューの幅を取得
	@return 最大幅
*/
//-----------------------------------------------------------------//
unsigned char util_menu_get_width(void)
{
	return g_menu_text_max * 6;
}


//-----------------------------------------------------------------//
/*!
	@breif	メニューの高さを取得
	@return 最大幅
*/
//-----------------------------------------------------------------//
unsigned char util_menu_get_height(void)
{
	return g_menu_num * 12;
}


//-----------------------------------------------------------------//
/*!
	@breif	メニューのセンターリングを禁止する
	@param[in]	bits	bit0 が横方向、bit1 が縦方向に対応
*/
//-----------------------------------------------------------------//
void util_menu_centerring_disable(char bits)
{
	g_centerring = bits;
}


//-----------------------------------------------------------------//
/*!
	@breif	ユーティリティー・メニューの表示オフセットを設定@n
			※初期化後に呼ぶ事
	@param[in]	ofsx	X 軸のオフセット
	@param[in]	ofsy	Y 軸のオフセット
*/
//-----------------------------------------------------------------//
void util_menu_offset(char ofsx, char ofsy)
{
	g_menu_ofsx = ofsx;
	g_menu_ofsy = ofsy;
}


//-----------------------------------------------------------------//
/*!
	@breif	ユーティリティー・メニューの表示サイズを設定
			※初期化後に呼ぶ事
	@param[in]	ofsx	X 軸のオフセット
	@param[in]	ofsy	Y 軸のオフセット
*/
//-----------------------------------------------------------------//
void util_menu_size(unsigned char w, unsigned char h)
{
	g_menu_w = w;
	g_menu_h = h;
}


//-----------------------------------------------------------------//
/*!
	@breif	キャンセル・ボタンの判定
	@return キャンセル・ボタンが押されたら、「０」以外の値
*/
//-----------------------------------------------------------------//
char util_get_cancel(void)
{
	char f = g_switch_positive & SWITCH_BIT_CANCEL;
	if(f) {
		psound_request(0 + 12 * 0, 5);
	}
	return f;
}


//-----------------------------------------------------------------//
/*!
	@breif	ボタンがホールドされリピートされているか検査
	@return リピート状態ならメニュー位置を返す、リピート無しなら負の値
*/
//-----------------------------------------------------------------//
char util_menu_get_repeat()
{
	return g_menu_repeat;
}


//-----------------------------------------------------------------//
/*!
	@breif	ユーティリティー・メニュー・サービス（許可ビットで制御付き）@n
			※事前にメニューを「util_menu_init」を使って初期化して@n
			おく必要がある。
	@param[in]	mpos	メニュー表示と消す位置（-1の時無効）
	@return -1 の場合、選択中状態、正の整数の場合、選択されたメニュー
			位置。
*/
//-----------------------------------------------------------------//
char util_menu_service(char mpos)
{
	unsigned char a;
	const prog_char *p;
	char i;
	short x, y;

	if(g_menu_batt) {
		if(g_ext_battery) {
			mobj_draw_batt(1, 1, -1);
		} else {
			uint16_t v = get_battery();
			v -= BATTERY_LOW_LIMIT;
			if(v >= 0) {
				v *= 15;
				v /= BATTERY_HIGH_LIMIT - BATTERY_LOW_LIMIT;
				if(v > 15) v = 15;
				mobj_draw_batt(1, 1, v);
			} else {
				if((g_loop_count & 15) > 6) {
					mobj_draw_batt(1, 1, 0);
				}
			}
		}

		const struct tm *tmp = get_time();
		monograph_draw_time(128 - (6 * 8), 0, tmp);

		monograph_line(0, 12, 127, 12, 1);
	}

	if(mpos != g_menu_pos) {
		i = g_menu_pos;
		if(g_switch_positive & SWITCH_BIT_DOWN) ++g_menu_pos;
		if(g_switch_positive & SWITCH_BIT_UP) --g_menu_pos;
		if(g_menu_pos < 0) {
			g_menu_pos = 0;
		} else if(g_menu_pos >= g_menu_num) {
			g_menu_pos = g_menu_num - 1;
		}
		if(i != g_menu_pos) {
			psound_request(9 + 12 * 4, 1);
		}
	}

	if(g_centerring & 1) {
		x = 0;
	} else {
		x = (g_menu_w - (6 * g_menu_text_max)) / 2;
	}
	if(g_centerring & 2) {
		y = 0;
	} else {
		y = (g_menu_h - (12 * g_menu_num)) / 2;
	}
	x += g_menu_ofsx;
	y += g_menu_ofsy;

	x -= 8;
	a = g_loop_count;
	if((a & 15) > 6) {
		monograph_draw_font(x, y + (g_menu_pos * 12), 4);
	}

	x += 8;
	for(i = 0; i < g_menu_num; ++i) {
		p = get_prog_text_P(g_menu_str, i);
		monograph_draw_string_P(x, y, p);
		if(mpos == i) {
			g_sepos_x = x + strlen_P(p) * 6;
			g_sepos_y = y;
		}
		y += 12;
	}

	if(mpos != g_menu_pos) {
		if(g_switch_level & SWITCH_BIT_LAP) {
#if 0
			if(g_menu_repeat_delay >= REPEAT_COUNT_DELAY) {
				g_menu_repeat = g_menu_pos;
			} else {
				++g_menu_repeat_delay;
			}
#endif
		} else {
#if 0
			g_menu_repeat_delay = 0;
			g_menu_repeat = -1;
#endif
		}
		if(g_switch_positive & SWITCH_BIT_LAP) {
			static prog_uint8_t ok[] = {
				PSD_G + 12 * 2 + 1, 2,
				PSD_F + 12 * 2 + 1, 3,
				PSD_D + 12 * 2 + 1, 2,
				0
			};
			psound_play_P(ok);
			return g_menu_pos;
		}
	}

	return -1;
}


//-----------------------------------------------------------------//
/*!
	@breif	文字列を特定のキャラクターコードで置き換える
	@param[in]	text	文字列のポインター
	@param[in]	font	置き換えるキャラコード
*/
//-----------------------------------------------------------------//
void util_text_font_fill(char *text, char font)
{
	while(*text != 0) {
		*text++ = font;
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	汎用的な数字の入力と、表示
	@param[in]	x	数字表示の X 軸
	@param[in]	y	数字表示の Y 軸
	@param[in]	ctrl
	@param[in]	min_limit	入力値の下限設定
	@param[in]	max_limit	入力値の上限設定
	@param[in]	val	値を格納するポインター
*/
//-----------------------------------------------------------------//
void util_input_number(short x, short y, char ctrl, short min_limit, short max_limit, short *val)
{
	char	text[8];
	unsigned char a;
	short	v = *val;

	sprintf_P(text, PSTR("%d"), v);
	if(ctrl) {
		a = g_loop_count;
		if((a & 15) > 8) {
			util_text_font_fill(text, EDIT_CHAR_CODE);
		}
		monograph_draw_string(x, y, text);
	} else {
		monograph_draw_string(x, y, text);
	}
	if(ctrl == 0) return;

	if(g_switch_positive & SWITCH_BIT_UP) ++v;
	if(g_switch_positive & SWITCH_BIT_DOWN) --v;

	if(v < min_limit) v = min_limit;
	else if(v > max_limit) v = max_limit;

	*val = v;
}


//-----------------------------------------------------------------//
/*!
	@breif	汎用的な文字列テーブルの選択と表示
	@param[in]	x	文字列表示の X 軸
	@param[in]	y	文字列表示の Y 軸
	@param[in]	ctrl	コントロール
	@param[in]	str	文字列のポインター（プログラム領域）
	@param[in]	val	値を格納するポインター
*/
//-----------------------------------------------------------------//
void util_input_str_P(short x, short y, char ctrl, const prog_char *str, char *val)
{
	unsigned char	trg;
	const prog_char	*p;
	short	num;

	char v = *val;
	p = get_prog_text_P(str, v);
	if(ctrl) {
		trg = g_loop_count;
		if((trg & 15) > 6) {
			for(trg = 0; trg < strlen_P(p); ++trg) {
				monograph_draw_font(x + trg * 6, y, EDIT_CHAR_CODE);
			}
		} else {
			monograph_draw_string_P(x, y, p);
		}
	} else {
		monograph_draw_string_P(x, y, p);
	}
	if(ctrl == 0) return;

	num = scan_prog_text_num_P(str);

	if(g_switch_positive & SWITCH_BIT_UP) ++v;
	if(g_switch_positive & SWITCH_BIT_DOWN) --v;

	if(v < 0) v = 0;
	else if(v >= num) v = num - 1;

	*val = v;
}


//-----------------------------------------------------------------//
/*!
	@breif	文字列をセンターに表示
	@param[in]	ofsx	X 軸オフセット
	@param[in]	ofsy	Y 軸オフセット
	@param[in]	text	文字列
*/
//-----------------------------------------------------------------//
void util_draw_text_center(char ofsx, char ofsy, const char* text)
{
	int n = strlen(text);
	monograph_draw_string((g_menu_w - (6 * n)) / 2 + ofsx, (g_menu_h - 12) / 2 + ofsy, text);
}


//-----------------------------------------------------------------//
/*!
	@breif	文字列をセンターに表示
	@param[in]	ofsx	X 軸オフセット
	@param[in]	ofsy	Y 軸オフセット
	@param[in]	text	文字列
*/
//-----------------------------------------------------------------//
void util_draw_text_center_P(char ofsx, char ofsy, const prog_char* text)
{
	int n = strlen_P(text);
	monograph_draw_string_P((g_menu_w - (6 * n)) / 2 + ofsx, (g_menu_h - 12) / 2 + ofsy, text);
}


//-----------------------------------------------------------------//
/*!
	@breif	エディット・ボックスの描画
	@param[in]	x	X 軸位置
	@param[in]	y	Y 軸位置
	@param[in]	w	横幅
	@param[in]	h	高さ
	@param[in]	c	カラー
*/
//-----------------------------------------------------------------//
void util_draw_edit_box(short x, short y, uint8_t w, uint8_t h, char c)
{
	monograph_frame(x - 1, y - 2, w + 2, h + 4, c);
	monograph_frame(x - 2, y - 1, w + 4, h + 2, c);
}


/* ----- End Of File "utils.c" ----- */
