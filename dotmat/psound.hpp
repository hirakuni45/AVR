#pragma once
//=====================================================================//
/*!	@file
	@breif	パルス・サウンド関係（ヘッダー）@n
			マスターボリューム制御の場合、外部ＲＣ時定数 @n
			R: 2200, C: 0.1uF
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <stdint.h>

// Volume 制御を行う場合有効にする
#define PSOUND_VOLUME_ENABLE

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@breif	パルス・サウンド・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class psound {
	public:

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@breif	プレイヤー・コマンド定義 @n
					８オクターブ１２平均音階率 @n
					B7 は制御ビット、立てると、次のバイトが制御コード
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct sound_key {
			enum type {
				C  = 0,		// C
				Cs = 1,		// C#
				Db = 1,		// Db
				D  = 2,		// D
				Ds = 3,		// D#
				Eb = 3,		// Eb
				E  = 4,		// E
				F  = 5,		// F
				Fs = 6,		// Fs
				Gb = 6,		// Gb
				G  = 7,		// G
				Gs = 8,		// G#
				Ab = 8,		// Ab
				A  = 9,		// A
				As = 10,	// A#
				Bb = 10,	// Bb
				B  = 11,	// B

				Q  = 96,	// 休符
				TEMPO,		// テンポ設定
				END,		// 終曲
			};
		};

	private:
		struct music_slot {
			bool	index_trg_;
			uint8_t	index_reg_;
			uint8_t	tempo_master_;
			uint8_t	tempo_;
			uint8_t	index_;
			uint8_t	length_;
			bool	enable_;
			const prog_uint8_t*	music_player_;

#ifdef PSOUND_VOLUME_ENABLE
			uint8_t	volume_reg_;
			uint8_t	volume_master_;
			int8_t	fader_speed_;
			uint8_t	envelope_;
#endif
			music_slot() : index_trg_(false), index_reg_(sound_key::Q),
				tempo_master_(0), tempo_(180), index_(0), length_(0),
				enable_(false), music_player_(0)
#ifdef PSOUND_VOLUME_ENABLE
				, volume_reg_(0),
				volume_master_(0), fader_speed_(0), envelope_(0)
#endif
			{ }
		};
		music_slot	music_slot_[2];

		void disable_(uint8_t chanel);
		void service_chanel_(music_slot& ms);
	public:
		//-----------------------------------------------------------------//
		/*!
			@breif	コンストラクター
		*/
		//-----------------------------------------------------------------//
		psound() { }


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
			@param[in]	chanel	チャンネル
		*/
		//-----------------------------------------------------------------//
		void request(uint8_t index, uint8_t length, uint8_t chanel = 0);


		//-----------------------------------------------------------------//
		/*!
			@breif	パルスサウンド演奏
			@param[in]	music	音階と音長の組をテーブルにしたもの
			@param[in]	chanel	チャンネル
		*/
		//-----------------------------------------------------------------//
		void play_P(const prog_uint8_t *music, uint8_t chanel = 0);


		//-----------------------------------------------------------------//
		/*!
			@breif	演奏中か取得
			@param[in]	chanel	チャンネル
			@return 演奏中なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_play_state(uint8_t chanel = 0) const {
			return music_slot_[chanel].music_player_ != 0;
		}


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

#ifdef PSOUND_VOLUME_ENABLE
		//-----------------------------------------------------------------//
		/*!
			@breif	マスター・ボリュームの設定
			@param[in]	vol	ボリューム
			@param[in]	chanel	チャネル
		*/
		//-----------------------------------------------------------------//
		void set_volume(uint8_t vol, uint8_t chanel = 0) {
			music_slot_[chanel].volume_master_ = vol;
		}


		//-----------------------------------------------------------------//
		/*!
			@breif	フェーダーの設定
			@param[in]	fader	フェーダー値
			@param[in]	chanel	チャネル
		*/
		//-----------------------------------------------------------------//
		void set_fader(int8_t fader, uint8_t chanel = 0) {
			music_slot_[chanel].fader_speed_ = fader;
		}
#endif
	};

}
