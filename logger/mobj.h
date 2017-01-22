#ifndef MOBJ_H
#define MOBJ_H
//=====================================================================//
/*!	@file
	@breif	モーション・オブジェクト関係（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <stdio.h>
#include <stdlib.h>
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

	//-----------------------------------------------------------------//
	/*!
		@breif	１２ピクセル（高さ）を使ったキャラクター表示
		@param[in]	x	Ｘ軸位置
		@param[in]	y	Ｙ軸位置
		@param[in]	ch	キャラクター・コード
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_cha_12(short x, short y, char ch);


	//-----------------------------------------------------------------//
	/*!
		@breif	１２ピクセル（高さ）を使った「電圧表示」
		@param[in]	x	Ｘ軸位置
		@param[in]	y	Ｙ軸位置
		@param[in]	n	電圧（10倍値）
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_volt_12(short x, short y, short n);


	//-----------------------------------------------------------------//
	/*!
		@breif	１２ピクセル（高さ）を使った「温度表示」
		@param[in]	x	Ｘ軸位置
		@param[in]	y	Ｙ軸位置
		@param[in]	n	温度（10倍値）
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_temp_12(short x, short y, short n);


	//-----------------------------------------------------------------//
	/*!
		@breif	１２ピクセル（高さ）数字を使った時間表示（時：分）
		@param[in]	x	Ｘ軸位置
		@param[in]	y	Ｙ軸位置
		@param[in]	t	時間（秒単位）
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_time_hm_12(short x, short y, time_t t);


	//-----------------------------------------------------------------//
	/*!
		@breif	１２ピクセル（高さ）数字を使った時間表示（時：分．秒）
		@param[in]	x	Ｘ軸位置
		@param[in]	y	Ｙ軸位置
		@param[in]	t	時間（秒単位）
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_time_hms_12(short x, short y, time_t t);


	//-----------------------------------------------------------------//
	/*!
		@breif	１２ピクセル（高さ）数字を使った時間表示（分：秒．1/100）@n
			※９分を超えたら「X」と表示
		@param[in]	x	Ｘ軸位置
		@param[in]	y	Ｙ軸位置
		@param[in]	t	時間（1/100 秒単位）
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_time_mst_12(short x, short y, time_t t);


	//-----------------------------------------------------------------//
	/*!
		@breif	１２ピクセル（高さ）数字を使った３桁表示
		@param[in]	x	Ｘ軸位置
		@param[in]	y	Ｙ軸位置
		@param[in]	n	週回数
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_number3_12(short x, short y, unsigned short n);


	//-----------------------------------------------------------------//
	/*!
		@breif	１２ピクセル（高さ）フォント用「/」表示
		@param[in]	x	Ｘ軸位置
		@param[in]	y	Ｙ軸位置
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_slash_12(short x, short y);


	//-----------------------------------------------------------------//
	/*!
		@breif	１２ピクセル（高さ）数字を使った６桁表示
		@param[in]	x	Ｘ軸位置
		@param[in]	y	Ｙ軸位置
		@param[in]	n	週回数
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_number6_12(short x, short y, unsigned long n);


	//-----------------------------------------------------------------//
	/*!
		@breif	１２ピクセル（高さ）数字を使った４．１桁表示
		@param[in]	x	Ｘ軸位置
		@param[in]	y	Ｙ軸位置
		@param[in]	n	週回数
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_number4d1_12(short x, short y, unsigned long n);


	//-----------------------------------------------------------------//
	/*!
		@breif	１２ピクセル（高さ）数字を使った４桁表示(ゼロサプレス有り)
		@param[in]	x	Ｘ軸位置
		@param[in]	y	Ｙ軸位置
		@param[in]	n	値
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_number4z_12(short x, short y, unsigned short n);


	//-----------------------------------------------------------------//
	/*!
		@breif	１２ピクセル（高さ）数字を使った５桁表示(ゼロサプレス有り)
		@param[in]	x	Ｘ軸位置
		@param[in]	y	Ｙ軸位置
		@param[in]	n	値
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_number5z_12(short x, short y, uint32_t n);


	//-----------------------------------------------------------------//
	/*!
		@breif	３２ピクセル（高さ）数字を使った時間（ラップタイム）の表示@n
			※９分を超えたら「X」と表示
		@param[in]	x	Ｘ軸位置
		@param[in]	y	Ｙ軸位置
		@param[in]	t	1/100 秒単位の整数
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_time_32(short x, short y, time_t t);


	//-----------------------------------------------------------------//
	/*!
		@breif	３２ピクセル（高さ）数字を使った３桁表示（速度表示用）
		@param[in]	x	Ｘ軸位置
		@param[in]	y	Ｙ軸位置
		@param[in]	v	値
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_speed_32(short x, short y, unsigned short v);


	//-----------------------------------------------------------------//
	/*!
		@breif	３２ピクセル（高さ）数字を使った５桁表示
		@param[in]	x	Ｘ軸位置
		@param[in]	y	Ｙ軸位置
		@param[in]	v	値
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_number5_32(short x, short y, unsigned long v);


	//-----------------------------------------------------------------//
	/*!
		@breif	タイトル描画
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_title(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	バッテリー・アイコン描画
		@param[in]	x	Ｘ軸位置
		@param[in]	y	Ｙ軸位置
		@param[in]	t	タイプ（0 to 15: level, -1: ext)
	*/
	//-----------------------------------------------------------------//
	void mobj_draw_batt(short x, short y, char t);


#ifdef __cplusplus
};
#endif

#endif // MOBJ_H