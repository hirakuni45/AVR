#pragma once
//=====================================================================//
/*!	@file
	@brief	タスク関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "switch.hpp"
#include "monograph.hpp"
#include "psound.hpp"
#include "i_task.hpp"
#include "dm_draw.hpp"
#include "music.hpp"
#include "ds1370_io.hpp"
#include "eeprom.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	タスク・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct task {
		typedef device::ds1371_io	rtc;

		system::switch_input	swi_;
		device::psound			psound_;
		graphics::monograph		mng_;
		sound::music			music_;
		graphics::dm_draw		dm_;
		device::twi				twi_;
		rtc						rtc_;
		device::eeprom			eeprom_;

	private:
		i_task*		task_;
		i_task*		erase_;

		uint8_t		share_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		task() : swi_(), psound_(), mng_(), music_(psound_), dm_(mng_),
			twi_(), rtc_(twi_),
			task_(0), erase_(0), share_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~task() {
			delete task_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void init() {
			psound_.init();
			mng_.init();
			twi_.init();
			rtc_.init();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	共有データの登録
			@param[in]	a	共有データ
		*/
		//-----------------------------------------------------------------//
		void set_share(uint8_t a) { share_ = a; }


		//-----------------------------------------------------------------//
		/*!
			@brief	共有データの取得
			@return 共有データ
		*/
		//-----------------------------------------------------------------//
		uint8_t get_share() const { return share_; }
 

		//-----------------------------------------------------------------//
		/*!
			@brief	タスクの起動
		*/
		//-----------------------------------------------------------------//
		template <class TASK>
		void start() {
			erase_ = task_;
			task_ = new TASK(*this);
			task_->init();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service() {
			psound_.service();

			if(task_) task_->service();

			if(erase_) {
				erase_->destroy();
				delete erase_;
				erase_ = 0;
			}
		}
	};
}
