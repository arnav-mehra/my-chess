#pragma once

#include "../Move.hpp"
#include "../../board/Board.hpp"
#include "../KillerTable.hpp"

U16 Move::get_move_score(Piece pc, Piece capt, U16 depth) {
    constexpr U16 Q_PROMO_ADJ = 0b11ULL << 10;
    constexpr U16 CAPTURE_ADJ = 0b10ULL << 10;
    constexpr U16 QUIET_ADJ   = 0b01ULL << 10;
    constexpr U16 CASTLE_ADJ  = 0b00ULL << 10;

    constexpr U32 vals[16] = { 
        10, 26, 33, 50, 90, 200,
        10, 26, 33, 50, 90, 200
    };

    Flag flag = this->get_flag();

    bool is_q_promo = flag == Flag::QUEEN_PROMO;
    bool is_castle  = flag == Flag::CASTLE;
    bool is_capture = (capt != Piece::NA) & (flag == Flag::REGULAR);
    bool is_quiet   = (capt == Piece::NA) & (flag == Flag::REGULAR);

    U16 q_promo_bonus = Q_PROMO_ADJ;
    U16 castle_bonus  = 0;
    U16 capture_bonus = CAPTURE_ADJ + vals[(int)capt] - vals[(int)pc];
    U16 quiet_bonus   = (
        QUIET_ADJ
        + PeSTOs::mg_table[(int)pc][this->get_to()]
        - PeSTOs::mg_table[(int)pc][this->get_from()]
    );

    return (
          capture_bonus * is_capture
        + q_promo_bonus * is_q_promo
        +   quiet_bonus * is_quiet
        +  castle_bonus * is_castle
    );
}

void Move::set_score_capt(void* b_ptr, U16 depth) { 
    Board& b = *((Board*) b_ptr);

    Piece pc   = b.get_board(this->get_from());
    Piece capt = b.get_board(this->get_to());
    this->data |= (U32)capt << 16;

    U16 score  = this->get_move_score(pc, capt, depth);
    this->data |= (U32)score << 20;
}

void MoveList::fill_moves(void* b, U16 depth) {
    for (int i = 0; i < cnt; i++) {
        move_list[i].set_score_capt(b, depth);
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