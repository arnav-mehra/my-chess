#pragma once

#include "Board.hpp"

struct Context {
    U64 moved;
    U64 hash; // zobrist hash
    Square en_passant;

    Context();
    Context(Context&);
    Context(const Context&);
    Context(void* b_ptr, bool turn, bool castling_rights[4]);

    void toggle_hash_turn();
    void toggle_hash_piece(Piece, Square);
    void toggle_castling_rights(Square);

    template<class Color> void set_en_passant(Piece, Square from, Square to);

    template<class Castle> bool has_castling_rights();
};