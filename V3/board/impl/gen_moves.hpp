#pragma once

#include "../Board.hpp"
#include "../../init/init.hpp"

// PAWNS: singles, doubles, promos, captures, promo-captures

template<class Color, Gen Gn>
void Board::gen_pawn_moves(MoveList &moves, U64 filter) {
    U64 promo_pawns = this->bitboards[Color::PAWN] & Color::PAWN_FINAL_RANK;
    U64 reg_pawns   = this->bitboards[Color::PAWN] & ~Color::PAWN_FINAL_RANK;

    if constexpr (Gn == Gen::QUIETS || Gn == Gen::BLOCKS) {
        U64 unoccup = ~this->bitboards[(int)Piece::ALL];
        U64 singles = shift<Color::FORWARD>(reg_pawns)   & unoccup;
        U64 doubles = shift<Color::FORWARD>(singles)     & unoccup & Color::PAWN_DOUBLE_RANK; 
        U64 promos  = shift<Color::FORWARD>(promo_pawns) & unoccup;

        singles &= filter;
        doubles &= filter;
        promos  &= filter;

        while (singles) {
            Square to = pop_lsb(singles);
            Square from = to - Color::FORWARD;
            moves.add(Move::make<Flag::QUIET>((Piece)Color::PAWN, Piece::NA, from, to));
        }
        while (doubles) {
            Square to = pop_lsb(doubles);
            Square from = to - 2 * Color::FORWARD;
            moves.add(Move::make<Flag::PAWN_DOUBLE>((Piece)Color::PAWN, Piece::NA, from, to));
        }
        while (promos) {
            Square to = pop_lsb(promos);
            moves.add_promos(to);
        }
    }

    if constexpr (Gn == Gen::CAPTURES || Gn == Gen::BLOCKS) {
        filter |= 1ULL << this->en_passant; // pawns can also capture en_passant

        U64 non_left_opp  = filter & NON_LEFT_PIECES;
        U64 non_right_opp = filter & NON_RIGHT_PIECES;

        U64 left_caps  = shift<(int)Color::FORWARD_LEFT >(reg_pawns) & non_right_opp;
        U64 right_caps = shift<(int)Color::FORWARD_RIGHT>(reg_pawns) & non_left_opp;

        while (left_caps) {
            Square to = pop_lsb(left_caps);
            Square from = to - (int)Color::FORWARD_LEFT;
            if (to == this->en_passant) {
                moves.add(Move::make<Flag::EN_PASSANT>((Piece)Color::PAWN, (Piece)Color::OPP_PAWN, from, to));
            } else {
                moves.add(Move::make<Flag::CAPTURE>((Piece)Color::PAWN, this->get_board(to), from, to));
            }
        }
        while (right_caps) {
            Square to = pop_lsb(right_caps);
            Square from = to - (int)Color::FORWARD_RIGHT;
            if (to == this->en_passant) {
                moves.add(Move::make<Flag::EN_PASSANT>((Piece)Color::PAWN, (Piece)Color::OPP_PAWN, from, to));
            } else {
                moves.add(Move::make<Flag::CAPTURE>((Piece)Color::PAWN, this->get_board(to), from, to));
            }
        }

        U64 left_cap_promos  = shift<(int)Color::FORWARD_LEFT >(promo_pawns) & non_right_opp;
        U64 right_cap_promos = shift<(int)Color::FORWARD_RIGHT>(promo_pawns) & non_left_opp;

        while (left_cap_promos) {
            Square to = pop_lsb(left_cap_promos);
            Square from = to - (int)Color::FORWARD_LEFT;
            moves.add_promo_captures(from, to);
        }
        while (right_cap_promos) {
            Square to = pop_lsb(right_cap_promos);
            Square from = to - (int)Color::FORWARD_RIGHT;
            moves.add_promo_captures(from, to);
        }
    }
}

// PIECES

template<class Color, Piece Pc>
U64 Board::gen_piece_attacks(Square from_sq) {
    U64 occup = this->bitboards[(int)Piece::ALL];
    return Pc == (Piece)Color::QUEEN  ? KMAGICS::get_q_attacks(from_sq, occup)
         : Pc == (Piece)Color::BISHOP ? KMAGICS::get_b_attacks(from_sq, occup)
         : Pc == (Piece)Color::ROOK   ? KMAGICS::get_r_attacks(from_sq, occup)
         : Pc == (Piece)Color::KNIGHT ? MAPPED_MOVES::get_n_attacks(from_sq)
         : Pc == (Piece)Color::KING   ? MAPPED_MOVES::get_k_attacks(from_sq)
         : Pc == (Piece)Color::PAWN   ? Color::PAWN_ATTACKS[from_sq]
         : 0ULL;
}

template<class Color, Gen Gn, Piece Pc>
void Board::_gen_piece_moves(MoveList &moves, U64 filter, Square from_sq) {
    constexpr Flag Fg = Gn == Gen::CAPTURES ? Flag::CAPTURE
                                            : Flag::QUIET;

    U64 attacks = this->gen_piece_attacks<Color, Pc>(from_sq);
    U64 to_bb = attacks & filter;
    while (to_bb) {
        Square to_sq = pop_lsb(to_bb);
        moves.add(
            Move::make<Fg>(Pc, this->get_board(to_sq), from_sq, to_sq)
        );
    }
}

template<class Color, Gen Gn, Piece Pc>
void Board::gen_piece_moves(MoveList &moves, U64 filter) {
    U64 from_bb = this->get_bitboard(Pc);

    if constexpr (Pc == (Piece)Color::KING) {
        Square from_sq = lsb(from_bb);
        this->_gen_piece_moves<Color, Gn, Pc>(moves, filter, from_sq);
    }
    else {
        while (from_bb) {
            Square from_sq = pop_lsb(from_bb);
            this->_gen_piece_moves<Color, Gn, Pc>(moves, filter, from_sq);
        }
    }
}

// CASTLES

template<class Color, class Castle>
void Board::gen_castle(MoveList &moves) {
    bool is_clear = (Castle::EMPTY_MASK & this->bitboards[(int)Piece::ALL]) == 0ULL;
    bool has_right = this->from_hist[Castle::KING_PRE] == 0 && this->from_hist[Castle::ROOK_PRE] == 0;
    bool rook_exists = (this->bitboards[(int)Color::ROOK] & (1ULL << Castle::ROOK_PRE)) != 0ULL;
    
    bool no_knight_attacks  = (Castle::KNIGHT_RISKS & this->bitboards[Color::OPP_KNIGHT]) == 0ULL;
    bool no_pawn_attacks    = (Castle::PAWN_RISKS   & this->bitboards[Color::OPP_PAWN]  ) == 0ULL;
    bool no_sliding_attacks = !sliding_castle_checks<Color, Castle>();

    bool can_castle = is_clear & has_right & rook_exists
                    & no_knight_attacks & no_pawn_attacks & no_sliding_attacks;
    moves.add_castle<Castle>(can_castle);
}

// GENS

template<class Color, Gen Gn>
void Board::gen_moves(MoveList &moves, U64 filter) {
    this->gen_pawn_moves<Color, Gn>(moves, filter);
    this->gen_piece_moves<Color, Gn, (Piece)Color::KNIGHT>(moves, filter);
    this->gen_piece_moves<Color, Gn, (Piece)Color::BISHOP>(moves, filter);
    this->gen_piece_moves<Color, Gn, (Piece)Color::ROOK  >(moves, filter);
    this->gen_piece_moves<Color, Gn, (Piece)Color::QUEEN >(moves, filter);
    this->gen_piece_moves<Color, Gn, (Piece)Color::KING  >(moves, filter);
}

template<class Color, Gen Gn>
void Board::gen_moves(MoveList &moves) {
    if constexpr (Gn == Gen::QUIETS) {
        U64 filter = ~this->get_bitboard(Piece::ALL);
        this->gen_moves<Color, Gen::QUIETS>(moves, filter);

        if (this->get_checks<Color>() == 0ULL) {
            this->gen_castle<Color, typename Color::OO >(moves);
            this->gen_castle<Color, typename Color::OOO>(moves);
        }
    }

    if constexpr (Gn == Gen::CAPTURES) {
        U64 filter = this->get_bitboard(Color::OPP_ALL);
        this->gen_moves<Color, Gen::CAPTURES>(moves, filter);
    }

    if constexpr (Gn == Gen::PSEUDOS) {
        this->gen_moves<Color, Gen::QUIETS  >(moves);
        this->gen_moves<Color, Gen::CAPTURES>(moves);
    }

    // if constexpr (Gn == Gen::BLOCKS) {
    //     U64 checks = this->get_checks<Color>();
    //     U64 filter = this->get_check_blocks<Color>(checks);
    //     this->gen_moves_with_filter<Color, Gen::QUIETS,   false>(moves, filter);
    //     this->gen_moves_with_filter<Color, Gen::CAPTURES, false>(moves, filter);
    // }

    // if constexpr (Gn == Gen::LEGALS) {
    //     U64 checks = this->get_checks<Color>();
    //     int num_checks = pop_count(checks);

    //     switch (num_checks) {
    //         case 0: { // filter on from sqs
    //             U64 pins = this->get_pins<Color>();
    //             U64 filter1 = ~pins; // moveable with worrying about legality
    //             U64 filter2 = pins; // brute force on this.
    //             this->gen_moves_with_filter<Color, Gen::QUIETS,   false>(moves, filter2);
    //             this->gen_moves_with_filter<Color, Gen::CAPTURES, false>(moves, filter2);
    //             this->gen_piece_moves<Color, Flag::QUIET,   Color::KING>(moves, this->get_bitboard(Color::ALL));
    //             this->gen_piece_moves<Color, Flag::CAPTURE, Color::KING>(moves, this->get_bitboard(Color::OPP_ALL));
    //             {
    //                 int& len = moves.size();
    //                 for (int i = 0; i < moves.size(); i++) {
    //                     this->do_move<Color>(moves[i]);
    //                     bool was_illegal = this->get_checks<Color>() != 0ULL;
    //                     this->undo_move<Color>(moves[i]);
    //                     if (was_illegal) {
    //                         moves[i--] = moves[--len];
    //                     }
    //                 }
    //             }
    //             this->gen_moves_with_filter<Color, Gen::QUIETS  , false>(moves, filter1);
    //             this->gen_moves_with_filter<Color, Gen::CAPTURES, false>(moves, filter1);
    //             this->gen_castle<Color, typename Color::OO >(moves);
    //             this->gen_castle<Color, typename Color::OOO>(moves);
    //             break;
    //         }
    //         case 1: { // filter on to_sqs
    //             U64 pins = this->get_pins<Color>();
    //             U64 check_blocks = this->get_check_blocks<Color>(this->get_checks<Color>());
    //             U64 filter = check_blocks & ~pins; // not on kings
    //             this->gen_moves_with_filter<Color, Gen::QUIETS,   false>(moves, filter);
    //             this->gen_moves_with_filter<Color, Gen::CAPTURES, false>(moves, filter);
    //             break;
    //         }
    //         case 2: {
    //             // brute force on king moves. degenerate case.
    //             this->gen_piece_moves<Color, Flag::QUIET,   Color::KING>(moves, this->get_bitboard(Color::ALL));
    //             this->gen_piece_moves<Color, Flag::CAPTURE, Color::KING>(moves, this->get_bitboard(Color::OPP_ALL));
    //             {
    //                 int& len = moves.size();
    //                 for (int i = 0; i < moves.size(); i++) {
    //                     this->do_move<Color>(moves[i]);
    //                     bool was_illegal = this->get_checks<Color>() != 0ULL;
    //                     this->undo_move<Color>(moves[i]);
    //                     if (was_illegal) {
    //                         moves[i--] = moves[--len];
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
}

// template<class Color>
// void Board::gen_moves<Gen::LEGALS>(MoveList &moves) {
//     int checks = pop_count(this->get_checks());
//     U64 pins = this->get_pins<Color>();

//     if (checks) {
//         // singles, doubles, promos, captures, promo-captures
//         this->gen_pawn_moves  <Color, Gen::EVASIONS>(moves);

//         this->gen_knight_moves<Color, Gen::EVASIONS>(moves);
//         this->gen_king_moves  <Color, Gen::EVASIONS>(moves);

//         this->gen_queen_moves <Color, Gen::EVASIONS>(moves);
//         this->gen_rook_moves  <Color, Gen::EVASIONS>(moves);
//         this->gen_bishop_moves<Color, Gen::EVASIONS>(moves);

//     } else {
//         // singles, doubles, promos, captures, promo-captures
//         this->gen_pawn_moves  <Color, Gen::NON_EVASIONS>(moves);

//         this->gen_knight_moves<Color, Gen::NON_EVASIONS>(moves);
//         this->gen_king_moves  <Color, Gen::NON_EVASIONS>(moves);

//         this->gen_queen_moves <Color, Gen::NON_EVASIONS>(moves);
//         this->gen_rook_moves  <Color, Gen::NON_EVASIONS>(moves);
//         this->gen_bishop_moves<Color, Gen::NON_EVASIONS>(moves);
        
//         this->gen_castle<Color, typename Color::OO>(moves);
//         this->gen_castle<Color, typename Color::OOO>(moves);
//     }
// }