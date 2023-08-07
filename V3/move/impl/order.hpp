#pragma once

#include "../Move.hpp"
#include "../../board/Board.hpp"

U32 Move::get_move_score(Flag flag, Piece pc, Piece capt) {
    const U32 QIT_MIN = 6;
    const U32 SPC_MIN = 12;
    const U32 CAP_MIN = 15;

    const U32 vals[16] = { 
        4, 26, 33, 50, 80, 200,
        4, 26, 33, 50, 80, 200
    };
    const U32 ords[16] = {
        1, 2, 3, 4, 5, 0,
        1, 2, 3, 4, 5, 0
    };

    const U32 CAP_ADJ = vals[(int)Piece::WHITE_KING] - vals[(int)Piece::WHITE_PAWN] + CAP_MIN;

    switch (flag) {
        // most likely to not be the best move (queen promo overshadows)
        case Flag::BISHOP_PROMO:         return 0;
        case Flag::ROOK_PROMO:           return 1;
        case Flag::KNIGHT_PROMO:         return 2;

        // baseline move
        // case Flag::QUIET:        return CAP_ADJ + vals[(int)capt] - vals[(int)pc];

        // normally slightly better than pure quiets
        case Flag::CASTLE: return SPC_MIN + 1;

        // captures/promos are very likely to be the best moves. 
        case Flag::REGULAR:             return CAP_ADJ + vals[(int)capt] - vals[(int)pc];
        case Flag::EN_PASSANT:          return CAP_ADJ + vals[(int)Piece::WHITE_PAWN]  - vals[(int)Piece::WHITE_PAWN];
        case Flag::QUEEN_PROMO:         return CAP_ADJ + vals[(int)Piece::WHITE_QUEEN] - vals[(int)Piece::WHITE_PAWN];
    }
    return 0;
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