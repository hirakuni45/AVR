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

	static const uint8_t font4x5_[] PROGMEM = {
	#include "font4x5.h"
	};
	static const int font4x5_block_ = (4 * 5 + 7) >> 3;

	static const uint8_t font5x5_[] PROGMEM = {
	#include "font5x5.h"
	};
	static const int font5x5_block_ = (5 * 5 + 7) >> 3;

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
		@brief	4x5, 5x5 オブジェクトの幅を返す
		@param[in]	n	オブジェクト番号
		@return 横幅
	*/
	//-----------------------------------------------------------------//
	uint8_t dm_draw::get_width_xx5(uint8_t n)
	{
		if('A' <= n && n <= 'L') {
			if(n == 'I') return 3;
			else return 4;
		} else if(n == 'M' || n == 'N') {
			return 5;
		} else if('O' <= n && n <= 'S') {
			return 4;
		} else if(n == 'T') {
			return 5;
		} else if('U' == n) {
			return 4;
		} else if('V' <= n && n <= 'Y') {
			return 5;
		} else if('Z' == n) {
			return 4;
		}
		return 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	4x5, 5x5 オブジェクトの描画
		@param[in]	x	X 位置
		@param[in]	y	Y 位置
		@param[in]	n	オブジェクト番号
		@return 横幅
	*/
	//-----------------------------------------------------------------//
	short dm_draw::draw_xx5(short x, short y, uint8_t n)
	{
		if('A' <= n && n <= 'L') {
			mg_.draw_image_P(x, y, &font4x5_[(n - 'A') * font4x5_block_], 4, 5); 
			if(n == 'I') return 3;
			else return 4;
		} else if(n == 'M' || n == 'N') {
			mg_.draw_image_P(x, y, &font5x5_[(n - 'M') * font5x5_block_], 5, 5); 
			return 5;
		} else if('O' <= n && n <= 'S') {
			mg_.draw_image_P(x, y, &font4x5_[(n - 'O' + 12) * font4x5_block_], 4, 5); 
			return 4;
		} else if(n == 'T') {
			mg_.draw_image_P(x, y, &font5x5_[(n - 'T' + 2) * font5x5_block_], 5, 5); 
			return 5;
		} else if('U' == n) {
			mg_.draw_image_P(x, y, &font4x5_[(n - 'U' + 17) * font4x5_block_], 4, 5); 
			return 4;
		} else if('V' <= n && n <= 'Y') {
			mg_.draw_image_P(x, y, &font5x5_[(n - 'V' + 3) * font5x5_block_], 5, 5); 
			return 5;
		} else if('Z' == n) {
			mg_.draw_image_P(x, y, &font4x5_[(n - 'Z' + 18) * font4x5_block_], 4, 5); 
			return 4;
		}
		return 0;
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
