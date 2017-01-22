//=====================================================================//
/*!	@file
	@breif	モーション・オブジェクト関係（スプライト・データ）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>

#include "monograph.h"
#include "mobj.h"

// タイトル表示を有効にする場合
#define TITLE_ENABLE

// 8x12 ピクセルフォント
static const prog_char nmb12_0[] = {
#include "nmb12_0.h"
};
static const prog_char nmb12_1[] = {
#include "nmb12_1.h"
};
static const prog_char nmb12_2[] = {
#include "nmb12_2.h"
};
static const prog_char nmb12_3[] = {
#include "nmb12_3.h"
};
static const prog_char nmb12_4[] = {
#include "nmb12_4.h"
};
static const prog_char nmb12_5[] = {
#include "nmb12_5.h"
};
static const prog_char nmb12_6[] = {
#include "nmb12_6.h"
};
static const prog_char nmb12_7[] = {
#include "nmb12_7.h"
};
static const prog_char nmb12_8[] = {
#include "nmb12_8.h"
};
static const prog_char nmb12_9[] = {
#include "nmb12_9.h"
};
static const prog_char nmb12_co[] = {
#include "nmb12_co.h"
};
static const prog_char nmb12_po[] = {
#include "nmb12_po.h"
};
static const prog_char nmb12_sl[] = {
#include "nmb12_sl.h"
};
static const prog_char nmb12_x[] = {
#include "nmb12_x.h"
};
static const prog_char nmb12_a[] = {
#include "nmb12_a.h"
};
static const prog_char nmb12_b[] = {
#include "nmb12_b.h"
};
static const prog_char nmb12_v[] = {
#include "nmb12_v.h"
};
static const prog_char nmb12_c[] = {
#include "nmb12_c.h"
};
static const prog_char nmb12_do[] = {
#include "nmb12_do.h"
};

// 18x32 ピクセルフォント
static const prog_char nmb32_0[] = {
#include "nmb32_0.h"
};
static const prog_char nmb32_1[] = {
#include "nmb32_1.h"
};
static const prog_char nmb32_2[] = {
#include "nmb32_2.h"
};
static const prog_char nmb32_3[] = {
#include "nmb32_3.h"
};
static const prog_char nmb32_4[] = {
#include "nmb32_4.h"
};
static const prog_char nmb32_5[] = {
#include "nmb32_5.h"
};
static const prog_char nmb32_6[] = {
#include "nmb32_6.h"
};
static const prog_char nmb32_7[] = {
#include "nmb32_7.h"
};
static const prog_char nmb32_8[] = {
#include "nmb32_8.h"
};
static const prog_char nmb32_9[] = {
#include "nmb32_9.h"
};
static const prog_char nmb32_kmh[] = {
#include "nmb32_kmh.h"
};
static const prog_char nmb32_c[] = {
#include "nmb32_c.h"
};
static const prog_char nmb32_p[] = {
#include "nmb32_p.h"
};
static const prog_char nmb32_x[] = {
#include "nmb32_x.h"
};
static const prog_char nmb32_rpm[] = {
#include "nmb32_rpm.h"
};

#ifdef TITLE_ENABLE
static const prog_char g_title_bmp[] = {
#include "title_bmp.h"
};
#endif

static const prog_char g_batt_icon[] = {
#include "batt_icon.h"
};
static const prog_char g_batt_ext[] = {
#include "batt_ext.h"
};

static const char *nmb12[10] = {
	nmb12_0,
	nmb12_1,
	nmb12_2,
	nmb12_3,
	nmb12_4,
	nmb12_5,
	nmb12_6,
	nmb12_7,
	nmb12_8,
	nmb12_9,
};

static const char *nmb32[10] = {
	nmb32_0,
	nmb32_1,
	nmb32_2,
	nmb32_3,
	nmb32_4,
	nmb32_5,
	nmb32_6,
	nmb32_7,
	nmb32_8,
	nmb32_9,
};

//-----------------------------------------------------------------//
/*!
	@breif	１２ピクセル（高さ）を使ったキャラクター表示
	@param[in]	x	Ｘ軸位置
	@param[in]	y	Ｙ軸位置
	@param[in]	ch	キャラクター・コード
*/
//-----------------------------------------------------------------//
void mobj_draw_cha_12(short x, short y, char ch)
{
	if(ch >= '0' && ch <= '9') {
		monograph_draw_mobj_P(x, y, nmb12[ch - '0']);
	} else if(ch == 'A') {
		monograph_draw_mobj_P(x, y, nmb12_a);
	} else if(ch == 'B') {
		monograph_draw_mobj_P(x, y, nmb12_b);
	} else if(ch == 'C') {
		monograph_draw_mobj_P(x, y, nmb12_c);
	} else if(ch == 'V') {
		monograph_draw_mobj_P(x, y, nmb12_v);
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	１２ピクセル（高さ）を使った「電圧表示」
	@param[in]	x	Ｘ軸位置
	@param[in]	y	Ｙ軸位置
	@param[in]	n	電圧（10倍値）
*/
//-----------------------------------------------------------------//
void mobj_draw_volt_12(short x, short y, short n)
{
	unsigned char m = n % 10;
	n /= 10;
	monograph_draw_mobj_P(x+9*2+3, y, nmb12[m]);

	monograph_draw_mobj_P(x+9*2, y, nmb12_po);

	m = n % 10;
	n /= 10;
	monograph_draw_mobj_P(x+9*1, y, nmb12[m]);

	m = n % 10;
	if(m) {
		monograph_draw_mobj_P(x+9*0, y, nmb12[m]);
	}

	monograph_draw_mobj_P(x+9*3+3, y, nmb12_v);
}


//-----------------------------------------------------------------//
/*!
	@breif	１２ピクセル（高さ）を使った「温度表示」
	@param[in]	x	Ｘ軸位置
	@param[in]	y	Ｙ軸位置
	@param[in]	n	温度（10倍値）
*/
//-----------------------------------------------------------------//
void mobj_draw_temp_12(short x, short y, short n)
{
	unsigned char m = n % 10;
	n /= 10;
	monograph_draw_mobj_P(x+9*3+3, y, nmb12[m]);

	monograph_draw_mobj_P(x+9*3, y, nmb12_po);

	m = n % 10;
	n /= 10;
	monograph_draw_mobj_P(x+9*2, y, nmb12[m]);

	m = n % 10;
	n /= 10;
	if(n > 0 || m != 0) {
		monograph_draw_mobj_P(x+9*1, y, nmb12[m]);
	}

	m = n % 10;
	if(m) {
		monograph_draw_mobj_P(x+9*0, y, nmb12[m]);
	}

	monograph_draw_mobj_P(x+9*4+3+5, y, nmb12_c);
	monograph_draw_mobj_P(x+9*4+3, y, nmb12_do);

}


//-----------------------------------------------------------------//
/*!
	@breif	１２ピクセル（高さ）数字を使った時間表示（時：分）
	@param[in]	x	Ｘ軸位置
	@param[in]	y	Ｙ軸位置
	@param[in]	t	時間（秒単位）
*/
//-----------------------------------------------------------------//
void mobj_draw_time_hm_12(short x, short y, time_t t)
{
	unsigned char	m;

	t /= 60;

	m = t % 10;
	t /= 10;
	monograph_draw_mobj_P(x+9*3+3, y, nmb12[m]);

	m = t % 6;
	t /= 6;
	monograph_draw_mobj_P(x+9*2+3, y, nmb12[m]);

	monograph_draw_mobj_P(x+9*2+0, y, nmb12_c);

	m = t % 24;
	monograph_draw_mobj_P(x+9*1+0, y, nmb12[m % 10]);
	m /= 10;
	monograph_draw_mobj_P(x+0*1+0, y, nmb12[m]);
}


//-----------------------------------------------------------------//
/*!
	@breif	１２ピクセル（高さ）数字を使った時間表示（時：分．秒）
	@param[in]	x	Ｘ軸位置
	@param[in]	y	Ｙ軸位置
	@param[in]	t	時間（秒単位）
*/
//-----------------------------------------------------------------//
void mobj_draw_time_hms_12(short x, short y, time_t t)
{
	unsigned char	m;

	m = t % 10;
	t /= 10;
	monograph_draw_mobj_P(x+9*5+3+3, y, nmb12[m]);

	m = t % 6;
	t /= 6;
	monograph_draw_mobj_P(x+9*4+3+3, y, nmb12[m]);

	monograph_draw_mobj_P(x+9*4+3+0, y, nmb12_po);

	m = t % 10;
	t /= 10;
	monograph_draw_mobj_P(x+9*3+3, y, nmb12[m]);

	m = t % 6;
	t /= 6;
	monograph_draw_mobj_P(x+9*2+3, y, nmb12[m]);

	monograph_draw_mobj_P(x+9*2+0, y, nmb12_co);

	m = t % 24;
	monograph_draw_mobj_P(x+9*1+0, y, nmb12[m % 10]);
	m /= 10;
	monograph_draw_mobj_P(x+0*1+0, y, nmb12[m]);
}


//-----------------------------------------------------------------//
/*!
	@breif	１２ピクセル（高さ）数字を使った時間表示（分：秒．1/100）@n
			※９分を超えたら「X」と表示
	@param[in]	x	Ｘ軸位置
	@param[in]	y	Ｙ軸位置
	@param[in]	t	時間（1/100 秒単位）
*/
//-----------------------------------------------------------------//
void mobj_draw_time_mst_12(short x, short y, time_t t)
{
	unsigned char	m;

	m = t % 10;
	t /= 10;
	monograph_draw_mobj_P(x+9*4+3+3, y, nmb12[m]);
	m = t % 10;
	t /= 10;
	monograph_draw_mobj_P(x+9*3+3+3, y, nmb12[m]);

	monograph_draw_mobj_P(x+9*3+3+0, y, nmb12_po);

	m = t % 10;
	t /= 10;
	monograph_draw_mobj_P(x+9*2+3, y, nmb12[m]);
	m = t % 6;
	t /= 6;
	monograph_draw_mobj_P(x+9*1+3, y, nmb12[m]);

	monograph_draw_mobj_P(x+9*1+0, y, nmb12_co);

	if(t < 10) {
		monograph_draw_mobj_P(x+0*1+0, y, nmb12[t]);
	} else {
		monograph_draw_mobj_P(x+0*1+0, y, nmb12_x);
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	１２ピクセル（高さ）数字を使った３桁表示
	@param[in]	x	Ｘ軸位置
	@param[in]	y	Ｙ軸位置
	@param[in]	n	週回数
*/
//-----------------------------------------------------------------//
void mobj_draw_number3_12(short x, short y, unsigned short n)
{
	unsigned char	m;

	m = n % 10;
	n /= 10;
	monograph_draw_mobj_P(x+9*2, y, nmb12[m]);

	m = n % 10;
	n /= 10;
	monograph_draw_mobj_P(x+9*1, y, nmb12[m]);

	m = n % 10;
//	n /= 10;
	monograph_draw_mobj_P(x+9*0, y, nmb12[m]);
}


//-----------------------------------------------------------------//
/*!
	@breif	１２ピクセル（高さ）フォント用「/」表示
	@param[in]	x	Ｘ軸位置
	@param[in]	y	Ｙ軸位置
*/
//-----------------------------------------------------------------//
void mobj_draw_slash_12(short x, short y)
{
	monograph_draw_mobj_P(x, y, nmb12_sl);
}


//-----------------------------------------------------------------//
/*!
	@breif	１２ピクセル（高さ）数字を使った６桁表示
	@param[in]	x	Ｘ軸位置
	@param[in]	y	Ｙ軸位置
	@param[in]	n	週回数
*/
//-----------------------------------------------------------------//
void mobj_draw_number6_12(short x, short y, unsigned long n)
{
	unsigned char	m;

	m = n % 10;
	n /= 10;
	monograph_draw_mobj_P(x+9*5, y, nmb12[m]);

	m = n % 10;
	n /= 10;
	monograph_draw_mobj_P(x+9*4, y, nmb12[m]);

	m = n % 10;
	n /= 10;
	monograph_draw_mobj_P(x+9*3, y, nmb12[m]);

	m = n % 10;
	n /= 10;
	monograph_draw_mobj_P(x+9*2, y, nmb12[m]);

	m = n % 10;
	n /= 10;
	monograph_draw_mobj_P(x+9*1, y, nmb12[m]);

	m = n % 10;
//	n /= 10;
	monograph_draw_mobj_P(x+9*0, y, nmb12[m]);
}


//-----------------------------------------------------------------//
/*!
	@breif	１２ピクセル（高さ）数字を使った４．１桁表示
	@param[in]	x	Ｘ軸位置
	@param[in]	y	Ｙ軸位置
	@param[in]	n	週回数
*/
//-----------------------------------------------------------------//
void mobj_draw_number4d1_12(short x, short y, unsigned long n)
{
	unsigned char	m;

	m = n % 10;
	n /= 10;
	monograph_draw_mobj_P(x+9*4+3, y, nmb12[m]);

	monograph_draw_mobj_P(x+9*4, y, nmb12_po);

	m = n % 10;
	n /= 10;
	monograph_draw_mobj_P(x+9*3, y, nmb12[m]);

	m = n % 10;
	n /= 10;
	monograph_draw_mobj_P(x+9*2, y, nmb12[m]);

	m = n % 10;
	n /= 10;
	monograph_draw_mobj_P(x+9*1, y, nmb12[m]);

	m = n % 10;
//	n /= 10;
	monograph_draw_mobj_P(x+9*0, y, nmb12[m]);
}


//-----------------------------------------------------------------//
/*!
	@breif	１２ピクセル（高さ）数字を使った４桁表示
			※ゼロサプレス有り
	@param[in]	x	Ｘ軸位置
	@param[in]	y	Ｙ軸位置
	@param[in]	n	値
*/
//-----------------------------------------------------------------//
void mobj_draw_number4z_12(short x, short y, unsigned short n)
{
	unsigned char	m[4];
	char	f;

	f = 0;

	m[0] = n % 10;
	n /= 10;
	m[1] = n % 10;
	n /= 10;
	m[2] = n % 10;
	n /= 10;
	m[3] = n % 10;

	if(f != 0 || m[3] != 0) {
		monograph_draw_mobj_P(x+9*0, y, nmb12[m[3]]);
		f++;
	}
	if(f != 0 || m[2] != 0) {
		monograph_draw_mobj_P(x+9*1, y, nmb12[m[2]]);
		f++;
	}
	if(f != 0 || m[1] != 0) {
		monograph_draw_mobj_P(x+9*2, y, nmb12[m[1]]);
	}
	monograph_draw_mobj_P(x+9*3, y, nmb12[m[0]]);
}


//-----------------------------------------------------------------//
/*!
	@breif	１２ピクセル（高さ）数字を使った５桁表示
			※ゼロサプレス有り
	@param[in]	x	Ｘ軸位置
	@param[in]	y	Ｙ軸位置
	@param[in]	n	値
*/
//-----------------------------------------------------------------//
void mobj_draw_number5z_12(short x, short y, uint32_t n)
{
	unsigned char m[5];
	char	f;

	f = 0;

	m[0] = n % 10;
	n /= 10;
	m[1] = n % 10;
	n /= 10;
	m[2] = n % 10;
	n /= 10;
	m[3] = n % 10;
	n /= 10;
	m[4] = n % 10;

	if(m[4] != 0) {
		monograph_draw_mobj_P(x+9*0, y, nmb12[m[4]]);
		f++;
	}
	if(f != 0 || m[3] != 0) {
		monograph_draw_mobj_P(x+9*1, y, nmb12[m[3]]);
		f++;
	}
	if(f != 0 || m[2] != 0) {
		monograph_draw_mobj_P(x+9*2, y, nmb12[m[2]]);
		f++;
	}
	if(f != 0 || m[3] != 0) {
		monograph_draw_mobj_P(x+9*3, y, nmb12[m[1]]);
	}
	monograph_draw_mobj_P(x+9*4, y, nmb12[m[0]]);
}


//-----------------------------------------------------------------//
/*!
	@breif	３２ピクセル（高さ）数字を使った時間（ラップタイム）の表示@n
			※９分を超えたら「X」と表示
	@param[in]	x	Ｘ軸位置
	@param[in]	y	Ｙ軸位置
	@param[in]	t	1/100 秒単位の整数
*/
//-----------------------------------------------------------------//
void mobj_draw_time_32(short x, short y, time_t t)
{
	unsigned char m;

	m = t % 10;
	t /= 10;
	monograph_draw_mobj_P(x+20*4+6+6, y, nmb32[m]);

	m = t % 10;
	t /= 10;
	monograph_draw_mobj_P(x+20*3+6+6, y, nmb32[m]);

	monograph_draw_mobj_P(x+20*3+6, y, nmb32_p);

	m = t % 10;
	t /= 10;
	monograph_draw_mobj_P(x+20*2+6, y, nmb32[m]);

	m = t % 6;
	t /= 6;
	monograph_draw_mobj_P(x+20*1+6, y, nmb32[m]);

	monograph_draw_mobj_P(x+20*1, y, nmb32_p);

	if(t < 10) {
		monograph_draw_mobj_P(x, y, nmb32[t]);
	} else {
		monograph_draw_mobj_P(x, y, nmb32_x);
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	３２ピクセル（高さ）数字を使った３桁表示（速度表示用）
	@param[in]	x	Ｘ軸位置
	@param[in]	y	Ｙ軸位置
	@param[in]	v	値
*/
//-----------------------------------------------------------------//
void mobj_draw_speed_32(short x, short y, unsigned short v)
{
	unsigned char m[3];
	char zf;

	m[2] = v % 10;
	v /= 10;
	m[1] = v % 10;
	v /= 10;
	m[0] = v % 10;

	if(m[0] != 0) {
		monograph_draw_mobj_P(x+20*0, y, nmb32[m[0]]);
		zf = 1;
	} else {
		zf = 0;
	}

	if(zf != 0 || m[1] != 0) {
		monograph_draw_mobj_P(x+20*1, y, nmb32[m[1]]);
	}

	monograph_draw_mobj_P(x+20*2, y, nmb32[m[2]]);

	monograph_draw_mobj_P(x+20*3, y+20, nmb32_kmh);
}


//-----------------------------------------------------------------//
/*!
	@breif	３２ピクセル（高さ）数字を使った５桁表示（ゼロサプレス）
	@param[in]	x	Ｘ軸位置
	@param[in]	y	Ｙ軸位置
	@param[in]	v	値
*/
//-----------------------------------------------------------------//
void mobj_draw_number5_32(short x, short y, unsigned long v)
{
	unsigned char m[5];
	char zf;
	unsigned char i;

	m[4] = v % 10;
	v /= 10;
	m[3] = v % 10;
	v /= 10;
	m[2] = v % 10;
	v /= 10;
	m[1] = v % 10;
	v /= 10;
	m[0] = v % 10;

	zf = 0;
	for(i = 0; i < 5; ++i) {
		if(i == (5 - 1)) ++zf;
		if(zf != 0 || m[i] != 0) {
			monograph_draw_mobj_P(x, y, nmb32[m[i]]);
			++zf;
		}
		x += 20;
	}

	monograph_draw_mobj_P(x, y+18, nmb32_rpm);
}


//-----------------------------------------------------------------//
/*!
	@breif	タイトル描画
*/
//-----------------------------------------------------------------//
void mobj_draw_title(void)
{
#ifdef TITLE_ENABLE
	monograph_draw_mobj_P(0, 0, g_title_bmp);
#endif
}


//-----------------------------------------------------------------//
/*!
	@breif	バッテリー・アイコン描画
	@param[in]	x	Ｘ軸位置
	@param[in]	y	Ｙ軸位置
	@param[in]	t	タイプ（0 to 15: level, -1: ext)
*/
//-----------------------------------------------------------------//
void mobj_draw_batt(short x, short y, char t)
{
	if(t >= 0) {
		monograph_draw_mobj_P(x, y, g_batt_icon);
		monograph_fill(x + 2, y + 2, t, 6, 1);
	} else {
		monograph_draw_mobj_P(x, y, g_batt_ext);
	}
}


/* ----- End Of File "mobj.c" ----- */
