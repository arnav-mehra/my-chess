#pragma once

#include "../Board.hpp"
#include "../../move/MoveList.hpp"
#include "../../search/DrawTable.hpp"

template<class Color>
void inline Board::undo_regular(Piece pc, Piece capt, Square from, Square to) {
    this->move_piece<Color>(pc, to, from);

    U64 cap_delta = (1ULL << to) * (capt != Piece::NA);
    this->set_board(to, capt);
    this->bitboards[(int)capt]           ^= cap_delta;
    this->bitboards[(int)Color::OPP_ALL] ^= cap_delta;
}

template<class Color, class Castle>
void inline Board::undo_castle() {
    this->set_board(Castle::KING_PRE,  Color::KING);
    this->set_board(Castle::ROOK_POST, Piece::NA);
    this->set_board(Castle::KING_POST, Piece::NA);
    this->set_board(Castle::ROOK_PRE,  Color::ROOK);

    this->bitboards[(int)Color::KING] ^= Castle::KING_DELTA;
    this->bitboards[(int)Color::ROOK] ^= Castle::ROOK_DELTA;
    this->bitboards[(int)Color::ALL]  ^= Castle::DELTA;
}

template<class Color>
void inline Board::undo_promo(Flag fg, Piece capt, Square from, Square to) {
    constexpr bool turn = std::is_same<Color, White>::value;
    constexpr int promo_offset = 6 * (1 - turn) - 2;
    Piece promo_piece = (Piece)((int)fg + (int)promo_offset);
    U64   from_bit    = 1ULL << from;
    U64   to_bit      = 1ULL << to;
    U64   bit_diff    = from_bit | to_bit;

    this->set_board(from, Color::PAWN);
    this->set_board(to,   capt);

    this->bitboards[(int)Color::PAWN] ^= from_bit;
    this->bitboards[(int)promo_piece] ^= to_bit;
    this->bitboards[(int)Color::ALL]  ^= bit_diff;

    U64 cap_delta = to_bit * (capt != Piece::NA);
    this->bitboards[(int)capt]           ^= cap_delta;
    this->bitboards[(int)Color::OPP_ALL] ^= cap_delta;
}

template<class Color>
void inline Board::undo_en_passant(Square from, Square to) {
    this->move_piece<Color>((Piece)Color::PAWN, to, from);
    Square en_passant_sq = to + Color::BACKWARD;
    this->add_piece<Color>(en_passant_sq, (Piece)Color::OPP_PAWN, (Piece)Color::OPP_ALL);
}

template<class Color>
void Board::undo_move(Move& m) {
    Flag   fg   = m.get_flag();
    Piece  capt = m.get_capture();
    Square from = m.get_from();
    Square to   = m.get_to();
    Piece  pc   = this->get_board(to);

    if (fg == Flag::REGULAR) {
        this->undo_regular<Color>(pc, capt, from, to);
    }
    else if (fg == Flag::CASTLE) {
        if (to == Color::OO::KING_POST) {
            this->undo_castle<Color, typename Color::OO >();
        } else {
            this->undo_castle<Color, typename Color::OOO>();
        }
    }
    else if (fg == Flag::EN_PASSANT) {
        this->undo_en_passant<Color>(from, to);
    }
    else {
        this->undo_promo<Color>(fg, capt, from, to);
    }

    DrawTable::pop_position();
}