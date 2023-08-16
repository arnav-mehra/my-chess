#include "Move.hpp"

// Move DS: [-----SC-----|CAPT|FLAG|-FROM-|--TO--]

namespace KillerTable {
    constexpr size_t MAX_DEPTH = 40;
    constexpr size_t IDX_BITS  = 12;
    constexpr U32    IDX_MASK  = ((1ULL << IDX_BITS) - 1ULL);
    constexpr size_t TT_SIZE   = 1ULL << IDX_BITS;

    U64 hits = 0;
    U64 fill = 0;
    U64 misses = 0;

    bool table[MAX_DEPTH * TT_SIZE] = {};

    U32 get_index(Move& m, U16 depth) {
        U32 p1 = (U32)depth << IDX_BITS;
        U32 p2 = m.get_raw() & IDX_MASK;
        return p1 | p2;
    }

    void add_move(Move& m, U16 depth) {
        if (m.get_capture() != Piece::NA) return;

        U32 idx = get_index(m, depth);
        fill += !table[idx];
        table[idx] = 1;
    }

    U16 has_move(Move& m, U16 depth) {
        U16 is_quiet = m.get_capture() == Piece::NA;
        U32 idx = get_index(m, depth);

        U16 hit = is_quiet & table[idx];
        hits += hit;
        misses += !hit;
        return hit;
    }
};