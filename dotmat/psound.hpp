#pragma once
//=====================================================================//
/*!	@file
	@brief	パルス・サウンド関係（ヘッダー）@n
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
		@brief	パルス・サウンド・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class psound {
	public:

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	プレイヤー・コマンド定義 @n
					８オクターブ１２平均音階率 
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct sound_key {
			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
			/*!
				@brief	タイプ
			*/
			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
			enum type {
				C  = 0,		///< C
				Cs = 1,		///< C#
				Db = 1,		///< Db
				D  = 2,		///< D
				Ds = 3,		///< D#
				Eb = 3,		///< Eb
				E  = 4,		///< E
				F  = 5,		///< F
				Fs = 6,		///< Fs
				Gb = 6,		///< Gb
				G  = 7,		///< G
				Gs = 8,		///< G#
				Ab = 8,		///< Ab
				A  = 9,		///< A
				As = 10,	///< A#
				Bb = 10,	///< Bb
				B  = 11,	///< B

				Q  = 96,	///< 休符
				TEMPO,		///< テンポ設定

				VOLUME,		///< ボリューム設定

				FOR,		///< ループ開始
				BEFORE,		///< ループ終端

				TR,			///< トランスポーズ
				TR_,		///< トランスポーズ差分

				COLOR0,		// 音色０
				COLOR1,		// 音色１
				COLOR2,		// 音色２
				COLOR3,		// 音色３
				COLOR4,		// 音色４
				COLOR5,		// 音色５
				COLOR6,		// 音色６
				COLOR7,		// 音色７
				COLOR8,		// 音色８
				COLOR9,		// 音色９

				VIBR0,		// ビブラート０（無し）
				VIBR1,		// ビブラート１
				VIBR2,		// ビブラート２
				VIBR3,		// ビブラート３
				VIBR4,		// ビブラート４
				VIBR5,		// ビブラート５
				VIBR6,		// ビブラート６
				VIBR7,		// ビブラート７

				END,		///< 終曲
			};
		};

	private:
		struct music_slot {
			bool	enable_;
			bool	index_trg_;
			uint8_t	index_reg_;
			uint8_t	tempo_master_;
			uint8_t	tempo_;
			uint8_t	index_;
			uint8_t	tr_;
			uint8_t	length_top_;
			uint8_t	length_;
			const prog_uint8_t*	music_player_;

			uint8_t	volume_reg_;
			uint8_t	volume_master_;
			int8_t	fader_speed_;
			uint8_t	envelope_;
			uint8_t	envelope_cmp_;
			uint8_t	envelope_down_;

			uint8_t	stack_[8];
			uint8_t	stack_pos_;

			music_slot() : enable_(false), index_trg_(false),
				index_reg_(sound_key::Q),
				tempo_master_(0), tempo_(255), index_(0), tr_(0),
				length_top_(0), length_(0),
				music_player_(0),
				volume_reg_(0), volume_master_(0), fader_speed_(0),
				envelope_(0), envelope_cmp_(0), envelope_down_(0),
				stack_pos_(0)
			{ }

			void push(uint8_t val) {
				stack_[stack_pos_ & 7] = val;
				++stack_pos_;
			}

			void push16(uint16_t val) {
				push(val & 0xff);
				push(val >> 8);
			}

			uint8_t pop() {
				--stack_pos_;
				return stack_[stack_pos_];
			}

			uint16_t pop16() {
				uint16_t val = pop();
				val <<= 8;
				val |= pop();
				return val;
			}
		};
		music_slot	music_slot_[2];

		void disable_(uint8_t chanel);
		void service_chanel_(music_slot& ms);
	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		psound() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	パルスサウンド初期化
		*/
		//-----------------------------------------------------------------//
		void init();


		//-----------------------------------------------------------------//
		/*!
			@brief	パルスサウンドサービス(1/100 で呼ぶ）
		*/
		//-----------------------------------------------------------------//
		void service();


		//-----------------------------------------------------------------//
		/*!
			@brief	パルスサウンドリクエスト（直接）
			@param[in]	index	音階
			@param[in]	length	音長
			@param[in]	chanel	チャンネル
		*/
		//-----------------------------------------------------------------//
		void request(uint8_t index, uint8_t length, uint8_t chanel = 0);


		//-----------------------------------------------------------------//
		/*!
			@brief	パルスサウンド演奏
			@param[in]	music	音階と音長の組をテーブルにしたもの
			@param[in]	chanel	チャンネル
		*/
		//-----------------------------------------------------------------//
		void play_P(const prog_uint8_t *music, uint8_t chanel = 0);


		//-----------------------------------------------------------------//
		/*!
			@brief	演奏中か取得
			@param[in]	chanel	チャンネル
			@return 演奏中なら「true」
		*/
		//-----------------------------------------------------------------//
		bool get_play_state(uint8_t chanel = 0) const {
			return music_slot_[chanel].music_player_ != 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	コンペアマッチに直接値を設定する。
			@param[in]	divide	ディバイダーの設定値
			@param[in]	cmpv	コンペアマッチに設定する値
		*/
		//-----------------------------------------------------------------//
		void direct(uint8_t divide, uint16_t cmpv);


		//-----------------------------------------------------------------//
		/*!
			@brief	トグル出力の周波数を設定する
			@param[in]	freq	出力周波数(Hz)下位４ビット小数点
			@return 「true」なら正常終了
		*/
		//-----------------------------------------------------------------//
		bool freq(uint32_t freq);


		//-----------------------------------------------------------------//
		/*!
			@brief	プリスケーラーの分周比を取得
			@return 分周比
		*/
		//-----------------------------------------------------------------//
		int16_t get_divider() const;

#ifdef PSOUND_VOLUME_ENABLE
		//-----------------------------------------------------------------//
		/*!
			@brief	マスター・ボリュームの設定
			@param[in]	vol	ボリューム
			@param[in]	chanel	チャネル
		*/
		//-----------------------------------------------------------------//
		void set_volume(uint8_t vol, uint8_t chanel = 0) {
			music_slot_[chanel].volume_master_ = vol;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フェーダーの設定
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
