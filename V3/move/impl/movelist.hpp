#pragma once

#include "../MoveList.hpp"

MoveList::MoveList() {}

Move& MoveList::operator[](int i) {
    return move_list[i];
}

void MoveList::prioritize(Move& m) {
    constexpr U32 MASK = (1ULL << 12) - 1;

    int idx = -1;
    for (int i = 0; i < this->size(); i++) {
        if ((move_list[i].get_raw() & MASK) == (m.get_raw() & MASK)) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return;

    move_list[idx] = move_list[0];
    move_list[0] = m;
}

void MoveList::clear() {
    cnt = 0;
}

int& MoveList::size() {
    return cnt;
}

void MoveList::add(Move m) {
    move_list[cnt++] = m;
}

template<class Castle>
void MoveList::add_castle(bool can_castle) {
    const Move cmove = Move::make<Flag::CASTLE>(Castle::KING_PRE, Castle::KING_POST); 
    this->move_list[cnt] = cmove;
    cnt += can_castle;
}

template<class Color>
void MoveList::add_promos(Square from, Square to) {    
    this->add(Move::make<Flag::QUEEN_PROMO >(from, to));
    this->add(Move::make<Flag::ROOK_PROMO  >(from, to));
    this->add(Move::make<Flag::BISHOP_PROMO>(from, to));
    this->add(Move::make<Flag::KNIGHT_PROMO>(from, to));
}

void MoveList::print() {
    std::cout << "MOVES: " << cnt << '\n';
    for (int i = 0; i < cnt; i++) {
        std::cout << (i + 1) << ": ";
        move_list[i].print();
    }
}