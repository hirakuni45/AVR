#pragma once
//=====================================================================//
/*!	@file
	@breif	パルス・サウンド関係（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <stdint.h>

namespace device {

	class psound {
	public:
		struct sound_key {
			enum type {
				C  = 0,
				Cs = 1,
				Db = 1,
				D  = 2,
				Ds = 3,
				Eb = 3,
				E  = 4,
				F  = 5,
				Fs = 6,
				Gb = 6,
				G  = 7,
				Gs = 8,
				Ab = 8,
				A  = 9,
				As = 10,
				Bb = 10,
				B  = 11,
			};
		};

	private:
		uint8_t	index_;
		uint8_t	length_;
		uint8_t count_;
		bool	enable_;
		const prog_uint8_t*	music_player_;

		void disable_();
	public:
		//-----------------------------------------------------------------//
		/*!
			@breif	コンストラクター
		*/
		//-----------------------------------------------------------------//
		psound() : index_(0), length_(0), count_(0), enable_(false),
			music_player_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@breif	パルスサウンド初期化
		*/
		//-----------------------------------------------------------------//
		void init();


		//-----------------------------------------------------------------//
		/*!
			@breif	パルスサウンドサービス(1/100 で呼ぶ）
		*/
		//-----------------------------------------------------------------//
		void service();


		//-----------------------------------------------------------------//
		/*!
			@breif	パルスサウンドリクエスト
			@param[in]	index	音階
			@param[in]	length	音長
		*/
		//-----------------------------------------------------------------//
		void request(uint8_t index, uint8_t length);


		//-----------------------------------------------------------------//
		/*!
			@breif	パルスサウンド演奏
			@param[in]	music	音階と音長の組をテーブルにしたもの
		*/
		//-----------------------------------------------------------------//
		void play_P(const prog_uint8_t *music);


		//-----------------------------------------------------------------//
		/*!
			@breif	コンペアマッチに直接値を設定する。
			@param[in]	divider	ディバイダーの設定値
			@param[in]	cmpv	コンペアマッチに設定する値
		*/
		//-----------------------------------------------------------------//
		void direct(uint8_t divide, uint16_t cmpv);


		//-----------------------------------------------------------------//
		/*!
			@breif	トグル出力の周波数を設定する
			@param[in]	freq	出力周波数(Hz)下位４ビット小数点
			@return 「true」なら正常終了
		*/
		//-----------------------------------------------------------------//
		bool freq(uint32_t freq);


		//-----------------------------------------------------------------//
		/*!
			@breif	プリスケーラーの分周比を取得
			@return 分周比
		*/
		//-----------------------------------------------------------------//
		int16_t get_divider() const;
	};

}
