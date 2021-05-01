#ifndef PIECE_H
#define PIECE_H


namespace Piece {
    
    enum Piece : unsigned int {
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

namespace MoveType {
    enum MoveType : unsigned int {
        PAWN = 0,
        KNIGHT = 1,
        BISHOP = 2,
        ROOK = 3,
        QUEEN = 4,
        KING = 5,
        LEFT_CASTLE = 6,
        RIGHT_CASTLE = 7,
        KNIGHT_PROMO = 8,
        BISHOP_PROMO = 9,
        ROOK_PROMO = 10,
        QUEEN_PROMO = 11,
        NONE = 12
    };
};


#endif