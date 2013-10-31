#pragma once
//=====================================================================//
/*!	@file
	@breif	テトリス関係
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "task.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@breif	テトリス・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class tetris : public i_task {
		task&	task_;

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
		block		blocks_[6];

		class bitmap {
			uint8_t	bits_[16];
		public:
			void set(const position& pos) {
				if(pos.y < 0 || pos.y >= 16) return;
				if(pos.x < 0 || pos.x >= 8) return;
				bits_[static_cast<uint8_t>(pos.y)] |= 1 << pos.x;
			}
			void reset(const position& pos) { bits_[pos.y & 15] &= ~(1 << (pos.x & 7)); }
			void clear() { for(uint8_t i = 0; i < 16; ++i) bits_[i] = 0; }
			bool get(const position& pos) const {
				if(pos.y >= 16) return true;
				else if(pos.y < 0) return false;
				if(bits_[static_cast<uint8_t>(pos.y)] & (1 << (pos.x & 7))) return true;
				else return false;
			}
			uint8_t get_byte(char y) const {
				return bits_[y & 15];
			}
			void erase_line(char y) {
				if(y < 0 || y >= 16) return;
				for(uint8_t i = y; i > 1; --i) {
					bits_[i] = bits_[i - 1];
				}
				bits_[0] = 0;
			}
		};
		bitmap	bitmap_;

		struct flush_line {
			char	ys_[4];
			flush_line() {
				for(uint8_t i = 0; i < 4; ++i) ys_[i] = -1;
			}
			void set(char y) {
				for(uint8_t i = 0; i < 4; ++i) {
					if(ys_[i] < 0) {
						ys_[i] = y;
						return;
					}
				}
			}
		};
		flush_line	flush_line_;
		uint16_t	del_delay_;

		bool clip_x_(const position& pos, const block& bck);
		bool clip_y_(const position& pos, const block& bck);
		void draw_block_(const position& pos, const block& bck);
		void rotate_(const block& in, char angle, block& out);
		bool scan_map_(const position& pos, const block& in);
		void set_block_(const position& pos, const block& in);
		void render_block_(char ofsx) const;
		char line_up_map_() const;
		void line_fill_anime_();

	public:
		//-----------------------------------------------------------------//
		/*!
			@breif	コンストラクター
		*/
		//-----------------------------------------------------------------//
		tetris(task& t) : task_(t), block_pos_(0, 0), v_pos_(0), v_spd_(0),
			block_idx_(0), angle_(0),
			del_delay_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@breif	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~tetris() { }


		//-----------------------------------------------------------------//
		/*!
			@breif	初期化
		*/
		//-----------------------------------------------------------------//
		void init();


		//-----------------------------------------------------------------//
		/*!
			@breif	サービス
		*/
		//-----------------------------------------------------------------//
		void service();


		//-----------------------------------------------------------------//
		/*!
			@breif	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy();

	};

}
