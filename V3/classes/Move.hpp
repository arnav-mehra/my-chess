#pragma once
#include "../util/data.hpp"
#include "../util/types.hpp"
#include "../util/util.hpp"
#include <iostream>

class Move {
    U32 data; // [----SC----|CAPT|-FROM-|--TO--|FLAG]

public:
    Move() {}

    Move(Flag flag, Square from, Square to) {
        data = ((U32)from << 10)
             | ((U32)to << 4)
             | (U32)flag;
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