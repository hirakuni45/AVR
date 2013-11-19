//=====================================================================//
/*!	@file
	@brief	ドットマトリックス用描画クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/pgmspace.h>
#include "dm_draw.hpp"

namespace graphics {

	static const uint8_t font3x5_[] PROGMEM = {
	#include "font3x5.h"
	};
	static const int font3x5_block_ = (3 * 5 + 7) >> 3;

	static const uint8_t font7x10_[] PROGMEM = {
	#include "font7x10.h"
	};
	static const int font7x10_block_ = (7 * 10 + 7) >> 3;

	static const uint8_t icon_[] PROGMEM = {
	#include "icon.h"
	};
	static const int icon_block_ = (16 * 16 + 7) >> 3;

	//-----------------------------------------------------------------//
	/*!
		@brief	3x5 オブジェクトの描画
		@param[in]	x	X 位置
		@param[in]	y	Y 位置
		@param[in]	n	オブジェクト番号
	*/
	//-----------------------------------------------------------------//
	void dm_draw::draw_3x5(short x, short y, uint8_t n)
	{
		mg_.draw_image_P(x, y, &font3x5_[n * font3x5_block_], 3, 5); 
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	7x10 オブジェクトの描画
		@param[in]	x	X 位置
		@param[in]	y	Y 位置
		@param[in]	n	オブジェクト番号
	*/
	//-----------------------------------------------------------------//
	void dm_draw::draw_7x10(short x, short y, uint8_t n)
	{
		mg_.draw_image_P(x, y, &font7x10_[n * font7x10_block_], 7, 10); 
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	icon オブジェクトの描画
		@param[in]	x	X 位置
		@param[in]	y	Y 位置
		@param[in]	n	オブジェクト番号
	*/
	//-----------------------------------------------------------------//
	void dm_draw::draw_icon(short x, short y, uint8_t n)
	{
		mg_.draw_image_P(x, y, &icon_[n * icon_block_], 16, 16); 
	}



}
