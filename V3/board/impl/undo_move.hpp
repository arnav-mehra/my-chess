#pragma once

#include "../Board.hpp"
#include "../../move/MoveList.hpp"

template<class Color>
void Board::undo_quiet(Move& m) {
    this->from_hist[m.get_from()]--;

    Piece piece = this->get_board(m.get_to());
    this->set_board(m.get_from(), piece         );
    this->set_board(m.get_to(),   Piece::GARBAGE);

    U64 bit_diff = m.get_from_bit() | m.get_to_bit();        
    this->bitboards[(int)piece]      ^= bit_diff;
    this->bitboards[(int)Color::ALL] ^= bit_diff;
    this->bitboards[(int)Piece::ALL] ^= bit_diff;
}

template<class Color>
void Board::undo_capture(Move& m) {
    this->from_hist[(int)m.get_from()]--;

    Piece piece = this->get_board(m.get_to());
    Piece capture = m.get_capture();
    this->set_board(m.get_from(), piece  );
    this->set_board(m.get_to(),   capture);

    U64 from_bit = m.get_from_bit();
    U64 to_bit   = m.get_to_bit();
    // re-add captured piece.
    this->bitboards[(int)capture]        ^= to_bit;
    this->bitboards[(int)Color::OPP_ALL] ^= to_bit;
    // move capturing piece.
    U64 bit_diff = from_bit | to_bit;
    this->bitboards[(int)piece]      ^= bit_diff;
    this->bitboards[(int)Color::ALL] ^= bit_diff;
    // overall change.
    this->bitboards[(int)Piece::ALL] ^= from_bit;
}

template<class Color, class Castle>
void Board::undo_castle() {
    this->from_hist[(int)Castle::KING_PRE]--;

    this->set_board(Castle::KING_PRE,  Color::KING   );
    this->set_board(Castle::ROOK_POST, Piece::GARBAGE);
    this->set_board(Castle::KING_POST, Piece::GARBAGE);
    this->set_board(Castle::ROOK_PRE,  Color::ROOK   );

    this->bitboards[(int)Color::KING] ^= Castle::KING_DELTA;
    this->bitboards[(int)Color::ROOK] ^= Castle::ROOK_DELTA;
    this->bitboards[(int)Color::ALL]  ^= Castle::DELTA;
    this->bitboards[(int)Piece::ALL]  ^= Castle::DELTA;
}

template<class Color>
void Board::undo_promo(Move& m) {
    constexpr bool turn = std::is_same<Color, White>::value;
    constexpr int promo_offset = 6 * (1 - turn) - 2;
    Piece promo_piece = (Piece)((int)m.get_flag() + (int)promo_offset);

    this->set_board(m.get_from(), Color::PAWN   );
    this->set_board(m.get_to(),   Piece::GARBAGE);

    U64 from_bit = m.get_from_bit();
    U64 to_bit   = m.get_to_bit();
    this->bitboards[(int)Color::PAWN] ^= from_bit;
    this->bitboards[(int)promo_piece] ^= to_bit;
    U64 bit_diff = from_bit | to_bit;
    this->bitboards[(int)Color::ALL] ^= bit_diff;
    this->bitboards[(int)Piece::ALL] ^= bit_diff;

    if (m.get_capture() != Piece::NA) {
        this->add_piece<Color>(m.get_to(), m.get_capture(), (Piece)Color::OPP_ALL);
    }
}

template<class Color>
void Board::undo_promo_capture(Move& m) {
    constexpr bool turn = std::is_same<Color, White>::value;
    constexpr int promo_offset = 6 * (1 - turn) - 2;
    Piece promo_piece = (Piece)((int)m.get_flag() + (int)promo_offset);

    Piece capture = m.get_capture();
    this->set_board(m.get_from(), Color::PAWN);
    this->set_board(m.get_to(),   capture    );

    U64 from_bit = m.get_from_bit();
    U64 to_bit   = m.get_to_bit();
    this->bitboards[(int)Color::PAWN]    ^= from_bit;
    this->bitboards[(int)promo_piece]    ^= to_bit;
    this->bitboards[(int)capture]        ^= to_bit;
    this->bitboards[(int)Color::OPP_ALL] ^= to_bit;
    U64 bit_diff = from_bit | to_bit;
    this->bitboards[(int)Color::ALL] ^= bit_diff;
    this->bitboards[(int)Piece::ALL] = this->bitboards[(int)Color::ALL]
                                     | this->bitboards[(int)Color::OPP_ALL]; // write straight from cache.
}

template<class Color>
void Board::undo_en_passant(Move& m) {
    Square en_passant_sq = m.get_to() + Color::BACKWARD;
    this->set_board(m.get_from(),  Color::PAWN    );
    this->set_board(m.get_to(),    Piece::GARBAGE );
    this->set_board(en_passant_sq, Color::OPP_PAWN);

    U64 from_bit = m.get_from_bit();
    U64 to_bit = m.get_to_bit();
    U64 bit_diff = from_bit | to_bit;
    this->bitboards[(int)Color::PAWN] ^= bit_diff;
    this->bitboards[(int)Color::ALL]  ^= bit_diff;
    
    U64 en_passant_bit = 1ULL << en_passant_sq;
    this->bitboards[(int)Color::OPP_PAWN] ^= en_passant_bit;
    this->bitboards[(int)Color::OPP_ALL]  ^= en_passant_bit;
    this->bitboards[(int)Piece::ALL] = this->bitboards[(int)Color::OPP_ALL]
                                     | this->bitboards[(int)Color::ALL];
}

template<class Color>
void Board::undo_move(Move& m) {
    Flag fg = m.get_flag();

    if (fg == Flag::REGULAR) {
        this->undo_quiet<Color>(m);
        if (m.get_capture() != Piece::NA) {
            this->add_piece<Color>(m.get_to(), m.get_capture(), (Piece)Color::OPP_ALL);
        }
        return;
    }

    if (fg == Flag::CASTLE) {
        if (m.get_to() == Color::OO::KING_POST) {
            this->undo_castle<Color, typename Color::OO >(); return;
        } else {
            this->undo_castle<Color, typename Color::OOO>(); return;
        }
    }
    
    if (fg == Flag::EN_PASSANT) {
        this->add_piece<Color>(m.get_to() + Color::BACKWARD, (Piece)Color::OPP_PAWN, (Piece)Color::OPP_ALL);
        this->undo_quiet<Color>(m);
        return;
    }

    this->undo_promo<Color>(m);
}