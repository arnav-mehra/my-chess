#pragma once
#include "_move.hpp"
#include "_movelist.hpp"

MoveList::MoveList() {}

inline void MoveList::push(Move m) {
    list[n++] = m;
}

inline void MoveList::clear() {
    n = 0;
}

inline Move& MoveList::operator[](int i) {
    return list[i];
}

inline int MoveList::size() {
    return n;
}

inline void MoveList::sort() {
    std::sort(list, list + n, [](Move& move1, Move& move2) {
        return move1.getPriority() > move2.getPriority();
    });
}