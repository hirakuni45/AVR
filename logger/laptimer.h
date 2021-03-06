#ifndef LAPTIMER_H
#define LAPTIMER_H
//=====================================================================//
/*!	@file
	@breif	ラップタイマー関係（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

	//-----------------------------------------------------------------//
	/*!
		@breif	ラップタイマー表示初期化
	*/
	//-----------------------------------------------------------------//
	void laptimer_init(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	ラップタイマーメニュー
	*/
	//-----------------------------------------------------------------//
	void laptimer_menu(void);


#if 0
	//-----------------------------------------------------------------//
	/*!
		@breif	ラップタイマーセットアップ初期化
	*/
	//-----------------------------------------------------------------//
	void laptimer_setup_init(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	ラップタイマーセットアップメイン
	*/
	//-----------------------------------------------------------------//
	void laptimer_setup_main(void);
#endif


	//-----------------------------------------------------------------//
	/*!
		@breif	ラップタイマー・サービス（サーバー）@n
				・1/100 秒間隔の割り込みから呼ばれる。@n
				・ラップセンサー信号の評価と、ラップタイム記録
		@param[in]	pit	ピットフラグ
	 */
	//-----------------------------------------------------------------//
	void laptimer_int_service(char pit);


	//-----------------------------------------------------------------//
	/*!
		@breif	ラップタイマー表示メイン
	*/
	//-----------------------------------------------------------------//
	void laptimer_main(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	ラップタイマーデータセーブ
	*/
	//-----------------------------------------------------------------//
	void laptimer_save(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	リコール表示初期化（ラップタイムテーブルの表示）
	*/
	//-----------------------------------------------------------------//
	void recall_init(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	リコールデータ無し
	*/
	//-----------------------------------------------------------------//
	void recall_nodata(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	リコール表示メイン（ラップタイムの表示）
	*/
	//-----------------------------------------------------------------//
	void recall_date(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	リコール表示メイン（ラップタイムの表示）
	*/
	//-----------------------------------------------------------------//
	void recall_main(void);

#ifdef __cplusplus
};
#endif

#endif // LAPTIMER_H
