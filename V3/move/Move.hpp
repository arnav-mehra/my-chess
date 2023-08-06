#pragma once

#include "../util/data.hpp"
#include "../util/types.hpp"
#include "../util/util.hpp"
#include "../util/conversion.hpp"
#include <iostream>

U32 get_move_score(Flag flag, Piece pc, Piece capt) {
    const U32 QIT_MIN = 6;
    const U32 SPC_MIN = 12;
    const U32 CAP_MIN = 15;

    const U32 vals[16] = { 
        4, 26, 33, 50, 80, 200,
        4, 26, 33, 50, 80, 200
    };
    const U32 ords[16] = {
        1, 2, 3, 4, 5, 0,
        1, 2, 3, 4, 5, 0
    };

    const U32 CAP_ADJ = vals[(int)Piece::WHITE_KING] - vals[(int)Piece::WHITE_PAWN] + CAP_MIN;

    switch (flag) {
        // most likely to not be the best move (queen promo overshadows)
        case Flag::BISHOP_PROMO:         return 0;
        case Flag::ROOK_PROMO:           return 1;
        case Flag::KNIGHT_PROMO:         return 2;
        case Flag::BISHOP_PROMO_CAPTURE: return 3;
        case Flag::ROOK_PROMO_CAPTURE:   return 4;
        case Flag::KNIGHT_PROMO_CAPTURE: return 5;

        // baseline move
        case Flag::QUIET:        return QIT_MIN + ords[(int)pc];

        // normally slightly better than pure quiets
        case Flag::SHORT_CASTLE: return SPC_MIN + 1;
        case Flag::LONG_CASTLE:  return SPC_MIN + 2;

        // captures/promos are very likely to be the best moves. 
        case Flag::CAPTURE:             return CAP_ADJ + vals[(int)capt] - vals[(int)pc];
        case Flag::EN_PASSANT:          return CAP_ADJ + vals[(int)Piece::WHITE_PAWN]  - vals[(int)Piece::WHITE_PAWN];
        case Flag::QUEEN_PROMO:         return CAP_ADJ + vals[(int)Piece::WHITE_QUEEN] - vals[(int)Piece::WHITE_PAWN];
        case Flag::QUEEN_PROMO_CAPTURE: return CAP_ADJ + vals[(int)capt] + vals[(int)Piece::WHITE_QUEEN]  - 2 * vals[(int)Piece::WHITE_PAWN];
    }
    return 0;
}

template<Flag flag>
U32 get_move_score(Piece pc, Piece capt) {
    return get_move_score(flag, pc, capt);
}

class Move {
public:
    // score max = 1023
    U32 data; // [----SC----|CAPT|-FROM-|--TO--|FLAG]

    Move() {}

    Move(U32 data) {
        this->data = data;
    }

    Move(U32 score, Piece capt, Square from, Square to, Flag flag) {
        this->data = (
              ((U32)score << 20)
            | ((U32)capt << 16)
            | ((U32)flag << 12)
            | ((U32)from << 6)
            | ((U32)to)
        );
    }

    template<Flag flag>
    static Move make(Piece pc, Piece capt, Square from, Square to) {
        U32 score = get_move_score<flag>(pc, capt);
        return Move(score, capt, from, to, flag);
    }

    Piece  get_capture() { return (Piece)((data >> 16) & 0b1111  ); }
    Flag   get_flag()    { return (Flag )((data >> 12) & 0b1111  ); }
    Square get_from()    { return (U32  )((data >> 6)  & 0b111111); }
    Square get_to()      { return (U32  )((data >> 0)  & 0b111111); }
    U32    get_raw()     { return data; }

    U64 get_from_bit() { return 1ULL << this->get_from(); }
    U64 get_to_bit()   { return 1ULL << this->get_to();   }

    bool operator==(const Move &m) {
        return this->data == m.data;
    }

    bool operator>(const Move &m) {
        return this->data > m.data;
    }

    void print() {
        std::cout << "Move: "
                  << square_num_to_string((int)get_from())
                  << square_num_to_string((int)get_to())
                  << " (" << FLAG_NAMES[(int)get_flag()] 
                  << ")\n";
    }
};