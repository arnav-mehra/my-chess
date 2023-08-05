#pragma once

#include "../board/impl/index.hpp"

namespace Evaluate {
    int piece_val(Board &b) {
        int vals[12] = {
            10,  28,  33,  50,  90,  10000,
            -10, -28, -33, -50, -90, -10000
        };

        int eval = 0;
        for (int i = 0; i < 12; i++) {
            eval += pop_count(b.get_bitboard(i)) * vals[i];
        }
        return eval;
    }
}