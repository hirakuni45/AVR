#ifndef SHELL_H
#define SHELL_H
//=====================================================================//
/*!	@file
	@breif	コマンド・シェル（ヘッダー）
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
		@breif	コマンド・シェル初期化
	*/
	//-----------------------------------------------------------------//
	void shell_init(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	コマンド・シェル・サービス
	*/
	//-----------------------------------------------------------------//
	void shell_service(void);

#ifdef __cplusplus
};
#endif

#endif // SHELL_H
