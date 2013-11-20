//=====================================================================//
/*!	@file
	@brief	テトリス・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "tetris.hpp"
#include "menu.hpp"

namespace app {
	static uint8_t v_ = 91;
	static uint8_t m_ = 123;

	static void randmize_(uint8_t v, uint8_t m)
	{
		v_ = v;
		m_ = m;
	}

	static uint8_t rand_()
	{
		v_ += v_ << 2;
		++v_;
		uint8_t n = 0;
		if(m_ & 0x02) n = 1;
		if(m_ & 0x40) n ^= 1;
		m_ += m_;
		if(n == 0) ++m_;
		return v_ ^ m_;
	}


	bool tetris::clip_x_(const position& pos, const block& bck)
	{
		for(uint8_t i = 0; i < 4; ++i) {
			char x = bck.poss[i].x + pos.x;
			if(x < 0 || x >= 10) return true;
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
			si = -1; co = 0;
			break;
		case 2:	// 180
			si = 0; co = -1;
			break;
		case 3:	// 270
			si = 1; co = 0;
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
			for(char x = 0; x < 10; ++x) {
				if(bitmap_.get(position(x, y))) {
					task_.at_monograph().point_set(ofsx + x, y); 
				}
			}
		}
	}

	// 消去ラインの確認
	char tetris::line_up_map_(char sy) const
	{
		for(char y = sy; y >= 0; --y) {
			if(bitmap_.get_line(y) == 0x3ff) {
				return y;
			}
		}
		return -1;
	}


	void tetris::task_game_()
	{
		const system::switch_input& swi = task_.at_switch();
		position p = block_pos_;
		if(swi.get_positive() & system::switch_input::bits::LEFT_UP) {
			--p.x ;
		}
		if(swi.get_positive() & system::switch_input::bits::RIGHT_UP) {
			++p.x;
		}

		char an = angle_;
		if(swi.get_positive() & system::switch_input::bits::RIGHT_DOWN) {
			++an;
			an &= 3;
		}

		bool bd = false;
		if(swi.get_level() & system::switch_input::bits::LEFT_DOWN) {
			bd = true;
		}

		// 消去ブロック
		char y = line_up_map_();
		if(y >= 0) {
			count_ = 60;
			mode_ = mode::blink;
			return;
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

		block bk;
		// 位置の左右クリップ
		rotate_(blocks_[block_idx_], angle_, bk);
		if(clip_x_(p, bk) || scan_map_(p, bk)) {
			p.x = block_pos_.x;
		} else {
			if(block_pos_.x != p.x) {
				task_.at_music().request(sound::music::id::tetris_move, 1);
			}
			block_pos_.x = p.x;
		}

		// 回転の左右クリップ
		rotate_(blocks_[block_idx_], an, bk);
		if(clip_x_(p, bk)) {
			bk = blocks_[block_idx_];
		} else {
			if(angle_ != an) {
				task_.at_music().request(sound::music::id::tetris_rot, 1);
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
			score_ += score::block_fall;
			if(bd) score_ += score::block_fall_quick;
			task_.at_music().request(sound::music::id::tetris_fall, 1);
			v_pos_ = 0;
			block_idx_ = rand_() % tetris_blocks_;
			v_spd_ += 1;
			if(v_spd_ >= 1024) v_spd_ = 1024;
			block_pos_.x = 5;	// 初期位置
			block_pos_.y = -2;
			angle_ = 0;
		} else {
			block_pos_.y = p.y;
		}

		// ゲーム終了判定
		if(bitmap_.get_line(0)) {
			mode_ = mode::over;
			count_ = 120 + 60;
		}

		// フレームの描画
		task_.at_monograph().line(offset_x_ - 1, 0, offset_x_ - 1, 15, 1);
		task_.at_monograph().line(offset_x_ + tetris_width_, 0,
			offset_x_ + tetris_width_, 15, 1);

		// 積みブロックの描画
		render_block_(offset_x_);

		// 落下ブロックの描画
		if(!bend) {
			position pp(p.x + offset_x_, p.y);
			draw_block_(pp, bk);
		}
	}


	void tetris::task_blink_()
	{
		char y = 15;
		char tmp[4];
		uint8_t dc = 0;
		while(1) {
			y = line_up_map_(y);
			if(y >= 0) {
				tmp[dc] = y;
				++dc;
				if(dc >= 4) break;
				--y;
			} else {
				break;
			}
		}

		// フレームの描画
		task_.at_monograph().line(offset_x_ - 1, 0, offset_x_ - 1, 15, 1);
		task_.at_monograph().line(offset_x_ + tetris_width_, 0,
			offset_x_ + tetris_width_, 15, 1);

		// 積みブロックの描画
		render_block_(offset_x_);

		// 消去ラインアニメーション
		if(count_) {
			if((count_ % 10) > 4) {
				for(uint8_t i = 0; i < dc; ++i) {
					task_.at_monograph().line(offset_x_, tmp[i],
						offset_x_ + tetris_width_ - 1, tmp[i], 0);
				}
			}
			--count_;
		} else {
			for(uint8_t i = 0; i < dc; ++i) {
				bitmap_.erase_line(tmp[i]);
				score_ += score::line_erase;
			}
			mode_ = mode::game;
			task_.at_music().request(sound::music::id::tetris_erase, 1);
		}
	}


	void tetris::task_over_()
	{
		if(count_) {
			--count_;
		} else {
			mode_ = mode::score;
			count_ = 128;
		}			

		if(count_ > 60) {
			if((count_ % 30) >= 15) {
				return;
			}
		}
		// フレームの描画
		task_.at_monograph().line(offset_x_ - 1, 0, offset_x_ - 1, 15, 1);
		task_.at_monograph().line(offset_x_ + tetris_width_, 0,
			offset_x_ + tetris_width_, 15, 1);

		// 積みブロックの描画
		render_block_(offset_x_);

		if(count_ <= 60) {
			uint8_t n = (60 - count_) / 8;
			for(uint8_t h = 0; h < n; ++h) {
				task_.at_monograph().line(0, h, 15, h, 1);
				task_.at_monograph().line(0, 15 - h, 15, 15 - h, 1);
			}
		}
	}


	void tetris::task_score_()
	{
		uint16_t sc = score_;
		int8_t y = 5;
		if(sc >= 10000) {
			y = 2 + 7;
		}
		task_.at_draw().draw_3x5(13, y, sc % 10);
		sc /= 10;
		task_.at_draw().draw_3x5(9,  y, sc % 10);
		sc /= 10;
		task_.at_draw().draw_3x5(5,  y, sc % 10);
		sc /= 10;
		task_.at_draw().draw_3x5(1,  y, sc % 10);
		if(sc >= 10000) {
			y = 2;
			sc /= 10;
			task_.at_draw().draw_3x5(9,  y, sc % 10);
			sc /= 10;
			task_.at_draw().draw_3x5(5,  y, sc % 10);
		}

#if 0
		{
			randmize_((score_ % 10) * 67, (score_ / 10) * 27);
			for(uint8_t y = 0; y < 16; ++y) {
				for(uint8_t x = 0; x < 16; ++y) {
					if(count_ < (rand_() & 127)) {
						task_.at_monograph().point_reset(x, y);
					}
				}
			}
			if(count_) --count_;
		}
#endif

		const system::switch_input& swi = task_.at_switch();
		if(swi.get_positive()) {
			task_.start<menu>();
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void tetris::init()
	{
		// ブロックの定義(0)
		blocks_[0].poss[0] = position(-2,  0);	//
		blocks_[0].poss[1] = position(-1,  0);	//
		blocks_[0].poss[2] = position( 0,  0);	// [][]<>[]
		blocks_[0].poss[3] = position( 1,  0);	//
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
		block_pos_.x = 5;	/// 初期ブロック位置
		block_pos_.y = -2;
		bitmap_.clear();
		score_ = 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void tetris::service()
	{
		if(mode_ == mode::game) {
			task_game_();
		} else if(mode_ == mode::blink) {
			task_blink_();
		} else if(mode_ == mode::over) {
			task_over_();
		} else if(mode_ == mode::score) {
			task_score_();
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void tetris::destroy()
	{
	}

}

