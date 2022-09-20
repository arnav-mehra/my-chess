#pragma once

class MoveList {
private:
    int n = 0;
    Move list[218];
public:
    MoveList();
    inline void push(Move m);
    inline void clear();
    inline Move& operator[](int i);
    inline int size();
    inline void sort();
};