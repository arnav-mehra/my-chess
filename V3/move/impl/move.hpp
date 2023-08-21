#pragma once

#include "../Move.hpp"
#include <string>

Move::Move() {
    this->data = 0;
}

Move::Move(U32 data) {
    this->data = data;
}

Move::Move(Square from, Square to, Flag flag) {
    this->data = (
          ((U32)flag << 12)
        | ((U32)from << 6)
        | ((U32)to)
    );
}

template<Flag flag>
Move Move::make(Square from, Square to) {
    return Move(from, to, flag);
}

Piece Move::get_capture() {
    return (Piece)((data >> 16) & 0b1111);
}

Flag Move::get_flag() {
    return (Flag)((data >> 12) & 0b1111);
}

Square Move::get_from() {
    return (U32)((data >> 6) & 0b111111);
}

Square Move::get_to() {
    return (U32)((data >> 0) & 0b111111);
}

U32 Move::get_raw() {
    return data;
}

U64 Move::get_from_bit() {
    return 1ULL << this->get_from();
}

U64 Move::get_to_bit() {
    return 1ULL << this->get_to();
}

bool Move::operator==(const Move& m) {
    return this->data == m.data;
}

bool Move::operator>(const Move& m) {
    return this->data > m.data;
}

std::string Move::to_string() {
    return square_num_to_string((int)get_from())
         + square_num_to_string((int)get_to());
}