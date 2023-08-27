#pragma once

#include "../Move.hpp"
#include "../../board/Board.hpp"
#include "../KillerTable.hpp"

// ORDERING: (Score = 11 bits)
//      1. TT-suggestion (PV-node, Hash Move) -> MAX_SCORE
//      2. Positive Captures and Promos       -> Victim - Aggressor (> 0) + CAPT_SCORE
//      3. Neutral Captures                   -> Victim - Aggressor (= 0) + CAPT_SCORE
//      4. Killers                            -> CAPT_SCORE - 1
//      5. Negative Captures                  -> Victim - Aggressor (< 0) + CAPT_SCORE
//      6. Quiets                             -> 0

// Calculating CAPT_SCORE:
//      MIN(Negative Captures) = 1
//      So, PC_VALS[PAWN] - PC_VALS[KING] + CAPT_SCORE = 1
//      CAPT_SCORE = PC_VALS[KING] - PC_VALS[PAWN] + 1

// PROMO FORMULA:
//      PROMO_ADJ = CAPTURE_BONUS + PC_VALS[PROMO_PC]

constexpr U32 PC_VALS[16] = { 
    7, 29, 30, 46, 90, 120,
    7, 29, 30, 46, 90, 120
};

constexpr U32 FLAG_VALS[7] = { 
    0, 0, 0, 29, 30, 46, 90
};

constexpr U32 MAX_SCORE = (0b1U << 11) - 1U;

constexpr U32 CAPT_SCORE = (
    PC_VALS[(int)Piece::WHITE_KING]
    - PC_VALS[(int)Piece::WHITE_PAWN]
    + 1U
);

U32 Move::get_move_score(Piece pc, Piece capt, Flag flag, U16 depth, Move& priority) {
    bool not_known = true;
    bool is_priority = (this->get_masked() == priority.get_masked());
    not_known &= !is_priority;
    bool is_killer = not_known && KillerTable::has_move(*this, depth);
    not_known &= !is_killer;
    bool is_promo = not_known & (flag >= Flag::KNIGHT_PROMO);
    not_known &= !is_promo;
    bool is_capture = not_known & (capt != Piece::NA);
    not_known &= !is_capture;
    bool is_quiet = not_known;

    U32 priority_bonus = MAX_SCORE;
    U32 killer_bonus   = CAPT_SCORE - 1U;
    U32 quiet_bonus    = 0U;
    U32 capture_bonus  = CAPT_SCORE + PC_VALS[(int)capt] - PC_VALS[(int)pc];
    U32 promo_bonus    = capture_bonus + FLAG_VALS[(int)flag];

    return (
          (is_priority * priority_bonus)
        | (is_killer   * killer_bonus)
        | (is_promo    * promo_bonus)
        | (is_capture  * capture_bonus)
        | (is_quiet    * quiet_bonus)
    );
}

U32 Move::get_capture_score(Piece pc, Piece capt, Flag flag, U16 depth, Move& priority) {
    bool not_known = true;
    bool is_priority = (this->get_masked() == priority.get_masked());
    not_known &= !is_priority;
    bool is_promo = not_known & (flag >= Flag::KNIGHT_PROMO);
    not_known &= !is_promo;
    bool is_capture = not_known;

    U32 priority_bonus = MAX_SCORE;
    U32 quiet_bonus    = 0U;
    U32 capture_bonus  = CAPT_SCORE + PC_VALS[(int)capt] - PC_VALS[(int)pc];
    U32 promo_bonus    = capture_bonus + FLAG_VALS[(int)flag];

    return (
          (is_priority * priority_bonus)
        | (is_promo    * promo_bonus)
        | (is_capture  * capture_bonus)
    );
}

U32 Move::get_quiet_score(Piece pc, Flag flag, U16 depth, Move& priority) {
    bool not_known = true;
    bool is_priority = (this->get_masked() == priority.get_masked());
    not_known &= !is_priority;
    bool is_killer = not_known & KillerTable::has_quiet(*this, depth);
    not_known &= !is_killer;
    bool is_promo = not_known & (flag >= Flag::KNIGHT_PROMO);
    not_known &= !is_promo;
    bool is_quiet = not_known;

    U32 priority_bonus = MAX_SCORE;
    U32 killer_bonus   = 1U;
    U32 quiet_bonus    = 0U;
    U32 promo_bonus    = CAPT_SCORE - PC_VALS[(int)Piece::WHITE_PAWN] + FLAG_VALS[(int)flag];

    return (
          (is_priority * priority_bonus)
        | (is_killer   * killer_bonus)
        | (is_promo    * promo_bonus)
        | (is_quiet    * quiet_bonus)
    );
}

template<class Color, Gen Gn>
void Move::set_score_capt(void* b_ptr, Move& priority, U16 depth) { 
    Board& b = *((Board*) b_ptr);

    Piece pc   = b.get_board(this->get_from());
    Flag  flag = this->get_flag();
    Piece capt = flag == Flag::EN_PASSANT ? (Piece)Color::OPP_PAWN
                                          : b.get_board(this->get_to());

    // set capture

    this->data |= (U32)capt << 16;

    // set reversability (for 3-fold repetition or 50-move rule)

    if constexpr (Gn != Gen::CAPTURES) {
        U32 is_reversable = (
            (flag == Flag::CASTLE) // castles are reversable (artifically)
            | (
                (flag == Flag::REGULAR) // non-pawn non-captures are reversable
                & (capt == (Piece)Piece::NA)
                & (pc != (Piece)Color::PAWN)
            )
        );
        this->data |= is_reversable << 20;
    }

    // set score

    if constexpr (Gn == Gen::CAPTURES) {
        U32 score = this->get_capture_score(pc, capt, flag, depth, priority);
        this->data |= score << 21;
    } else {
        U32 score = capt == Piece::NA
            ? this->get_quiet_score(pc, flag, depth, priority)
            : this->get_capture_score(pc, capt, flag, depth, priority);
        this->data |= score << 21;
    }
}

template<class Color, Gen Gn>
void MoveList::fill_moves(
    void* b,
    U16 depth,
    Move priority
) {
    for (int i = 0; i < cnt; i++) {
        move_list[i].set_score_capt<Color, Gn>(b, priority, depth);
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