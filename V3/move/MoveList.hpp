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

    template<class Color, Gen Gn>
    void fill_moves(
        void* b,
        U16 depth = 0,
        Move priority = Move()
    );

    void sort();

    void print();
};