#pragma once
#include "_move.hpp"

<<<<<<< HEAD
const U32 captureValTaken[6] = {
    10U + 190U, 30U + 190U, 33U + 190U, 56U + 190U, 95U + 190U, 200U + 190U
=======

int16_t const captureVal[6] = {100, 305, 333, 563, 950, 10000};
int16_t const quietVal[12] = {
    950, -10, -10, -10, //promo
    0, 0, 0, 0, 0, -20, //quiet
    7, 5 //castle
};
// int cap = 0;
// int pieceDist[6];



class Move { 

    public:
        
        uint8_t capture; uint8_t piece;
        uint8_t fromSq; uint8_t toSq;        
        int16_t priority;


        Move(uint8_t p, uint8_t f, uint8_t t, uint8_t c) {
            fromSq = f; toSq = t; piece = p; capture = c;
            priority = captureVal[c] - captureVal[p] + 10;

            // cap++; pieceDist[p]++;
        }
        Move(uint8_t p, uint8_t f, uint8_t t) {
            fromSq = f; toSq = t; piece = p; capture = 12;
            priority = quietVal[p+4];            
            
            // pieceDist[p]++;
        }
        Move(bool isCheckmate) {
            fromSq = 0; toSq = 0;
            capture = 0; piece = isCheckmate? 69: 70;
            priority = 0;
        }

        uint8_t getCapture() { return capture; }
        uint8_t getPiece() { return piece; }
        uint8_t getFromSq() { return fromSq; }
        uint8_t getToSq() { return toSq; }
        int8_t getPriority() { return priority; }
        inline uint64_t getFromVal() { return 1ULL << fromSq; }
        inline uint64_t getToVal() { return 1ULL << toSq; }
<<<<<<< Updated upstream
=======
>>>>>>> 0990fb1dff89ad5922db1d893123056578fe63de
>>>>>>> Stashed changes
};

const U32 captureValUsed[6] = {
    10U, 30U, 33U, 56U, 95U, 200U
};

const U32 quietVal[12] = {
    95 + 180, -1 + 180, -1 + 180, -1 + 180, // promo
    0 + 180, 0 + 180, 0 + 180, 0 + 180, 0 + 180, -2 + 180, // quiet
    7 + 180, 5 + 180 // castle
};

Move::Move() {}

<<<<<<< HEAD
inline Move::Move(MoveType::Piece p, MoveCapture::Piece c, U32 f, U32 t) {
    bitField = p;
    U32 priority = captureValTaken[c] - captureValUsed[p];
    bitField |= (priority << 21);
    bitField |= (c << 5);
    bitField |= (f << 9);
    bitField |= (t << 15);
}

inline Move::Move(MoveType::Piece p, U32 f, U32 t) {
    bitField = p;
    U32 priority = quietVal[p];
    bitField |= (priority << 21);
    bitField |= (MoveCapture::NONE << 5);
    bitField |= (f << 9);
    bitField |= (t << 15);
}

inline Move::Move(bool isCheckmate) {
    bitField = MoveType::GAMEOVER;
}

inline MoveType::Piece Move::getPiece() {
    return (MoveType::Piece) (bitField & 31U);
}

inline MoveCapture::Piece Move::getCapture() {
    return (MoveCapture::Piece) ((bitField >> 5) & 15U);
}

inline U32 Move::getFromSq() {
    return (bitField >> 9) & 63U;
}

inline U32 Move::getToSq() {
    return (bitField >> 15) & 63U;
}

inline U32 Move::getPriority() {
    return bitField >> 21;
}

inline U64 Move::getFromVal() {
    return 1ULL << getFromSq();
}

inline U64 Move::getToVal() {
    return 1ULL << getToSq();
}
=======
// capture dict and piece indexes:
    // 0: w pawns
    // 1: w knights
    // 2: w bishop
    // 3: w rook
    // 4: w queen
    // 5: w king
    // 6-11: b pieces
    // 12: no capture
<<<<<<< Updated upstream
=======
>>>>>>> 0990fb1dff89ad5922db1d893123056578fe63de
>>>>>>> Stashed changes
