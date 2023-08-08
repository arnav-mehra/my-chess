#pragma once

#include "../util/types.hpp"
#include "../util/data.hpp"
#include <vector>

namespace MAPPED_MOVES {
    U64 KNIGHT_MOVES[NUM_SQUARES];
    U64 KING_MOVES[NUM_SQUARES];

    U64 BITS_BETWEEN[NUM_SQUARES][NUM_SQUARES];

    typedef std::vector<std::pair<int, int>> DIRS;

    DIRS KNIGHT_DIRECTIONS = {
        {-1, -2}, {-1, 2}, {1, -2}, {1, 2},
        {-2, -1}, {-2, 1}, {2, -1}, {2, 1},
    };
    DIRS KING_DIRECTIONS = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, 1}, {0, -1},
        {1, -1}, {1, 0}, {1, 1},
    };
    DIRS WHITE_PAWN_ATTACK_DIRECTIONS = {
        {-1, -1}, {-1, 1},
    };
    DIRS BLACK_PAWN_ATTACK_DIRECTIONS = {
        {1, -1}, {1, 1},
    };

    void init_mapped_piece_moves(U64 (&bb)[64], DIRS& dirs) {
        for (int sq = 0; sq < NUM_SQUARES; sq++) {
            U64& b = bb[sq]; b = 0;
            int r = sq >> 3, c = sq & 0b111;
            for (auto [ i, j ] : dirs) {
                int ri = r + i, cj = c + j;
                if (ri >= 0 && ri < 8 && cj >= 0 && cj < 8) {
                    int to_sq = (ri << 3) | cj;
                    b |= 1ULL << to_sq;
                }
            }
            // printBB(b); std::cout << '\n';
        }
    }

    template<class Color, class Castle>
    void init_mapped_castling_threats() {
        Castle::KNIGHT_RISKS = 0ULL, Castle::PAWN_RISKS = 0ULL;
        for (Square sq : Castle::NON_CHECKS) {
            Castle::KNIGHT_RISKS |= KNIGHT_MOVES[sq];
            Castle::PAWN_RISKS   |= Color::PAWN_ATTACKS[sq];
        }
    }

    void init_bits_between() {
        for (Square sq = 0; sq < NUM_SQUARES; sq++) {
            for (auto [ i, j ] : KING_DIRECTIONS) {
                U64 cum = 0;
                int r = (sq >> 3) + i;
                int c = (sq & 0b111) + j;
                while (r >= 0 && c >= 0 && c < 8 && r < 8) {
                    Square sq2 = (r << 3) | c;
                    r += i; c += j;
                    BITS_BETWEEN[sq][sq2] = cum; // cum is currently between sq & sq2.
                    cum |= 1ULL << sq2; // cum now includes sq2
                }
            }
        }
    }

    void init() {
        init_mapped_piece_moves(KNIGHT_MOVES, KNIGHT_DIRECTIONS);
        init_mapped_piece_moves(KING_MOVES, KING_DIRECTIONS);
        init_mapped_piece_moves(White::PAWN_ATTACKS, WHITE_PAWN_ATTACK_DIRECTIONS);
        init_mapped_piece_moves(Black::PAWN_ATTACKS, BLACK_PAWN_ATTACK_DIRECTIONS);
        
        init_mapped_castling_threats<White, White::OO>();
        init_mapped_castling_threats<White, White::OOO>();
        init_mapped_castling_threats<Black, Black::OO>();
        init_mapped_castling_threats<Black, Black::OOO>();

        init_bits_between();
    }

    U64 get_n_attacks(Square sq) {
        return KNIGHT_MOVES[sq];
    }

    U64 get_k_attacks(Square sq) {
        return KING_MOVES[sq];
    }
};