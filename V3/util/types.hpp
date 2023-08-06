#pragma once

typedef unsigned long long U64;
typedef unsigned int       U32;
typedef unsigned short     U16;
typedef unsigned char      U8;
typedef signed long long   I64;
typedef signed int         I32;
typedef signed short       I16;
typedef signed char        I8;

typedef U32 Square;

enum class Piece : Square { // 4 bits
    WHITE_PAWN = 0,
    WHITE_KNIGHT = 1,
    WHITE_BISHOP = 2,
    WHITE_ROOK = 3,
    WHITE_QUEEN = 4,
    WHITE_KING = 5,

    BLACK_PAWN = 6,
    BLACK_KNIGHT = 7,
    BLACK_BISHOP = 8,
    BLACK_ROOK = 9,
    BLACK_QUEEN = 10,
    BLACK_KING = 11,

    WHITE_ALL = 12,
    BLACK_ALL = 13,
    ALL = 14,
    GARBAGE = 15,

    NA = 15
};

enum class Flag : Square { // colors tested for
    QUIET,
    
    CAPTURE,

    EN_PASSANT,

    SHORT_CASTLE,
    LONG_CASTLE,

    KNIGHT_PROMO,
    BISHOP_PROMO,
    ROOK_PROMO,
    QUEEN_PROMO,

    KNIGHT_PROMO_CAPTURE,
    BISHOP_PROMO_CAPTURE,
    ROOK_PROMO_CAPTURE,
    QUEEN_PROMO_CAPTURE,
};

enum class Gen {
    PSEUDOS,
    LEGALS,
    EVASIONS,
    NON_EVASIONS,
    CAPTURES,
    QUIETS,

    BLOCKS
};