#pragma once

#include "../Board.hpp"
#include "../../init/init.hpp"
#include "../../util/conversion.hpp"

template<class Color, class Castle>
U64 Board::castling_checks() {
    U64 occ_wo_king = this->get_occ() & ~this->bitboards[(int)Color::KING];

    U64 rook_risks = 0ULL, bishop_risks = 0ULL;
    for (Square sq : Castle::NON_CHECKS) {
        rook_risks   |= KMAGICS::get_r_attacks(sq, occ_wo_king);
        bishop_risks |= KMAGICS::get_b_attacks(sq, occ_wo_king);
    }
    U64 queen_risks = rook_risks | bishop_risks;

    U64 pawn_attacks   = Castle::PAWN_RISKS   & this->get_bitboard(Color::OPP_PAWN);
    U64 knight_attacks = Castle::KNIGHT_RISKS & this->get_bitboard(Color::OPP_KNIGHT);
    U64 king_attacks   = Castle::KING_RISKS   & this->get_bitboard(Color::OPP_KING);

    U64 bishop_attackers = bishop_risks & this->get_bitboard(Color::OPP_BISHOP);
    U64 rook_attackers   = rook_risks   & this->get_bitboard(Color::OPP_ROOK);
    U64 queen_attackers  = queen_risks  & this->get_bitboard(Color::OPP_QUEEN);
    
    return pawn_attacks | knight_attacks | king_attacks
         | bishop_attackers | rook_attackers | queen_attackers;
}

template<class Color>
U64 Board::get_checks() {
    U64 occ = this->get_occ();
        
    Square sq = lsb(this->get_bitboard(Color::KING));

    U64 pawn_risks   = Color::PAWN_ATTACKS[sq];
    U64 knight_risks = MAPPED_MOVES::KNIGHT_MOVES[sq];
    U64 rook_risks   = KMAGICS::get_r_attacks(sq, occ);
    U64 bishop_risks = KMAGICS::get_b_attacks(sq, occ);
    U64 queen_risks  = rook_risks | bishop_risks;
    U64 king_risks   = MAPPED_MOVES::get_k_attacks(sq);

    U64 pawn_attackers   = pawn_risks   & this->bitboards[Color::OPP_PAWN];
    U64 knight_attackers = knight_risks & this->bitboards[Color::OPP_KNIGHT];
    U64 bishop_attackers = bishop_risks & this->bitboards[Color::OPP_BISHOP];
    U64 rook_attackers   = rook_risks   & this->bitboards[Color::OPP_ROOK];
    U64 queen_attackers  = queen_risks  & this->bitboards[Color::OPP_QUEEN];
    U64 king_attackers   = king_risks   & this->bitboards[Color::OPP_KING];

    return bishop_attackers | rook_attackers | queen_attackers
         | knight_attackers | pawn_attackers | king_attackers;
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

void Board::assert_board_consistency() {
    // BITBOARD self-consistency.

    // accidental aggregate miscalculation.
    assert(
        "WHITE_AGGREGATE",
        bitboards[(int)Piece::WHITE_ALL] == (
            bitboards[(int)Piece::WHITE_PAWN]
            | bitboards[(int)Piece::WHITE_KNIGHT]
            | bitboards[(int)Piece::WHITE_BISHOP]
            | bitboards[(int)Piece::WHITE_ROOK]
            | bitboards[(int)Piece::WHITE_QUEEN]
            | bitboards[(int)Piece::WHITE_KING]
        )
    );
    assert(
        "BLACK_AGGREGATE",
        bitboards[(int)Piece::BLACK_ALL] == (
            bitboards[(int)Piece::BLACK_PAWN]
            | bitboards[(int)Piece::BLACK_KNIGHT]
            | bitboards[(int)Piece::BLACK_BISHOP]
            | bitboards[(int)Piece::BLACK_ROOK]
            | bitboards[(int)Piece::BLACK_QUEEN]
            | bitboards[(int)Piece::BLACK_KING]
        )
    );

    // accidental piece non-removal or addition.
    // if piece overlap, additional +1 to pop_count.
    assert(
        "WHITE_EXCLUSIVITY",
        pop_count(bitboards[(int)Piece::WHITE_ALL]) == ( 
            pop_count(bitboards[(int)Piece::WHITE_PAWN])
            + pop_count(bitboards[(int)Piece::WHITE_KNIGHT])
            + pop_count(bitboards[(int)Piece::WHITE_BISHOP])
            + pop_count(bitboards[(int)Piece::WHITE_ROOK])
            + pop_count(bitboards[(int)Piece::WHITE_QUEEN])
            + pop_count(bitboards[(int)Piece::WHITE_KING])
        )
    );
    assert(
        "BLACK_EXCLUSIVITY",
        pop_count(bitboards[(int)Piece::BLACK_ALL]) == ( 
            pop_count(bitboards[(int)Piece::BLACK_PAWN])
            + pop_count(bitboards[(int)Piece::BLACK_KNIGHT])
            + pop_count(bitboards[(int)Piece::BLACK_BISHOP])
            + pop_count(bitboards[(int)Piece::BLACK_ROOK])
            + pop_count(bitboards[(int)Piece::BLACK_QUEEN])
            + pop_count(bitboards[(int)Piece::BLACK_KING])
        )
    );
    assert(
        "COLOR_EXCLUSIVITY",
        0ULL == (
            bitboards[(int)Piece::BLACK_ALL]
            & bitboards[(int)Piece::WHITE_ALL]
        )
    );

    // BOARD vs. BITBOARD consistency.

    // presence
    for (int pc = (int)Piece::WHITE_PAWN; pc <= (int)Piece::BLACK_KING; pc++) {
        U64 cpy = bitboards[pc];
        while (cpy) {
            int sq = pop_lsb(cpy);
            std::string name = PIECE_NAMES[pc] + " PRESENCE AT " + std::to_string(sq);
            assert(name, this->get_board(sq) == (Piece)pc);
        }
    }
    // void
    U64 void_bits = ~(this->get_bitboard(Piece::BLACK_ALL) | this->get_bitboard(Piece::WHITE_ALL));
    while (void_bits) {
        int sq = pop_lsb(void_bits);
        std::string name = "VOID AT " + std::to_string(sq);
        assert(name, this->get_board(sq) == Piece::NA);
    }

    // OTHER

    // accidental piece creation.
    assert(
        "WHITE OVERFLOW",
        pop_count(bitboards[(int)Piece::WHITE_ALL]) <= 16
    );
    assert(
        "BLACK OVERFLOW",
        pop_count(bitboards[(int)Piece::BLACK_ALL]) <= 16
    );
    assert(
        "WHITE PAWN OVERFLOW",
        pop_count(bitboards[(int)Piece::WHITE_PAWN]) <= 8
    );
    assert(
        "BLACK PAWN OVERFLOW",
        pop_count(bitboards[(int)Piece::BLACK_PAWN]) <= 8
    );

    // success("ASSERTIONS PASSED!");
}

bool Board::operator==(Board &b) {
    for (int i = 0; i < NUM_SQUARES; i++) {
        if (b.get_board(i) != this->get_board(i)) {
            std::cout << "NOT EQUAL: square " << i << "\n";
            return false;
        }
    }
    for (int i = 0; i < NUM_BITBOARDS; i++) {
        if (b.get_bitboard(i) != this->get_bitboard(i)) {
            std::cout << "NOT EQUAL: bitboard " << PIECE_NAMES[i] << "\n";
            return false;
        }
    }
    return true;
}