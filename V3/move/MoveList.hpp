#pragma once

#include "Move.hpp"
#include <array>

class MoveList {
    std::array<Move, MAX_NUM_MOVES> move_list;
    int cnt = 0;

public:
    MoveList();

    Move& operator[](int i);
    void clear();
    int& size();

    void add(Move m);
    template<class Castle>
    void add_castle(bool can_castle);
    template<class Color>
    void add_promos(Square from, Square to);

    void fill_moves(void* b, U16 depth);

    void sort();
    void prioritize(Move& m);

    void print();
};