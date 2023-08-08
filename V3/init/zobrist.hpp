#pragma once

#include "../move/Move.hpp"
#include "../board/Board.hpp"

#define IMAX_BITS(m) ((m)/((m)%255+1) / 255%255*8 + 7-86/((m)%255+12))
#define RAND_MAX_WIDTH IMAX_BITS(RAND_MAX)

namespace ZOBRIST {
    U64 piece_rands[16][64];
    U64 turn_rand;

    U64 get_rand() {
        U64 x = 0;
        for (int i = 0; i < 64; i += RAND_MAX_WIDTH) {
            x <<= RAND_MAX_WIDTH;
            x ^= (unsigned) rand();
        }
        return x;
    }

    void init() {
        turn_rand = get_rand();
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < 64; j++) {
                piece_rands[i][j] = get_rand();
            }
        }
        for (int i = 12; i < 16; i++) {
            for (int j = 0; j < 64; j++) {
                piece_rands[i][j] = 0;
            }
        }
    }
};