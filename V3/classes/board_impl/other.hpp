#pragma once
#include "../Board.hpp"

template<class Color, class Castle>
bool Board::sliding_castle_checks() {
    U64 occ_wo_king = this->bitboards[(int)Piece::ALL]
                    & ~this->bitboards[(int)Color::KING];

    U64 rook_risks = 0ULL, bishop_risks = 0ULL;
    for (Square sq : Castle::NON_CHECKS) {
        rook_risks   |= KMAGICS::get_r_attacks(sq, occ_wo_king);
        bishop_risks |= KMAGICS::get_b_attacks(sq, occ_wo_king);
    }
    U64 queen_risks = rook_risks | bishop_risks;

    bool bishop_attackers = (bishop_risks & this->bitboards[Color::OPP_BISHOP]) != 0ULL;
    bool rook_attackers   = (rook_risks   & this->bitboards[Color::OPP_ROOK]  ) != 0ULL;
    bool queen_attackers  = (queen_risks  & this->bitboards[Color::OPP_QUEEN] ) != 0ULL;
    return bishop_attackers | rook_attackers | queen_attackers;
}

template<class Color>
U64 Board::get_checks() {
    U64 occ = this->bitboards[(int)Piece::ALL];
    Square sq = lsb(this->bitboards[(int)Color::KING]);

    U64 pawn_risks   = Color::PAWN_ATTACKS[sq];
    U64 knight_risks = MAPPED_MOVES::KNIGHT_MOVES[sq];
    U64 rook_risks   = KMAGICS::get_r_attacks(sq, occ);
    U64 bishop_risks = KMAGICS::get_b_attacks(sq, occ);
    U64 queen_risks  = rook_risks | bishop_risks;

    U64 pawn_attackers   = (pawn_risks   & this->bitboards[Color::OPP_PAWN]  ) != 0ULL;
    U64 knight_attackers = (knight_risks & this->bitboards[Color::OPP_KNIGHT]) != 0ULL;
    U64 bishop_attackers = (bishop_risks & this->bitboards[Color::OPP_BISHOP]) != 0ULL;
    U64 rook_attackers   = (rook_risks   & this->bitboards[Color::OPP_ROOK]  ) != 0ULL;
    U64 queen_attackers  = (queen_risks  & this->bitboards[Color::OPP_QUEEN] ) != 0ULL;
    
    return bishop_attackers | rook_attackers | queen_attackers
         | knight_attackers | pawn_attackers;
}

template<class Color>
U64 Board::get_pins() {
    Square sq = lsb(this->get_bitboard(Color::KING));
    U64 (&BITS_BETWEEN_KING)[64] = MAPPED_MOVES::BITS_BETWEEN[sq];

    U64 opp = get_bitboard(Color::OPP_ALL);
    U64 rook_risks = KMAGICS::get_r_attacks(sq, opp)
                   & (get_bitboard(Color::OPP_ROOK) | get_bitboard(Color::OPP_QUEEN));
    U64 bishop_risks = KMAGICS::get_b_attacks(sq, opp)
                     & (get_bitboard(Color::OPP_BISHOP) | get_bitboard(Color::OPP_QUEEN));
    U64 all_risks = rook_risks | bishop_risks;

    U64 pins = 0ULL;
    while (all_risks) {
        Square sq2 = pop_lsb(all_risks);
        U64 potential_pins = BITS_BETWEEN_KING[sq2] & get_bitboard(Color::ALL);
        if (pop_count(potential_pins) == 1) {
            pins |= potential_pins;
        }
    }
    return pins;
}

template<class Color>
U64 Board::get_check_blocks(U64 checks) {
    Square king = lsb(this->get_bitboard(Color::KING));
    Square check = lsb(checks);
    return MAPPED_MOVES::BITS_BETWEEN[king][check] | (1ULL << check);
}

template<class Color>
U64 Board::get_opp_attacks() {
    U64 attacks = 0ULL;
    // for (int pc = (int)Color::OPP_PAWN; pc <= (int)Color::OPP_QUEEN; pc++) {
    //     while (from_sqs) {
    //         Square from_sq = pop_lsb(from_sqs);
    //         if constexpr (std::is_same<Color, White>::value)
    //             attacks |= this->gen_piece_attacks<Black, (Piece)pc>(from_sq);
    //         else
    //             attacks |= this->gen_piece_attacks<White, (Piece)pc>(from_sq);
    //     }
    // }
    return attacks;
}