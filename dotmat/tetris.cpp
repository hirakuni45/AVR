//=====================================================================//
/*!	@file
	@breif	テトリス・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "tetris.hpp"

namespace app {

	static uint8_t rand_()
	{
		static uint8_t v = 91;
		v += v << 2;
		++v;
		static uint8_t m = 123;
		uint8_t n = 0;
		if(m & 0x02) n = 1;
		if(m & 0x40) n ^= 1;
		m += m;
		if(n == 0) ++m;
		return v ^ m;
	}


	bool tetris::clip_x_(const position& pos, const block& bck)
	{
		for(uint8_t i = 0; i < 4; ++i) {
			char x = bck.poss[i].x + pos.x;
			if(x < 0 || x >= 8) return true;
		}
		return false;
	}


	bool tetris::clip_y_(const position& pos, const block& bck)
	{
		for(uint8_t i = 0; i < 4; ++i) {
			char y = bck.poss[i].y + pos.y;
			if(y >= 16) return true;
		}
		return false;
	}


	void tetris::draw_block_(const position& pos, const block& bck)
	{
		for(unsigned char i = 0; i < 4; ++i) {
			task_.at_monograph().point_set(pos.x + bck.poss[i].x, pos.y + bck.poss[i].y); 
		}
	}


	void tetris::rotate_(const block& in, char angle, block& out)
	{
		char si;
		char co;
		switch(angle) {
		case 0:	// 0
		default:
			si = 0; co = 1;
			break;
		case 1:	// 90
			si = 1; co = 0;
			break;
		case 2:	// 180
			si = 0; co = -1;
			break;
		case 3:	// 270
			si = -1; co = 0;
			break;
		}
		for(unsigned char i = 0; i < 4; ++i) {
			out.poss[i].x = in.poss[i].x * co + in.poss[i].y * si;
			out.poss[i].y = in.poss[i].y * co - in.poss[i].x * si;
		}
	}

	bool tetris::scan_map_(const position& pos, const block& in)
	{
		for(unsigned char i = 0; i < 4; ++i) {
			if(bitmap_.get(pos + in.poss[i])) return true;
		}
		return false;
	}

	void tetris::set_block_(const position& pos, const block& in)
	{
		for(unsigned char i = 0; i < 4; ++i) {
			bitmap_.set(pos + in.poss[i]);
		}
	}

	void tetris::render_block_(char ofsx) const
	{
		for(char y = 0; y < 16; ++y) {
			for(char x = 0; x < 8; ++x) {
				if(bitmap_.get(position(x, y))) {
					task_.at_monograph().point_set(ofsx + x, y); 
				}
			}
		}
	}

	// 消去ラインの確認
	char tetris::line_up_map_() const
	{
		for(char y = 15; y >= 0; --y) {
			if(bitmap_.get_byte(y) == 0xff) {
				return y;
			}
		}
		return -1;
	}


	//-----------------------------------------------------------------//
	/*!
		@breif	初期化
	*/
	//-----------------------------------------------------------------//
	void tetris::init()
	{
		// ブロックの定義(0)
		blocks_[0].poss[0] = position( 0, -1);	// []
		blocks_[0].poss[1] = position( 0, -2);	// []
		blocks_[0].poss[2] = position( 0,  0);	// <>
		blocks_[0].poss[3] = position( 0,  1);	// []
		// ブロックの定義(1)
		blocks_[1].poss[0] = position(-1, -1);	//
		blocks_[1].poss[1] = position(-1,  0);	// 
		blocks_[1].poss[2] = position( 0,  0);	// []<>[]
		blocks_[1].poss[3] = position( 1,  0);	// []
		// ブロックの定義(2)
		blocks_[2].poss[0] = position(-1,  0);	// 
		blocks_[2].poss[1] = position( 0,  0);	// 
		blocks_[2].poss[2] = position( 1,  0);	// []<>[]
		blocks_[2].poss[3] = position( 1, -1);	//     []
		// ブロックの定義(3)
		blocks_[3].poss[0] = position(-1,  0);	// 
		blocks_[3].poss[1] = position( 0,  0);	// []<>[]
		blocks_[3].poss[2] = position( 0, -1);	//   []
		blocks_[3].poss[3] = position( 1,  0);	// 
		// ブロックの定義(4)
		blocks_[4].poss[0] = position(-1,  0);	// 
		blocks_[4].poss[1] = position( 0,  0);	// []<>
		blocks_[4].poss[2] = position( 0, -1);	//   [][]
		blocks_[4].poss[3] = position( 1, -1);	// 
		// ブロックの定義(5)
		blocks_[5].poss[0] = position(-1, -1);	// 
		blocks_[5].poss[1] = position( 0, -1);	// 
		blocks_[5].poss[2] = position( 0,  0);	//   <>[]
		blocks_[5].poss[3] = position( 1,  0);	// [][]
		// ブロックの定義(6)
		blocks_[6].poss[0] = position(-1, -1);	// 
		blocks_[6].poss[1] = position( 0, -1);	// 
		blocks_[6].poss[2] = position( 0,  0);	// []<>
		blocks_[6].poss[3] = position(-1,  0);	// [][]

		v_spd_ = 128;
		v_pos_ = 0;
		block_pos_.x = 4;
		block_pos_.y = -2;
		bitmap_.clear();
	}


	//-----------------------------------------------------------------//
	/*!
		@breif	サービス
		@param[in]	swi	スイッチ入力
	*/
	//-----------------------------------------------------------------//
	void tetris::service()
	{
		const system::switch_input& swi = task_.at_switch();
		char ofsx = 4;
		char width = 8;
		position p = block_pos_;
		if(swi.get_positive() & system::switch_input::LEFT_UP) {
			--p.x ;
		}
		if(swi.get_positive() & system::switch_input::RIGHT_UP) {
			++p.x;
		}

		char an = angle_;
		if(swi.get_positive() & system::switch_input::RIGHT_DOWN) {
			++an;
			an &= 3;
		}

		bool bd = false;
		if(swi.get_level() & system::switch_input::LEFT_DOWN) {
			bd = true;
		}

		// 消去ラインアニメーション、自由落下
		if(del_delay_) {
			--del_delay_;
//			bitmap_.erase_line(del_lines_.get());
		} else {
			char y = line_up_map_();
			if(y >= 0) {
				bitmap_.erase_line(y);
			}

			if(bd) {
				v_pos_ += 2048;
			} else {
				v_pos_ += v_spd_;
			}
			if(v_pos_ >= 4096) {
				v_pos_ = 0;
				++p.y;
			}
		}

		block bk;
		// 位置の左右クリップ
		rotate_(blocks_[block_idx_], angle_, bk);
		if(clip_x_(p, bk) || scan_map_(p, bk)) {
			p.x = block_pos_.x;
		} else {
			if(block_pos_.x != p.x) {
				task_.at_psound().request(40, 2);
			}
			block_pos_.x = p.x;
		}

		// 回転の左右クリップ
		rotate_(blocks_[block_idx_], an, bk);
		if(clip_x_(p, bk)) {
			bk = blocks_[block_idx_];
		} else {
			if(angle_ != an) {
				task_.at_psound().request(45, 2);
			}
			angle_ = an;
		}

		bool bend = false;

		// 底？
		if(clip_y_(p, bk)) {
			p.y = block_pos_.y;
			set_block_(p, bk);
			bend = true;
		}

		// 積みブロック検査
		if(scan_map_(p, bk)) {
			p.y = block_pos_.y;
			set_block_(p, bk);
			bend = true;
		}

		// 新規ブロック発生
		if(bend) {
			v_pos_ = 0;
			block_idx_ = rand_() % tetris_blocks_;
			v_spd_ += 1;
			if(v_spd_ >= 1024) v_spd_ = 1024;
			block_pos_.x = 4;
			block_pos_.y = -2;
			angle_ = 0;
		} else {
			block_pos_.y = p.y;
		}

		// ゲーム終了判定
		if(bitmap_.get_byte(0)) {
			init();
			return;
		}

		// フレームの描画
		task_.at_monograph().line(ofsx - 1, 0, ofsx - 1, 15, 1);
		task_.at_monograph().line(ofsx + width, 0, ofsx + width, 15, 1);

		// 積みブロックの描画
		render_block_(ofsx);

		if(!bend) {
			position pp(p.x + ofsx, p.y);
			draw_block_(pp, bk);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@breif	廃棄
	*/
	//-----------------------------------------------------------------//
	void tetris::destroy()
	{
	}

}

