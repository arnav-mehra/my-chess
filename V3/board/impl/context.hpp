#pragma once

#include "../Context.hpp"
#include "../Board.hpp"
#include "../../init/zobrist.hpp"

Context::Context(void* b_ptr, bool turn, bool castling_rights[4]) {
    this->en_passant = 0;

    this->hash = turn * ZOBRIST::turn_rand;

    U64 castling_state = (
          (1ULL << White::OO::ROOK_PRE ) * (1 - castling_rights[0])
        | (1ULL << White::OOO::ROOK_PRE) * (1 - castling_rights[1])
        | (1ULL << Black::OO::ROOK_PRE ) * (1 - castling_rights[2])
        | (1ULL << Black::OOO::ROOK_PRE) * (1 - castling_rights[3])
    );
    this->moved = castling_state;
    this->hash ^= castling_state;

    Board& b = *((Board*)b_ptr);
    for (int i = 0; i < 64; i++) {
        Piece pc = b.get_board(i);
        if (pc != Piece::NA) {
            this->hash ^= ZOBRIST::piece_rands[(int)pc][i];
        }
    }
}

Context::Context(Context& ctx) {
    this->moved = ctx.moved;
    this->hash = ctx.hash;
    this->en_passant = 0;
}

Context::Context(const Context& ctx) {
    this->moved = ctx.moved;
    this->hash = ctx.hash;
    this->en_passant = 0;
}

void Context::toggle_hash_turn() {
    hash ^= ZOBRIST::turn_rand;
}

void Context::toggle_hash_piece(Piece pc, Square sq) {
    hash ^= ZOBRIST::piece_rands[(int)pc][(int)sq];
}

void Context::toggle_castling_rights(Square sq) {
    constexpr U64 CASTLING_MASK = (
          (1ULL << White::OO ::ROOK_PRE)
        | (1ULL << White::OOO::ROOK_PRE)
        | (1ULL << White::OO ::KING_PRE)
        | (1ULL << Black::OO ::ROOK_PRE)
        | (1ULL << Black::OOO::ROOK_PRE)
        | (1ULL << Black::OO ::KING_PRE)
    );
    U64 prev = moved;
    moved |= (1ULL << (int)sq); // permanently mark sq as moved from
    U64 delta = (prev ^ moved) & CASTLING_MASK;
    hash ^= delta;
}

template<class Color>
void Context::set_en_passant(Piece pc, Square from, Square to) {
    bool is_pawn     = pc == (Piece)Color::PAWN;
    bool is_double   = from + Color::FORWARD + Color::FORWARD == to;
    this->en_passant = (is_pawn & is_double) * (from + Color::FORWARD);
}

template<class Castle>
bool Context::has_castling_rights() {
    constexpr U64 CASTLING_MASK = (
          (1ULL << (int)Castle::KING_PRE)
        | (1ULL << (int)Castle::ROOK_PRE)
    );
    return (CASTLING_MASK & moved) == 0ULL;
}