#pragma once
#include "../util/data.hpp"
#include "../util/types.hpp"
#include "../util/util.hpp"
#include <iostream>

int get_move_score(Flag flag, Piece pc, Piece capt) {
    return 0;
    int vals[16] = { 1, 3, 5, 9, 20, 1, 3, 5, 9, 20 };
    switch (flag) {
        case Flag::QUIET:
        case Flag::EN_PASSANT:
        case Flag::PAWN_DOUBLE:
        case Flag::SHORT_CASTLE:
        case Flag::LONG_CASTLE:  return 32;

        case Flag::KNIGHT_PROMO: return 32 + vals[(int)Piece::WHITE_KNIGHT] - 1;
        case Flag::BISHOP_PROMO: return 32 + vals[(int)Piece::WHITE_BISHOP] - 1;
        case Flag::ROOK_PROMO:   return 32 + vals[(int)Piece::WHITE_ROOK] - 1;
        case Flag::QUEEN_PROMO:  return 32 + vals[(int)Piece::WHITE_QUEEN] - 1;

        case Flag::CAPTURE:  
        case Flag::KNIGHT_PROMO_CAPTURE:
        case Flag::BISHOP_PROMO_CAPTURE:
        case Flag::ROOK_PROMO_CAPTURE:
        case Flag::QUEEN_PROMO_CAPTURE: return 32 + vals[(int)capt] - vals[(int)pc];
    }
    return 32;
}

template<Flag flag>
int get_move_score(Piece pc, Piece capt) {
    return 0;
    return get_move_score(flag, pc, capt);
}

class Move {
    U32 data; // [----SC----|CAPT|-FROM-|--TO--|FLAG]

public:
    Move() {}

    Move(U32 data) {
        this->data = data;
    }

    template<Flag flag>
    static Move make(Piece pc, Piece capt, Square from, Square to) {
        U32 score = get_move_score<flag>(pc, capt);
        return Move(
              ((U32)score << 20)
            | ((U32)capt << 16)
            | ((U32)from << 10)
            | ((U32)to << 4)
            | ((U32)flag)
        );
    }

    Move(Flag flag, Piece pc, Piece capt, Square from, Square to) {
        U32 score = get_move_score(flag, pc, capt);
        this->data = (
              ((U32)score << 20)
            | ((U32)capt << 16)
            | ((U32)from << 10)
            | ((U32)to << 4)
            | ((U32)flag)
        );
    }

    Piece get_capture() {
        return (Piece)((data >> 16) & 0b1111);
    }

    void set_capture(Piece pc) {
        data |= ((U32)pc) << 16;
    }

    Square get_from() { 
        return (data >> 10) & 0b111111;
    }

    Square get_to() {
        return (data >> 4) & 0b111111;
    }

    U64 get_from_bit() {
        return 1ULL << this->get_from();
    }

    U64 get_to_bit() {
        return 1ULL << this->get_to();
    }

    Flag get_flag() {
        return (Flag)(data & 0b1111);
    }

    U32 get_raw() {
        return (U32)data;
    }

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