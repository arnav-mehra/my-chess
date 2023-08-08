#pragma once

#include "../Move.hpp"
#include "../../board/Board.hpp"

constexpr U32 QIT_MIN = 6;
constexpr U32 SPC_MIN = 12;
constexpr U32 CAP_MIN = 15;

constexpr U32 vals[16] = { 
    4, 26, 33, 50, 80, 200,
    4, 26, 33, 50, 80, 200
};
constexpr U32 ords[16] = {
    1, 2, 3, 4, 5, 0,
    1, 2, 3, 4, 5, 0
};

constexpr U32 CAP_ADJ = vals[(int)Piece::WHITE_KING] - vals[(int)Piece::WHITE_PAWN] + CAP_MIN;
constexpr U32 QUEEN_PROMO_SCORE = CAP_ADJ + vals[(int)Piece::WHITE_QUEEN] - vals[(int)Piece::WHITE_PAWN];

int get_flag_bonus(Flag fg) {
    switch (fg) {
        case Flag::BISHOP_PROMO: return 0;
        case Flag::ROOK_PROMO:   return 1;
        case Flag::KNIGHT_PROMO: return 2;
        case Flag::REGULAR:      return 3;
        case Flag::CASTLE:       return 4;
        case Flag::EN_PASSANT:   return 5;
        case Flag::QUEEN_PROMO:  return 6;
    }
    return 0;
}

U32 Move::get_move_score(Flag flag, Piece pc, Piece capt) {
    U32 cap_bonus   = (CAP_ADJ + vals[(int)capt] - vals[(int)pc]) * (capt != Piece::NA);
    U32 promo_bonus = QUEEN_PROMO_SCORE * (flag == Flag::QUEEN_PROMO);
    U32 flag_bonus  = get_flag_bonus(flag);
    return cap_bonus + flag_bonus + promo_bonus;
}

template<Flag flag>
U32 Move::get_move_score(Piece pc, Piece capt) {
    return get_move_score(flag, pc, capt);
}

void Move::set_score_capt(void* b_ptr) { 
    Board& b = *((Board*) b_ptr);

    Piece pc   = b.get_board(this->get_from());
    Piece capt = b.get_board(this->get_to());
    U32 score  = get_move_score(this->get_flag(), pc, capt);
    this->data |= score << 20;
    this->data |= (U32)capt << 16; 
}

void MoveList::fill_moves(void* b) {
    for (int i = 0; i < cnt; i++) {
        move_list[i].set_score_capt(b);
    }
}

void MoveList::sort() { // insertion sort
    for (int i = 1; i < cnt; i++) {
        Move subject = move_list[i]; // save value of subject
        int j;
        for (j = i - 1; j >= 0 && subject > move_list[j]; j--) {
            move_list[j + 1] = move_list[j];
        }
        move_list[j + 1] = subject; // insert subject
    }
}