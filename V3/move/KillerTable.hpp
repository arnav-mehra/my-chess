#pragma once

#include "Move.hpp"

#include <array>

// Move DS: [-----SC-----|CAPT|FLAG|-FROM-|--TO--]

constexpr bool USE_KILLER_TABLE = true;

namespace KillerTable {
    constexpr size_t SLOTS = 2;

    U64 hits = 0;
    U64 fill = 0;
    U64 misses = 0;
    U64 calls = 0;

    U32 table[MAX_DEPTH][SLOTS];

    bool has_move(Move& m, U16 depth) {
        if constexpr (!USE_KILLER_TABLE) return false;

        U32 target = m.get_masked();
        bool is_hit = false;
        for (int i = 0; i < SLOTS; i++) {
            is_hit |= (table[depth][i] == target);
        }

        hits += is_hit;
        misses += !is_hit;
        return is_hit;
    }

    bool has_quiet(Move& m, U16 depth) {
        return has_move(m, depth);
    }

    void add_move(Move& m, U16 depth) {
        if constexpr (!USE_KILLER_TABLE) return;

        if (m.get_capture() != Piece::NA) return;
        if (has_move(m, depth)) return;

        for (int i = SLOTS - 1; i >= 1; i--) {
            table[depth][i] = table[depth][i - 1];
        }
        table[depth][0] = m.get_masked();
    }
};