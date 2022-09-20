#pragma once

namespace Piece {
    enum Piece : int {
        W_PAWN = 0,
        W_KNIGHT = 1,
        W_BISHOP = 2,
        W_ROOK = 3,
        W_QUEEN = 4,
        W_KING = 5,
        B_PAWN = 6,
        B_KNIGHT = 7,
        B_BISHOP = 8,
        B_ROOK = 9,
        B_QUEEN = 10,
        B_KING = 11,
        WHITE = 12,
        BLACK = 13,
        OCCUP = 14,
        UNOCC = 15
    };

    char letters[] = {
        'P', 'N', 'B', 'R', 'Q', 'K',
        'p', 'n', 'b', 'r', 'q', 'k',
        'O', 'o'
    };
};

namespace MoveType { // note: color = turn
    enum Piece : U32 {
        W_PAWN = 0,
        W_KNIGHT = 1,
        W_BISHOP = 2,
        W_ROOK = 3,
        W_QUEEN = 4,
        W_KING = 5,
        B_PAWN = 6,
        B_KNIGHT = 7,
        B_BISHOP = 8,
        B_ROOK = 9,
        B_QUEEN = 10,
        B_KING = 11,

        WHITE_LEFT_CASTLE = 12,
        WHITE_RIGHT_CASTLE = 13,
        BLACK_LEFT_CASTLE = 14,
        BLACK_RIGHT_CASTLE = 15,

        KNIGHT_PROMO = 16,
        BISHOP_PROMO = 17,
        ROOK_PROMO = 18,
        QUEEN_PROMO = 19,

        GAMEOVER = 20
    };
};

namespace MoveCapture { // note: color = !turn
    enum Piece : int {
        W_PAWN = 0,
        W_KNIGHT = 1,
        W_BISHOP = 2,
        W_ROOK = 3,
        W_QUEEN = 4,
        W_KING = 5,
        B_PAWN = 6,
        B_KNIGHT = 7,
        B_BISHOP = 8,
        B_ROOK = 9,
        B_QUEEN = 10,
        B_KING = 11,

        NONE = 12
    };
};