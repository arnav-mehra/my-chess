#pragma once
#include "Move.hpp"
#include <array>

class MoveList {
    std::array<Move, MAX_NUM_MOVES> move_list;
    int cnt = 0;

public:
    MoveList() {}

    void add(Move m) {
        move_list[cnt++] = m;
    }

    template<Flag flag>
    void add_move(Square from, Square to) {
        this->add(Move(flag, from, to));
    }

    void add_move(Flag flag, Square from, Square to) {
        this->add(Move(flag, from, to));
    }

    template<class Castle>
    void add_castle(bool can_castle) {
        this->move_list[cnt] = Move(Castle::FLAG, Castle::KING_PRE, Castle::KING_POST);
        cnt += can_castle;
    }

    void add_quiet(Square from, Square to) {
        this->add(Move(Flag::QUIET, from, to));
    }

    void add_capture(Square from, Square to) {
        this->add(Move(Flag::CAPTURE, from, to));
    }

    void add_pawn_double(Square to) {
        this->add(Move(Flag::PAWN_DOUBLE, 0, to));
    }
    
    void add_promos(Square to) {    
        this->add(Move(Flag::QUEEN_PROMO,  0, to));
        this->add(Move(Flag::ROOK_PROMO,   0, to));
        this->add(Move(Flag::BISHOP_PROMO, 0, to));
        this->add(Move(Flag::KNIGHT_PROMO, 0, to));
    }

    void add_promo_captures(Square from, Square to) {
        this->add(Move(Flag::QUEEN_PROMO_CAPTURE,  from, to));
        this->add(Move(Flag::ROOK_PROMO_CAPTURE,   from, to));
        this->add(Move(Flag::BISHOP_PROMO_CAPTURE, from, to));
        this->add(Move(Flag::KNIGHT_PROMO_CAPTURE, from, to));
    }

    void clear() {
        cnt = 0;
    }

    void sort() { // insertion sort
        for (int i = 1; i < cnt; i++) {
            Move subject = move_list[i]; // save value of subject
            int j;
            for (j = i - 1; j >= 0 && subject > move_list[j]; j--) {
                move_list[j + 1] = move_list[j];
            }
            move_list[j + 1] = subject; // insert subject
        }
    }

    int& size() {
        return cnt;
    }

    Move& operator[](int i) {
        return move_list[i];
    }

    void print() {
        std::cout << "MOVES: " << cnt << '\n';
        for (int i = 0; i < cnt; i++) {
            std::cout << (i + 1) << ": ";
            move_list[i].print();
        }
    }
};