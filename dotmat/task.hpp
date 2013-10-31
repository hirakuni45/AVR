#pragma once
//=====================================================================//
/*!	@file
	@breif	タスク関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "switch.hpp"
#include "monograph.hpp"
#include "psound.hpp"
#include "i_task.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@breif	タスク・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class task {
		system::switch_input	swi_;
		device::psound			psound_;
		graphics::monograph		mng_;

		i_task*		task_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@breif	コンストラクター
		*/
		//-----------------------------------------------------------------//
		task() : task_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@breif	デストラクター
		*/
		//-----------------------------------------------------------------//
		~task() {
			delete task_;
		}


		//-----------------------------------------------------------------//
		/*!
			@breif	初期化
		*/
		//-----------------------------------------------------------------//
		void init() {
			psound_.init();
			mng_.init();
		}


		//-----------------------------------------------------------------//
		/*!
			@breif	スイッチの参照
		*/
		//-----------------------------------------------------------------//
		system::switch_input& at_switch() { return swi_; }		


		//-----------------------------------------------------------------//
		/*!
			@breif	パルス・サウンドの参照
		*/
		//-----------------------------------------------------------------//
		device::psound& at_psound() { return psound_; }		


		//-----------------------------------------------------------------//
		/*!
			@breif	グラフィックスの参照
		*/
		//-----------------------------------------------------------------//
		graphics::monograph& at_monograph() { return mng_; }


		//-----------------------------------------------------------------//
		/*!
			@breif	タスクの起動
		*/
		//-----------------------------------------------------------------//
		template <class TASK>
		void start() {
			if(task_) task_->destroy();
			delete task_;
			task_ = new TASK(*this);
			task_->init();
		}


		//-----------------------------------------------------------------//
		/*!
			@breif	サービス
		*/
		//-----------------------------------------------------------------//
		void service() {
			psound_.service();
			if(task_) task_->service();
		}
	};
}
