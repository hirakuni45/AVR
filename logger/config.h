#ifndef CONFIG_H
#define CONFIG_H
//=====================================================================//
/*!	@file
	@breif	コンフィグレーション関係（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <stdio.h>
#include <stdlib.h>

extern uint8_t g_log_gps_enable;
extern uint8_t g_log_adc_enable;
extern uint8_t g_log_puls_enable;

extern uint8_t g_title_mode;
extern uint8_t g_startup_mode;
extern uint8_t g_kanji_mode;

extern uint8_t g_gps_interval;
extern uint8_t g_gps_auto_adjust;
extern uint8_t g_gps_ext_echo;

extern uint8_t g_pi_mode;
extern uint8_t g_pi_ppr_spd;
extern uint8_t g_pi_ppr_rpm;
extern uint16_t g_pi_distance;

#ifdef __cplusplus
extern "C" {
#endif

	//-----------------------------------------------------------------//
	/*!
		@breif	設定ロード
	*/
	//-----------------------------------------------------------------//
	void load_configuration(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	設定セーブ
	*/
	//-----------------------------------------------------------------//
	void save_configuration(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	コンフィグ表示初期化
	*/
	//-----------------------------------------------------------------//
	void config_init(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	コンフィグ表示メイン
	*/
	//-----------------------------------------------------------------//
	void config_main(void);


#ifdef __cplusplus
};
#endif

#endif // CONFIG_H
