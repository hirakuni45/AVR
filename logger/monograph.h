#ifndef MONOGRAPH_H
#define MONOGRAPH_H
//=====================================================================//
/*!	@file
	@breif	ビットマップ・グラフィックスの為のライブラリー（ヘッダー）
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
		@breif	初期化
		@return	フレームバッファのポインター
	*/
	//-----------------------------------------------------------------//
	unsigned char *monograph_init(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	点を描画する
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
	*/
	//-----------------------------------------------------------------//
	void monograph_point_set(short x, short y);


	//-----------------------------------------------------------------//
	/*!
		@breif	点を消去する
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
	*/
	//-----------------------------------------------------------------//
	void monograph_point_reset(short x, short y);


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
	void monograph_fill(short x, short y, short w, short h, char c);


	//-----------------------------------------------------------------//
	/*!
		@breif	全画面クリアをする
		@param[in]	c	クリアカラー
	*/
	//-----------------------------------------------------------------//
	void monograph_clear(char c);


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
	void monograph_line(short x1, short y1, short x2, short y2, char c);


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
	void monograph_frame(short x, short y, short w, short h, char c);


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
	void monograph_draw_image_P(short x, short y, const prog_char *img, unsigned char w, unsigned char h);


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
	void monograph_draw_image(short x, short y, const unsigned char *img, unsigned char w, unsigned char h);


	//-----------------------------------------------------------------//
	/*!
		@breif	モーションオブジェクトを描画する
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	img	描画ソースのポインター
	*/
	//-----------------------------------------------------------------//
	void monograph_draw_mobj_P(short x, short y, const prog_char *img);


	//-----------------------------------------------------------------//
	/*!
		@breif	ascii フォントを描画する。
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	code	ASCII コード（0x00 to 0x7F)
	*/
	//-----------------------------------------------------------------//
	void monograph_draw_font(short x, short y, char code);


	//-----------------------------------------------------------------//
	/*!
		@breif	ascii テキストを描画する。
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	text	テキストのポインター
		@return 文字の終端座標（Ｘ）
	*/
	//-----------------------------------------------------------------//
	short monograph_draw_string(short x, short y, const char *text);


	//-----------------------------------------------------------------//
	/*!
		@breif	ascii プログラム領域のテキストを描画する。
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	text	テキストのポインター
		@return 文字の終端座標（Ｘ）
	*/
	//-----------------------------------------------------------------//
	short monograph_draw_string_P(short x, short y, const prog_char *text);


	//-----------------------------------------------------------------//
	/*!
		@breif	テキスト列を中心に描画する。
		@param[in]	text	テキストのポインター
	*/
	//-----------------------------------------------------------------//
	void monograph_draw_string_center_P(const prog_char *text);


	//-----------------------------------------------------------------//
	/*!
		@breif	日付表示 YYYY/MM/DD を行う
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	tmp tm 構造体のポインター
	*/
	//-----------------------------------------------------------------//
	void monograph_draw_date(short x, short y, const struct tm *tmp);


	//-----------------------------------------------------------------//
	/*!
		@breif	時間表示 HH/MM/SS を行う
		@param[in]	x	開始点Ｘ軸を指定
		@param[in]	y	開始点Ｙ軸を指定
		@param[in]	tmp tm 構造体のポインター
	*/
	//-----------------------------------------------------------------//
	void monograph_draw_time(short x, short y, const struct tm *tmp);


#ifdef __cplusplus
};
#endif

#endif // MONOGRAPH_H
