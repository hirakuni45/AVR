#ifndef EEPROM_MAN_H
#define EEPROM_MAN_H
//=====================================================================//
/*!	@file
	@breif	EEPROM マネージメント関係（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/pgmspace.h>

// for ATmega1284P EEPROM area (4 KBytes)

#define EEPROM_LAP_STREAM_TOP	0
#define EEPROM_LAP_STREAM_LEN	(1024 * 3)

enum EEPROM_ID {

	EEPROM_ID_TRIP_INDEX       = 4059,		///< 1 bytes トリップ・インデックス
	EEPROM_ID_TOTAL_DISTANCE   = 4060,		///< 4 bytes 合計距離
	EEPROM_ID_TOTAL_TRIP_A     = 4064,		///< 4 bytes トリップＡ
	EEPROM_ID_TOTAL_TRIP_B     = 4068,		///< 4 bytes トリップＢ

	EEPROM_ID_PLUS_INPUT_MODE  = 4072,		///< 1 byte 0:Speed, 1:R.P.M.
	EEPROM_ID_PLUS_PER_RAD_SPD = 4073,		///< 1 byte Puls Per Radian at Speed
	EEPROM_ID_PLUS_PER_RAD_RPM = 4074,		///< 1 byte Puls Per Radian at R.P.M.
	EEPROM_ID_PLUS_RAD_DISTANCE= 4075,		///< 2 byte Radian at Distance

	EEPROM_ID_LOG_GPS_ENABLE   = 4080,		///< 1 byte 0:No, 1:Yes
	EEPROM_ID_LOG_ADC_ENABLE   = 4081,		///< 1 byte 0:No, 1:Yes
	EEPROM_ID_LOG_PULS_ENABLE  = 4082,		///< 1 byte 0:No, 1:Yes

	EEPROM_ID_TITLE_MODE       = 4090,		///< 1 byte 0:none, 1:default, 2:user
	EEPROM_ID_STARTUP_MODE     = 4091,		///< 1 byte 0:Speed, 1:Taco, 2:Menu 3:LapTimer
	EEPROM_ID_KANJI_MODE       = 4092,		///< 1 byte 0:none, 1:Kanji

	EEPROM_ID_GPS_EXT_ECHO     = 4093,		///< 1 byte 0:No, 1:Yes
	EEPROM_ID_GPS_10HZ_ENABLE  = 4094,		///< 1 byte 0:10Hz, 1:1Hz
	EEPROM_ID_GPS_AUTO_ADJUST  = 4095,		///< 1 byte 0:No, 1:Yes
};


#ifdef __cplusplus
extern "C" {
#endif

	//-----------------------------------------------------------------//
	/*!
		@breif	EEPROM マネージャー初期化
		@param[in]	start	開始アドレス
		@param[in]	length	長さ
	*/
	//-----------------------------------------------------------------//
	void eeprom_man_init(uint16_t start, uint16_t length);



#ifdef __cplusplus
};
#endif

#endif // EEPROM_MAN_H
