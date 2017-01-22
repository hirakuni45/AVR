#ifndef MCP3208_H
#define MCP3208_H
//=====================================================================//
/*!	@file
	@breif	MCP3208(A/D Converter) の制御モジュール（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <stdio.h>
#include <stdlib.h>
#include "ff.h"

#ifdef __cplusplus
extern "C" {
#endif

	//-----------------------------------------------------------------//
	/*!
		@breif	MCP3208 指定のチャネルをA/D変換
		@param[in]	chanel	チャネル
		@return 12ビットの変換データ
	*/
	//-----------------------------------------------------------------//
	uint16_t read_chanel(unsigned char chanel);


	//-----------------------------------------------------------------//
	/*!
		@breif	MCP3208 ハードウェアー関係の初期化など
	*/
	//-----------------------------------------------------------------//
	void mcp3208_init(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	MCP3208 割り込みから呼ばれる
	*/
	//-----------------------------------------------------------------//
	void mcp3208_int_service();


	//-----------------------------------------------------------------//
	/*!
		@breif	MCP3208 全チャネル A/D 変換サービス
		@param[in]	fd	ファイル・ディスクリプタ
	*/
	//-----------------------------------------------------------------//
	void mcp3208_service(FIL *fd);


	//-----------------------------------------------------------------//
	/*!
		@breif	MCP3208 の A/D 変換データを得る
		@param[in]	chanel	A/D 変換チャネル番号（0〜7）
		@return		A/D 値（0〜4095）
	*/
	//-----------------------------------------------------------------//
	inline uint16_t mcp3208_get(char chanel);


	//-----------------------------------------------------------------//
	/*!
		@breif	A/D 変換 FIFO サービスの許可
		@param[in]	enable	許可の場合「０」以外
	*/
	//-----------------------------------------------------------------//
	void mcp3208_fifo_enable(char enable);


	//-----------------------------------------------------------------//
	/*!
		@breif	バッテリー電圧の取得
		@return	バッテリー電圧
	*/
	//-----------------------------------------------------------------//
	uint16_t get_battery(void);

#ifdef __cplusplus
};
#endif

#endif // MCP3208_H
