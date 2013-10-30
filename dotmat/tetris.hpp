#pragma once
//=====================================================================//
/*!	@file
	@breif	テトリス・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "switch.hpp"
#include "monograph.hpp"

namespace game {

	//-----------------------------------------------------------------//
	/*!
		@breif	テトリス
	*/
	//-----------------------------------------------------------------//
	class tetris {

		graphics::monograph& mng_;

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
		unsigned char	block_idx_;
		char		angle_;
		block		blocks_[6];

		class bitmap {
			unsigned char	bits_[16];
		public:
			void set(const position& pos) {
				if(pos.y < 0 || pos.y >= 16) return;
				if(pos.x < 0 || pos.x >= 8) return;
				bits_[static_cast<unsigned char>(pos.y)] |= 1 << pos.x;
			}
			void reset(const position& pos) { bits_[pos.y & 15] &= ~(1 << (pos.x & 7)); }
			void clear() { for(unsigned char i = 0; i < 16; ++i) bits_[i] = 0; }
			bool get(const position& pos) const {
				if(pos.y >= 16) return true;
				else if(pos.y < 0) return false;
				if(bits_[static_cast<unsigned char>(pos.y)] & (1 << (pos.x & 7))) return true;
				else return false;
			}
			unsigned char get_byte(char y) const {
				return bits_[y & 15];
			}
			void erase(char y) {
				if(y < 0 || y >= 16) return;
				for(unsigned char i = y; i > 1; --i) {
					bits_[i] = bits_[i - 1];
				}
				bits_[0] = 0;
			}
		};
		bitmap	bitmap_;

		class fifo {
			unsigned char	put_;
			unsigned char	get_;
			char 			buff_[16];
		public:
			fifo() : put_(0), get_(0) { }
			void put(char y) { buff_[put_ & 15] = y; ++put_; }
			char get() { char ch = buff_[get_ & 15]; ++get_; return ch; }
			unsigned char length() const {
				if(put_ > get_) return put_ - get_;
				else return 256 + static_cast<unsigned short>(put_) - get_;
			}
		};
		fifo	fifo_;

		bool clip_x_(const position& pos, const block& bck);
		bool clip_y_(const position& pos, const block& bck);
		void draw_block_(const position& pos, const block& bck);
		void rotate_(const block& in, char angle, block& out);
		bool scan_map_(const position& pos, const block& in);
		void set_block_(const position& pos, const block& in);
		void render_block_(char ofsx) const;
		char line_up_map_() const;

	public:
		//-----------------------------------------------------------------//
		/*!
			@breif	コンストラクター
		*/
		//-----------------------------------------------------------------//
		tetris(graphics::monograph& mng) : mng_(mng), block_pos_(0, 0), v_pos_(0), v_spd_(0),
				   block_idx_(0), angle_(0) { }


		void init();


		//-----------------------------------------------------------------//
		/*!
			@breif	サービス
			@param[in]	swi	スイッチ入力
		*/
		//-----------------------------------------------------------------//
		void service(const system::switch_input& swi);


	};

}
