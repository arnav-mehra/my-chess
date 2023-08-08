#pragma once

#include "../Board.hpp"
#include "context.hpp"

template<class Color>
void Board::remove_piece(Square sq, Piece pc, Piece pc_col_all) {
    this->set_board(sq, Piece::NA);

    U64 to_bit = 1ULL << sq;
    this->bitboards[(int)pc]         ^= to_bit;
    this->bitboards[(int)pc_col_all] ^= to_bit;
}

template<class Color>
void Board::remove_piece(Context& ctx, Square sq, Piece pc, Piece pc_col_all) {
    ctx.toggle_hash_piece(pc, sq);
    this->remove_piece<Color>(sq, pc, pc_col_all);
}

template<class Color>
void Board::add_piece(Square sq, Piece pc, Piece pc_col_all) {
    this->set_board(sq, pc);

    U64 to_bit = 1ULL << sq;
    this->bitboards[(int)pc]         ^= to_bit;
    this->bitboards[(int)pc_col_all] ^= to_bit;
}

template<class Color>
void Board::add_piece(Context& ctx, Square sq, Piece pc, Piece pc_col_all) {
    ctx.toggle_hash_piece(pc, sq);
    this->remove_piece<Color>(sq, pc, pc_col_all);
}

template<class Color>
void Board::move_piece(Piece pc, Square from, Square to) {
    this->set_board(from, Piece::NA);
    this->set_board(to,   pc);

    U64 delta = (1ULL << from) | (1ULL << to);
    this->bitboards[(int)pc]         ^= delta;
    this->bitboards[(int)Color::ALL] ^= delta;
}

template<class Color>
void Board::move_piece(Context& ctx, Piece pc, Square from, Square to) {
    ctx.toggle_hash_piece(pc, from);
    ctx.toggle_hash_piece(pc, to);
    this->move_piece<Color>(pc, from, to);
}

template<class Color>
void Board::do_regular(Context& ctx, Piece pc, Piece capt, Square from, Square to) {
    ctx.toggle_hash_piece(capt, to);
    U64 to_bit = 1ULL << to;
    this->bitboards[(int)capt]           &= ~to_bit;
    this->bitboards[(int)Color::OPP_ALL] &= ~to_bit;

    this->move_piece<Color>(ctx, pc, from, to);
    ctx.set_en_passant<Color>(pc, from, to);
}

template<class Color, class Castle>
void Board::do_castle(Context& ctx) {
    ctx.toggle_hash_piece((Piece)Color::KING, (Square)Castle::KING_PRE );
    ctx.toggle_hash_piece((Piece)Color::KING, (Square)Castle::KING_POST);
    ctx.toggle_hash_piece((Piece)Color::ROOK, (Square)Castle::ROOK_PRE );
    ctx.toggle_hash_piece((Piece)Color::ROOK, (Square)Castle::ROOK_POST);

    this->set_board(Castle::KING_PRE,  Piece::NA  );
    this->set_board(Castle::ROOK_POST, Color::ROOK);
    this->set_board(Castle::KING_POST, Color::KING);
    this->set_board(Castle::ROOK_PRE,  Piece::NA  );

    this->bitboards[(int)Color::KING] ^= Castle::KING_DELTA;
    this->bitboards[(int)Color::ROOK] ^= Castle::ROOK_DELTA;
    this->bitboards[(int)Color::ALL]  ^= Castle::DELTA;
}

template<class Color>
void Board::do_promo(Context& ctx, Flag fg, Piece capt, Square from, Square to) {
    constexpr bool turn = std::is_same<Color, White>::value;
    constexpr int promo_offset = 6 * (1 - turn) - 2;
    Piece promo_piece = (Piece)((int)fg + (int)promo_offset);
    U64 to_bit = 1ULL << to;
    U64 from_bit = 1ULL << from;
    U64 bit_diff = from_bit | to_bit;

    ctx.toggle_hash_piece(capt, to);
    this->bitboards[(int)capt]           &= ~to_bit;
    this->bitboards[(int)Color::OPP_ALL] &= ~to_bit;

    ctx.toggle_hash_piece(promo_piece, to);
    ctx.toggle_hash_piece((Piece)Color::PAWN, from);

    this->set_board(from, Piece::NA);
    this->set_board(to,   promo_piece);

    this->bitboards[(int)Color::PAWN] ^= from_bit;
    this->bitboards[(int)promo_piece] ^= to_bit;
    this->bitboards[(int)Color::ALL] ^= bit_diff;
}

template<class Color>
void Board::do_en_passant(Context& ctx, Square from, Square to) {
    Square cap_sq = to + Color::BACKWARD;
    this->remove_piece<Color>(ctx, cap_sq, (Piece)Color::OPP_PAWN, (Piece)Color::OPP_ALL);
    this->move_piece<Color>(ctx, (Piece)Color::PAWN, from, to);
}

template<class Color>
void Board::do_move(Move& m, Context& ctx) {
    ctx.toggle_hash_turn();
    ctx.toggle_castling_rights(m.get_from());

    Flag   fg   = m.get_flag();
    Square from = m.get_from();
    Square to   = m.get_to();
    Piece  pc   = this->get_board(from);
    Piece  capt = m.get_capture();

    if (fg == Flag::REGULAR) {
        this->do_regular<Color>(ctx, pc, capt, from, to);
    }
    else if (fg == Flag::CASTLE) {
        if (to == Color::OO::KING_POST) {
            this->do_castle<Color, typename Color::OO >(ctx);
        } 
        else {
            this->do_castle<Color, typename Color::OOO>(ctx);
        }
    }
    else if (fg == Flag::EN_PASSANT) {
        this->do_en_passant<Color>(ctx, from, to);
    }
    else {
        this->do_promo<Color>(ctx, fg, capt, from, to);
    }
}