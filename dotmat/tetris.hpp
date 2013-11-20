#pragma once
//=====================================================================//
/*!	@file
	@brief	テトリス関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "task.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	テトリス・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class tetris : public i_task {
		task&	task_;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	各スコア
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct score {
			enum type {
				block_fall = 1,			///< ブロック落下
				line_erase = 20,		///< ライン消去
				block_fall_quick = 1,	///< 高速ブロック落下
			};
		};

		///< テトリスブロック種
		static const uint8_t 	tetris_blocks_ = 7;
		static const char offset_x_ = 3;
		static const char tetris_width_ = 10;

		struct position {
			char	x;
			char	y;
			position() { }
			position(char x_, char y_) : x(x_), y(y_) { }
			position operator + (const position& p) const {
				position pos(x + p.x, y + p.y);
				return pos;
			} 
		};

		struct block {
			position	poss[4];
		};

		position	block_pos_;
		short		v_pos_;
		short		v_spd_;
		uint8_t		block_idx_;
		char		angle_;
		block		blocks_[tetris_blocks_];

		class bitmap {
			uint16_t	bits_[16];
		public:
			void set(const position& pos) {
				if(pos.y < 0 || pos.y >= 16) return;
				if(pos.x < 0 || pos.x >= 10) return;
				bits_[static_cast<uint8_t>(pos.y)] |= 1 << pos.x;
			}
			void reset(const position& pos) { bits_[pos.y & 15] &= ~(1 << (pos.x & 15)); }
			void clear() { for(uint8_t i = 0; i < 16; ++i) bits_[i] = 0; }
			bool get(const position& pos) const {
				if(pos.y >= 16) return true;
				else if(pos.y < 0) return false;
				if(bits_[static_cast<uint8_t>(pos.y)] & (1 << (pos.x & 15))) return true;
				else return false;
			}
			uint16_t get_line(char y) const {
				return bits_[y & 15];
			}
			void erase_line(char y) {
				if(y < 0 || y >= 16) return;
				uint8_t i = y;
				while(i > 0) {
					bits_[i] = bits_[i - 1];
					--i;
				}
				bits_[0] = 0;
			}
		};
		bitmap	bitmap_;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ゲーム・モード
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct mode {
			enum type {
				game,	///< 通常ゲーム
				blink,	///< 消去ブロック点滅
				over,	///< ゲーム終了
				score,	///< スコア表示
			};
		};
		mode::type	mode_;
		uint8_t		count_;

		uint16_t	score_;

		bool clip_x_(const position& pos, const block& bck);
		bool clip_y_(const position& pos, const block& bck);
		void draw_block_(const position& pos, const block& bck);
		void rotate_(const block& in, char angle, block& out);
		bool scan_map_(const position& pos, const block& in);
		void set_block_(const position& pos, const block& in);
		void render_block_(char ofsx) const;
		char line_up_map_(char sy = 15) const;
		void line_fill_anime_();

		void task_game_();
		void task_blink_();
		void task_over_();
		void task_score_();
	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		tetris(task& t) : task_(t), block_pos_(0, 0), v_pos_(0), v_spd_(0),
			block_idx_(0), angle_(0),
			mode_(mode::game), count_(0), score_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~tetris() { }


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
