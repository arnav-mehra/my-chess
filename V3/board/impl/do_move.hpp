#pragma once
#include "../Board.hpp"

template<class Color>
void Board::do_quiet(Move& m) {
    this->from_hist[m.get_from()]++;

    Piece piece = this->get_board(m.get_from());
    this->set_board(m.get_from(), Piece::GARBAGE);
    this->set_board(m.get_to(),   piece         );

    U64 bit_diff = m.get_from_bit() | m.get_to_bit();
    this->bitboards[(int)piece]      ^= bit_diff;
    this->bitboards[(int)Color::ALL] ^= bit_diff;
    this->bitboards[(int)Piece::ALL] ^= bit_diff;

    U32 enables_ep = m.get_to() == (m.get_from() + Color::FORWARD + Color::FORWARD);
    enables_ep &= (piece == (Piece)Color::PAWN);
    this->en_passant = enables_ep * (m.get_from() + Color::FORWARD);
}

template<class Color>
void Board::remove_piece(Square sq, Piece pc, Piece pc_col_all) {
    this->set_board(sq, Piece::GARBAGE);

    U64 to_bit = 1ULL << sq;
    this->bitboards[(int)pc]         ^= to_bit;
    this->bitboards[(int)pc_col_all] ^= to_bit;
    this->bitboards[(int)Piece::ALL] ^= to_bit;
}

template<class Color>
void Board::add_piece(Square sq, Piece pc, Piece pc_col_all) {
    this->set_board(sq, pc);

    U64 to_bit = 1ULL << sq;
    this->bitboards[(int)pc]         ^= to_bit;
    this->bitboards[(int)pc_col_all] ^= to_bit;
    this->bitboards[(int)Piece::ALL] ^= to_bit;
}

template<class Color>
void Board::do_capture(Move& m) {
    this->from_hist[(int)m.get_from()]++;

    Piece piece   = this->get_board(m.get_from());
    Piece capture = this->get_board(m.get_to());
    this->set_board(m.get_from(), Piece::GARBAGE);
    this->set_board(m.get_to(), piece);

    U64 from_bit = m.get_from_bit();
    U64 to_bit   = m.get_to_bit();
    U64 bit_diff = from_bit | to_bit;
    // remove captured piece.
    this->bitboards[(int)capture]        ^= to_bit;
    this->bitboards[(int)Color::OPP_ALL] ^= to_bit;
    // move capturing piece.
    this->bitboards[(int)piece]      ^= bit_diff;
    this->bitboards[(int)Color::ALL] ^= bit_diff;
    // overall change.
    this->bitboards[(int)Piece::ALL] ^= from_bit;

    this->en_passant = 0;
}

template<class Color, class Castle>
void Board::do_castle() {
    this->from_hist[(int)Castle::KING_PRE]++;

    this->set_board(Castle::KING_PRE,  Piece::GARBAGE);
    this->set_board(Castle::ROOK_POST, Color::ROOK   );
    this->set_board(Castle::KING_POST, Color::KING   );
    this->set_board(Castle::ROOK_PRE,  Piece::GARBAGE);
    
    this->bitboards[(int)Color::KING] ^= Castle::KING_DELTA;
    this->bitboards[(int)Color::ROOK] ^= Castle::ROOK_DELTA;
    this->bitboards[(int)Color::ALL]  ^= Castle::DELTA;
    this->bitboards[(int)Piece::ALL]  ^= Castle::DELTA;

    this->en_passant = 0;
}

template<class Color>
void Board::do_promo(Move& m) {
    constexpr bool turn = std::is_same<Color, White>::value;
    constexpr int promo_offset = 6 * (1 - turn) - 2;
    Piece promo_piece = (Piece)((int)m.get_flag() + (int)promo_offset);

    if (m.get_capture() != Piece::NA) {
        this->remove_piece<Color>(m.get_to(), m.get_capture(), (Piece)Color::OPP_ALL);
    }

    this->set_board(m.get_from(), Piece::GARBAGE);
    this->set_board(m.get_to(),   promo_piece   );

    U64 from_bit = m.get_from_bit();
    U64 to_bit   = m.get_to_bit();
    this->bitboards[(int)Color::PAWN] ^= from_bit;
    this->bitboards[(int)promo_piece] ^= to_bit;

    U64 bit_diff = from_bit | to_bit;
    this->bitboards[(int)Color::ALL] ^= bit_diff;
    this->bitboards[(int)Piece::ALL] ^= bit_diff;
    
    this->en_passant = 0;
}

template<class Color>
void Board::do_promo_capture(Move& m) {
    constexpr bool turn = std::is_same<Color, White>::value;
    constexpr int promo_offset = 6 * (1 - turn) - 2;
    Piece promo_piece = (Piece)((int)m.get_flag() + (int)promo_offset);

    Piece capture = this->get_board(m.get_to());
    this->set_board(m.get_from(), Piece::GARBAGE);
    this->set_board(m.get_to(),   promo_piece   );

    U64 from_bit = m.get_from_bit();
    U64 to_bit = m.get_to_bit();
    this->bitboards[(int)Color::PAWN]    ^= from_bit;
    this->bitboards[(int)promo_piece]    ^= to_bit;
    this->bitboards[(int)capture]        ^= to_bit;
    this->bitboards[(int)Color::OPP_ALL] ^= to_bit;

    U64 bit_diff = from_bit | to_bit;
    this->bitboards[(int)Color::ALL] ^= bit_diff;
    this->bitboards[(int)Piece::ALL] = this->bitboards[(int)Color::ALL]
                                     | this->bitboards[(int)Color::OPP_ALL]; // write straight from cache.
    
    this->en_passant = 0;
}

template<class Color>
void Board::do_en_passant(Move& m) {
    Square en_passant_sq = m.get_to() + Color::BACKWARD;
    this->set_board(m.get_from(),  Piece::GARBAGE);
    this->set_board(m.get_to(),    Color::PAWN   );
    this->set_board(en_passant_sq, Piece::GARBAGE);

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

    this->en_passant = 0;
}

template<class Color>
void Board::do_move(Move& m) {
    Flag fg = m.get_flag();

    if (fg == Flag::REGULAR) {
        if (m.get_capture() != Piece::NA) {
            this->remove_piece<Color>(m.get_to(), m.get_capture(), (Piece)Color::OPP_ALL);
        }
        this->do_quiet<Color>(m);
        return;
    }

    if (fg == Flag::CASTLE) {
        if (m.get_to() == Color::OO::KING_POST) {
            this->do_castle<Color, typename Color::OO >(); return;
        } else {
            this->do_castle<Color, typename Color::OOO>(); return;
        }
    }

    if (fg == Flag::EN_PASSANT) {
        this->remove_piece<Color>(m.get_to() + Color::BACKWARD, (Piece)Color::OPP_PAWN, (Piece)Color::OPP_ALL);
        this->do_quiet<Color>(m);
        return;
    }

    this->do_promo<Color>(m);
}