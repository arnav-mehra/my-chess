#pragma once

#include "types.hpp"
#include <string>

#define NUM_SQUARES 64
#define NUM_BITBOARDS 16
#define MAX_NUM_MOVES 216
#define MAX_DEPTH 40

U64 NON_LEFT_PIECES = 0xfefefefefefefefe;
U64 NON_RIGHT_PIECES = 0x7f7f7f7f7f7f7f7f;

namespace BB_SETS {
    constexpr U64 FILE[8] = {
        0x0101010101010101ULL,
        0x0202020202020202ULL,
        0x0404040404040404ULL,
        0x0808080808080808ULL,
        0x1010101010101010ULL,
        0x2020202020202020ULL,
        0x4040404040404040ULL,
        0x8080808080808080ULL
    };

    constexpr U64 RANK[8] = {
        0xFFULL << (7 * 8),
        0xFFULL << (6 * 8),
        0xFFULL << (5 * 8),
        0xFFULL << (4 * 8),
        0xFFULL << (3 * 8),
        0xFFULL << (2 * 8),
        0xFFULL << (1 * 8),
        0xFFULL << (0 * 8),
    };

    // [(7 - rank) + file]
    U64 F_DIAGONAL[15] = {
        0x1ULL,
        0x102ULL,
        0x10204ULL,
        0x1020408ULL,
        0x102040810ULL,
        0x10204081020ULL,
        0x1020408102040ULL,
        0x102040810204080ULL,
        0x204081020408000ULL,
        0x408102040800000ULL,
        0x810204080000000ULL,
        0x1020408000000000ULL,
        0x2040800000000000ULL,
        0x4080000000000000ULL,
        0x8000000000000000ULL
    };

    // [(7 - rank) - file + 7]
    U64 B_DIAGONAL[15] = {
        0x80ULL,
        0x8040ULL,
        0x804020ULL,
        0x80402010ULL,
        0x8040201008ULL,
        0x804020100804ULL,
        0x80402010080402ULL,
        0x8040201008040201ULL,
        0x4020100804020100ULL,
        0x2010080402010000ULL,
        0x1008040201000000ULL,
        0x804020100000000ULL,
        0x402010000000000ULL,
        0x201000000000000ULL,
        0x100000000000000ULL
    };
};

class White { public:
    static U64 PAWN_ATTACKS[64];

    class OO { public:
        static Square NON_CHECKS[3];
        static U64 KNIGHT_RISKS;
        static U64 PAWN_RISKS;
        static U64 KING_RISKS;

        enum Squares : Square {
            KING_PRE  = 60,
            ROOK_POST = 61,
            KING_POST = 62,
            ROOK_PRE  = 63,
        };

        enum Bitboards : U64 {
            KING_DELTA = (1ULL << KING_PRE) | (1ULL << KING_POST),
            ROOK_DELTA = (1ULL << ROOK_PRE) | (1ULL << ROOK_POST),
            DELTA      = KING_DELTA | ROOK_DELTA,
            EMPTY_MASK = (1ULL << ROOK_POST) | (1ULL << KING_POST),
        };
    };

    class OOO { public:
        static Square NON_CHECKS[3];
        static U64 KNIGHT_RISKS;
        static U64 PAWN_RISKS;
        static U64 KING_RISKS;

        enum Squares : Square {
            ROOK_PRE  = 56,
            UNUSED    = 57,
            KING_POST = 58,
            ROOK_POST = 59,
            KING_PRE  = 60,
        };

        enum Bitboards : U64 {
            KING_DELTA = (1ULL << KING_PRE) | (1ULL << KING_POST),
            ROOK_DELTA = (1ULL << ROOK_PRE) | (1ULL << ROOK_POST),
            DELTA      = KING_DELTA | ROOK_DELTA,
            EMPTY_MASK = (1ULL << ROOK_POST) | (1ULL << KING_POST) | (1ULL << UNUSED),
        };
    };

    enum Pieces {
        PAWN   = (int) Piece::WHITE_PAWN,
        KNIGHT = (int) Piece::WHITE_KNIGHT,
        BISHOP = (int) Piece::WHITE_BISHOP,
        ROOK   = (int) Piece::WHITE_ROOK,
        QUEEN  = (int) Piece::WHITE_QUEEN,
        KING   = (int) Piece::WHITE_KING,
        ALL    = (int) Piece::WHITE_ALL,

        OPP_PAWN   = (int) Piece::BLACK_PAWN,
        OPP_KNIGHT = (int) Piece::BLACK_KNIGHT,
        OPP_BISHOP = (int) Piece::BLACK_BISHOP,
        OPP_ROOK   = (int) Piece::BLACK_ROOK,
        OPP_QUEEN  = (int) Piece::BLACK_QUEEN,
        OPP_KING   = (int) Piece::BLACK_KING,
        OPP_ALL    = (int) Piece::BLACK_ALL
    };

    enum Directions {
        FORWARD = -8,
        BACKWARD = 8,
        FORWARD_LEFT = -9,
        FORWARD_RIGHT = -7,
    };

    enum Squares : Square {
        KING_INITIAL_SQUARE = 60,
    };

    enum Bitboards : U64 {
        FINAL_RANK = 0xFFULL,
        PAWN_FINAL_RANK = 0xFFULL << 8,
        PAWN_DOUBLE_RANK = (0xFFULL << 32),
    };
};

class Black { public:
    static U64 PAWN_ATTACKS[64];

    class OO { public:
        static Square NON_CHECKS[3];
        static U64 KNIGHT_RISKS;
        static U64 PAWN_RISKS;
        static U64 KING_RISKS;

        enum Squares : Square {
            KING_PRE  = 04,
            ROOK_POST = 05,
            KING_POST = 06,
            ROOK_PRE  = 07,
        };

        enum Bitboards : U64 {
            KING_DELTA = (1ULL << KING_PRE) | (1ULL << KING_POST),
            ROOK_DELTA = (1ULL << ROOK_PRE) | (1ULL << ROOK_POST),
            DELTA      = KING_DELTA | ROOK_DELTA,
            EMPTY_MASK = (1ULL << ROOK_POST) | (1ULL << KING_POST),
        };
    };

    class OOO { public:
        static Square NON_CHECKS[3];
        static U64 KNIGHT_RISKS;
        static U64 PAWN_RISKS;
        static U64 KING_RISKS;

        enum Squares : Square {
            ROOK_PRE  = 00,
            UNUSED    = 01,
            KING_POST = 02,
            ROOK_POST = 03,
            KING_PRE  = 04,
        };

        enum Bitboards : U64 {
            KING_DELTA = (1ULL << KING_PRE) | (1ULL << KING_POST),
            ROOK_DELTA = (1ULL << ROOK_PRE) | (1ULL << ROOK_POST),
            DELTA      = KING_DELTA | ROOK_DELTA,
            EMPTY_MASK = (1ULL << ROOK_POST) | (1ULL << KING_POST) | (1ULL << UNUSED),
        };
    };

    enum Pieces {
        PAWN   = (int) Piece::BLACK_PAWN,
        KNIGHT = (int) Piece::BLACK_KNIGHT,
        BISHOP = (int) Piece::BLACK_BISHOP,
        ROOK   = (int) Piece::BLACK_ROOK,
        QUEEN  = (int) Piece::BLACK_QUEEN,
        KING   = (int) Piece::BLACK_KING,
        ALL    = (int) Piece::BLACK_ALL,

        OPP_PAWN   = (int) Piece::WHITE_PAWN,
        OPP_KNIGHT = (int) Piece::WHITE_KNIGHT,
        OPP_BISHOP = (int) Piece::WHITE_BISHOP,
        OPP_ROOK   = (int) Piece::WHITE_ROOK,
        OPP_QUEEN  = (int) Piece::WHITE_QUEEN,
        OPP_KING   = (int) Piece::WHITE_KING,
        OPP_ALL    = (int) Piece::WHITE_ALL,
    };

    enum Directions {
        FORWARD = 8,
        BACKWARD = -8,
        FORWARD_LEFT = 7,
        FORWARD_RIGHT = 9,
    };

    enum Squares : Square {
        KING_INITIAL_SQUARE = 04,
    };

    enum Bitboards : U64 {
        FINAL_RANK = 0xFFULL << 56,
        PAWN_FINAL_RANK = 0xFFULL << 48,
        PAWN_DOUBLE_RANK = (0xFFULL << 24),
    };
};

// init static members...

U64 White::PAWN_ATTACKS[64];
U64 Black::PAWN_ATTACKS[64];

Square White::OO::NON_CHECKS[3] = { 60, 61, 62 };
U64 White::OO::KNIGHT_RISKS;
U64 White::OO::PAWN_RISKS;
U64 White::OO::KING_RISKS;
Square White::OOO::NON_CHECKS[3] = { 60, 59, 58 };
U64 White::OOO::KNIGHT_RISKS;
U64 White::OOO::PAWN_RISKS;
U64 White::OOO::KING_RISKS;

Square Black::OO::NON_CHECKS[3] = { 4, 5, 6 };
U64 Black::OO::KNIGHT_RISKS;
U64 Black::OO::PAWN_RISKS;
U64 Black::OO::KING_RISKS;
Square Black::OOO::NON_CHECKS[3] = { 2, 3, 4 };
U64 Black::OOO::KNIGHT_RISKS;
U64 Black::OOO::PAWN_RISKS;
U64 Black::OOO::KING_RISKS;