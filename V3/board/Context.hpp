#pragma once

#include "Board.hpp"

struct Context {
    U64 moved;
    U64 hash; // zobrist hash
    Square en_passant;

    Context();
    Context(void* b_ptr, bool turn, bool castling_rights[4]);

    void toggle_hash_turn();
    void toggle_hash_piece(Piece, Square);
    void toggle_castling_rights(Square);

    template<class Color> void set_en_passant(Piece, Square from, Square to);

    template<class Castle> U64 moved_castling_pieces();

    void print() {
        std::cout << "moved:\n";
        printBB(moved);
        std::cout << "hash: " << std::hex << hash << '\n';
        std::cout << "en_passant: " << std::hex << en_passant << '\n';
    }
};