#pragma once
//=====================================================================//
/*!	@file
	@brief	タイマー関係（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "task.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	キッチン・タイマー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class timer : public i_task {
		task&	task_;

		struct mode {
			enum type {
				display,		///< 時間日付表示
				setting_y,		///< 年設定
				setting_md,		///< 月日設定
				setting_t,		///< 時間設定
				ret_menu,		///< メニューに戻る
			};
		};

		mode::type	mode_;

		int32_t		speed_;
		int32_t		pos_;

		int8_t		page__;
		int8_t		page_;

		time_t		time_;

		uint8_t		set_count_;
		uint8_t		frame_count_;
		int8_t		edit_pos_;

		tm			tm_;

		uint32_t get_time_();
		void draw_year_(int16_t ofs, uint16_t year);
		void draw_date_(int16_t ofs, const tm& t);
		void draw_time_(int16_t ofs, const tm& t);
		void display_();
		void setting_y_();
		void setting_md_();
		void setting_t_();
	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		timer(task& t) : task_(t), mode_(mode::display),
			speed_(0), pos_(0), page__(0), page_(0),
			time_(0),
			set_count_(0), frame_count_(0), edit_pos_(-1) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~timer() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void init();


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service();


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy();
	};
}
