#ifndef SPEED_H
#define SPEED_H
//=====================================================================//
/*!	@file
	@breif	スピード表示関係（ヘッダー）
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
		@breif	速度サービス・初期化
	*/
	//-----------------------------------------------------------------//
	void speed_service_init(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	速度サービス・メイン
	*/
	//-----------------------------------------------------------------//
	void speed_service_main(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	速度表示初期化
	*/
	//-----------------------------------------------------------------//
	void speed_init(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	速度表示メイン
	*/
	//-----------------------------------------------------------------//
	void speed_main(void);

#ifdef __cplusplus
};
#endif

#endif // SPEED_H
