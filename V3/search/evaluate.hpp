#pragma once

#include "../board/impl/index.hpp"
#include "../init/pestos.hpp"

namespace Evaluate {
    I16 piece_val(Board& b) {
        int vals[12] = {
            10,  28,  33,  50,  90,  10000,
            -10, -28, -33, -50, -90, -10000
        };

        I16 eval = 0;
        for (int i = 0; i < 12; i++) {
            eval += pop_count(b.get_bitboard(i)) * vals[i];
        }
        return eval;
    }

    I16 pestos(Board& b) {
        return PeSTOs::eval(b);
    }
}