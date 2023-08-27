#pragma once

#include "../Board.hpp"
#include "../../move/Move.hpp"
#include "../../init/init.hpp"
#include "context.hpp"

// PAWNS: singles, doubles, promos, captures, promo-captures

template<class Color, GenType Gn>
void Board::gen_pawn_moves(MoveList &moves, Context& ctx, U64 filter) {
    if (Gn == GenType::PSEUDOS) {
        gen_pawn_moves<Color, GenType::CAPTURES>(moves, ctx, this->get_bitboard(Color::OPP_ALL));
        gen_pawn_moves<Color, GenType::QUIETS  >(moves, ctx, this->get_unocc());
        return;
    }

    U64 promo_pawns = this->bitboards[Color::PAWN] & Color::PAWN_FINAL_RANK;
    U64 reg_pawns   = this->bitboards[Color::PAWN] & ~Color::PAWN_FINAL_RANK;

    if constexpr (Gn == GenType::QUIETS || Gn == GenType::BLOCKS) {
        U64 singles = shift<Color::FORWARD>(reg_pawns)   & filter;
        U64 doubles = shift<Color::FORWARD>(singles)     & filter & Color::PAWN_DOUBLE_RANK; 
        U64 promos  = shift<Color::FORWARD>(promo_pawns) & filter;

        while (singles) {
            Square to = pop_lsb(singles);
            Square from = to - Color::FORWARD;
            moves.add(Move::make<Flag::REGULAR>(from, to));
        }
        while (doubles) {
            Square to = pop_lsb(doubles);
            Square from = to - 2 * Color::FORWARD;
            moves.add(Move::make<Flag::REGULAR>(from, to));
        }
        while (promos) {
            Square to = pop_lsb(promos);
            Square from = to - Color::FORWARD;
            moves.add_promos<Color>(from, to);
        }
    }

    if constexpr (Gn == GenType::CAPTURES || Gn == GenType::BLOCKS) {
        U64 non_left_opp  = filter & NON_LEFT_PIECES;
        U64 non_right_opp = filter & NON_RIGHT_PIECES;

        U64 left_caps  = shift<(int)Color::FORWARD_LEFT >(reg_pawns) & non_right_opp;
        U64 right_caps = shift<(int)Color::FORWARD_RIGHT>(reg_pawns) & non_left_opp;

        while (left_caps) {
            Square to = pop_lsb(left_caps);
            Square from = to - (Square)Color::FORWARD_LEFT;
            moves.add(Move::make<Flag::REGULAR>(from, to));
        }
        while (right_caps) {
            Square to = pop_lsb(right_caps);
            Square from = to - (Square)Color::FORWARD_RIGHT;
            moves.add(Move::make<Flag::REGULAR>(from, to));
        }

        U64 left_cap_promos  = shift<(int)Color::FORWARD_LEFT >(promo_pawns) & non_right_opp;
        U64 right_cap_promos = shift<(int)Color::FORWARD_RIGHT>(promo_pawns) & non_left_opp;

        while (left_cap_promos) {
            Square to = pop_lsb(left_cap_promos);
            Square from = to - (Square)Color::FORWARD_LEFT;
            moves.add_promos<Color>(from, to);
        }
        while (right_cap_promos) {
            Square to = pop_lsb(right_cap_promos);
            Square from = to - (Square)Color::FORWARD_RIGHT;
            moves.add_promos<Color>(from, to);
        }

        if (ctx.en_passant) {
            U64 pot_l_attackers = 1ULL << (ctx.en_passant - Color::FORWARD_RIGHT);
            U64 pot_r_attackers = 1ULL << (ctx.en_passant - Color::FORWARD_LEFT );
            U64 pot_attackers = (pot_l_attackers & NON_RIGHT_PIECES) | (pot_r_attackers & NON_LEFT_PIECES);
            U64 attackers = pot_attackers & reg_pawns;
            
            while (attackers) {
                U64 from = pop_lsb(attackers);
                U64 to = ctx.en_passant;
                moves.add(Move::make<Flag::EN_PASSANT>(from, to));
            }
        }
    }
}

// PIECES

template<class Color, Piece Pc>
U64 Board::gen_piece_attacks(Square from_sq) {
    return Pc == (Piece)Color::QUEEN  ? KMAGICS::get_q_attacks(from_sq, this->get_occ())
         : Pc == (Piece)Color::BISHOP ? KMAGICS::get_b_attacks(from_sq, this->get_occ())
         : Pc == (Piece)Color::ROOK   ? KMAGICS::get_r_attacks(from_sq, this->get_occ())
         : Pc == (Piece)Color::KNIGHT ? MAPPED_MOVES::get_n_attacks(from_sq)
         : Pc == (Piece)Color::KING   ? MAPPED_MOVES::get_k_attacks(from_sq)
         : Pc == (Piece)Color::PAWN   ? Color::PAWN_ATTACKS[from_sq]
         : 0ULL;
}

template<class Color, Piece Pc>
void Board::_gen_piece_moves(MoveList &moves, U64 filter, Square from_sq) {
    U64 attacks = this->gen_piece_attacks<Color, Pc>(from_sq);
    U64 to_bb = attacks & filter;
    while (to_bb) {
        Square to_sq = pop_lsb(to_bb);
        moves.add(Move::make<Flag::REGULAR>(from_sq, to_sq));
    }
}

template<class Color, Piece Pc>
void Board::gen_piece_moves(MoveList &moves, U64 filter) {
    U64 from_bb = this->get_bitboard(Pc);

    if constexpr (Pc == (Piece)Color::KING) {
        Square from_sq = lsb(from_bb);
        this->_gen_piece_moves<Color, Pc>(moves, filter, from_sq);
    }
    else {
        while (from_bb) {
            Square from_sq = pop_lsb(from_bb);
            this->_gen_piece_moves<Color, Pc>(moves, filter, from_sq);
        }
    }
}

// CASTLES

template<class Color, class Castle>
void Board::gen_castle(MoveList& moves, Context& ctx) {    
    bool rook_exists = (this->get_bitboard(Color::ROOK) & (1ULL << Castle::ROOK_PRE)) != 0ULL;

    U64 moved    = ctx.moved_castling_pieces<Castle>();
    U64 blockers = Castle::EMPTY_MASK & this->get_occ();
    U64 checks   = castling_checks<Color, Castle>();
    
    bool can_castle = ((blockers | checks | moved) == 0ULL) & rook_exists;
    moves.add_castle<Castle>(can_castle);
}

// GENS

template<class Color, GenType Gn>
void Board::gen_moves(MoveList& moves, Context& ctx) {
    U64 filter = Gn == GenType::CAPTURES ? this->get_bitboard(Color::OPP_ALL)
               : Gn == GenType::QUIETS   ? this->get_unocc()
               : Gn == GenType::PSEUDOS  ? ~this->get_bitboard(Color::ALL)
               : 0ULL;

    this->gen_pawn_moves <Color, Gn>(moves, ctx, filter);

    this->gen_piece_moves<Color, (Piece)Color::KNIGHT>(moves, filter);
    this->gen_piece_moves<Color, (Piece)Color::BISHOP>(moves, filter);
    this->gen_piece_moves<Color, (Piece)Color::ROOK  >(moves, filter);
    this->gen_piece_moves<Color, (Piece)Color::QUEEN >(moves, filter);
    this->gen_piece_moves<Color, (Piece)Color::KING  >(moves, filter);

    if constexpr (Gn == GenType::QUIETS || Gn == GenType::PSEUDOS) {
        if (this->get_checks<Color>() == 0ULL) {
            this->gen_castle<Color, typename Color::OO >(moves, ctx);
            this->gen_castle<Color, typename Color::OOO>(moves, ctx);
        }
    }
}

template<class Color, GenType Gn>
void Board::gen_order_moves(
    MoveList& ml,
    Context& ctx,
    Move priority_move,
    U16 depth
) {
    this->gen_moves<Color, Gn>(ml, ctx);
    ml.fill_moves<Color, Gn>(this, depth, priority_move);
    ml.sort();
}