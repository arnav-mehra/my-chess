#pragma once
#include "../Board.hpp"

template<class Color>
void Board::do_quiet(Move& m) {
    this->from_hist[m.get_from()]++;
    
    Piece piece = this->board[m.get_from()];
    this->board[(int)m.get_from()] = Piece::GARBAGE;
    this->board[(int)m.get_to()]   = piece;

    U64 bit_diff = m.get_from_bit() | m.get_to_bit();
    this->bitboards[(int)piece]      ^= bit_diff;
    this->bitboards[(int)Color::ALL] ^= bit_diff;
    this->bitboards[(int)Piece::ALL] ^= bit_diff;

    this->en_passant = 0;
}

template<class Color>
void Board::do_pawn_double(Move& m) {
    Square to = m.get_to();
    this->en_passant = to + Color::BACKWARD;
    Square from = this->en_passant + Color::BACKWARD;

    this->board[(int)from] = Piece::GARBAGE;
    this->board[(int)to]   = (Piece)Color::PAWN;

    U64 bit_diff = (1ULL << from) | (1ULL << to);        
    this->bitboards[(int)Color::PAWN] ^= bit_diff;
    this->bitboards[(int)Color::ALL]  ^= bit_diff;
    this->bitboards[(int)Piece::ALL]  ^= bit_diff;
}

template<class Color>
void Board::do_capture(Move& m) {
    this->from_hist[(int)m.get_from()]++;

    Piece piece = this->board[m.get_from()];
    Piece capture = this->board[m.get_to()];
    m.set_capture(capture);
    this->board[(int)m.get_from()] = Piece::GARBAGE;
    this->board[(int)m.get_to()]   = piece;

    U64 from_bit = m.get_from_bit();
    U64 to_bit   = m.get_to_bit();
    // remove captured piece.
    this->bitboards[(int)capture]        ^= to_bit;
    this->bitboards[(int)Color::OPP_ALL] ^= to_bit;
    // move capturing piece.
    U64 bit_diff = from_bit | to_bit;
    this->bitboards[(int)piece]      ^= bit_diff;
    this->bitboards[(int)Color::ALL] ^= bit_diff;
    // overall change.
    this->bitboards[(int)Piece::ALL] ^= from_bit;

    this->en_passant = 0;
}

template<class Color, class Castle>
void Board::do_castle() {
    this->from_hist[(int)Castle::KING_PRE]++;

    this->board[(int)Castle::KING_PRE]  = Piece::GARBAGE;
    this->board[(int)Castle::ROOK_POST] = (Piece)Color::ROOK;
    this->board[(int)Castle::KING_POST] = (Piece)Color::KING;
    this->board[(int)Castle::ROOK_PRE]  = Piece::GARBAGE;
    
    this->bitboards[(int)Color::KING] ^= Castle::KING_DELTA;
    this->bitboards[(int)Color::ROOK] ^= Castle::ROOK_DELTA;
    this->bitboards[(int)Color::ALL]  ^= Castle::DELTA;
    this->bitboards[(int)Piece::ALL]  ^= Castle::DELTA;

    this->en_passant = 0;
}

template<class Color, Piece promo_piece>
void Board::do_promo(Move& m) {
    Square from = m.get_to() + Color::BACKWARD;

    this->board[from] = Piece::GARBAGE;
    this->board[(int)m.get_to()] = promo_piece;

    U64 from_bit = 1ULL << from;
    U64 to_bit   = m.get_to_bit();
    this->bitboards[(int)Color::PAWN] ^= from_bit;
    this->bitboards[(int)promo_piece] ^= to_bit;

    U64 bit_diff = from_bit | to_bit;
    this->bitboards[(int)Color::ALL] ^= bit_diff;
    this->bitboards[(int)Piece::ALL] ^= bit_diff;
    
    this->en_passant = 0;
}

template<class Color, Piece promo_piece>
void Board::do_promo_capture(Move& m) {
    Piece capture = this->board[m.get_to()];
    m.set_capture(capture);
    this->board[(int)m.get_from()] = Piece::GARBAGE;
    this->board[(int)m.get_to()] = promo_piece;

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
    this->board[(int)m.get_from()]  = Piece::GARBAGE;
    this->board[(int)m.get_to()]    = (Piece)Color::PAWN;
    this->board[(int)en_passant_sq] = Piece::GARBAGE;

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
    switch (m.get_flag()) {
        case Flag::QUIET: this->do_quiet<Color>(m); return;

        case Flag::CAPTURE: this->do_capture<Color>(m); return;

        case Flag::PAWN_DOUBLE: this->do_pawn_double<Color>(m); return;

        case Flag::SHORT_CASTLE: this->do_castle<Color, typename Color::OO>(); return;
        case Flag::LONG_CASTLE:  this->do_castle<Color, typename Color::OOO>(); return;

        case Flag::KNIGHT_PROMO: this->do_promo<Color, (Piece)Color::KNIGHT>(m); return;
        case Flag::BISHOP_PROMO: this->do_promo<Color, (Piece)Color::BISHOP>(m); return;
        case Flag::ROOK_PROMO:   this->do_promo<Color, (Piece)Color::ROOK>(m); return;
        case Flag::QUEEN_PROMO:  this->do_promo<Color, (Piece)Color::QUEEN>(m); return;

        case Flag::KNIGHT_PROMO_CAPTURE: this->do_promo_capture<Color, (Piece)Color::KNIGHT>(m); return;
        case Flag::BISHOP_PROMO_CAPTURE: this->do_promo_capture<Color, (Piece)Color::BISHOP>(m); return;
        case Flag::ROOK_PROMO_CAPTURE:   this->do_promo_capture<Color, (Piece)Color::ROOK>(m); return;
        case Flag::QUEEN_PROMO_CAPTURE:  this->do_promo_capture<Color, (Piece)Color::QUEEN>(m); return;

        case Flag::EN_PASSANT: this->do_en_passant<Color>(m); return;
    }
}