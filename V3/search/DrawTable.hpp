#pragma once

#include "../util/types.hpp"
#include "../move/Move.hpp"
#include "../board/Context.hpp"

#include <array>
#include <iostream>

// NOTES: 
//  - this is a very naive implementation of a draw table.
//  - castles are part of the 50-rule, but currently aren't.

namespace DrawTable {

    struct Entry {
        U32 is_reversable;
        U32 sub_hash;
    };

    std::array<Entry, 1024> history;
    int cnt = 0;
    int last_non_reversable = 0;

    // add position, then return if a draw is detected.

    void push_position(Move& last_move, U64& hash) {
        U32 sub_hash = (U32)hash;
        U32 is_reversable = last_move.get_reversable();

        if (!is_reversable) {
            last_non_reversable = cnt;
        }

        history[cnt++] = { is_reversable, sub_hash };
    }

    void pop_position() {
        cnt--;
        // item removed was the last non-reversable move, find the new one.
        if (cnt == last_non_reversable) {
            last_non_reversable--;
            while (history[last_non_reversable].is_reversable) {
                last_non_reversable--;
            }
        }
    }

    // get position repeats and consecutive reversable move counts.
    std::pair<int, int> get_rule_stats() {
        int reps = 1;
        U32 target_sub_hash = history[cnt - 1].sub_hash;

        for (int ptr = cnt - 2; ptr >= last_non_reversable; ptr--) {
            reps += (history[ptr].sub_hash == target_sub_hash);
        }

        int reversable_cnt = cnt - 1 - last_non_reversable;

        return { reps, reversable_cnt };
    }

    // check if the last move caused a 3-fold or 50-move draw
    bool is_draw() {
        auto [ reps, reversable_cnt ] = get_rule_stats();
        bool is_fifty_move = reversable_cnt >= 100;
        bool is_three_fold = reps >= 3;
        return is_three_fold | is_fifty_move;
    }

    void clear(Context& init_ctx) {
        U32 sub_hash = (U32)init_ctx.hash;
        history[0] = { false, sub_hash };
        last_non_reversable = 0;
        cnt = 1;
    }

    void print() {
        for (int i = 0; i < cnt; i++) {
            std::cout << i << ": "
                      << history[i].is_reversable << " "
                      << std::hex << history[i].sub_hash << std::dec << "\n";
        }
        auto [ reps, reversable_cnt ] = get_rule_stats();
        std::cout << "reps: " << reps << "\n";
        std::cout << "last_non_reversable: " << last_non_reversable << '\n';
        std::cout << "reversable_cnt: " << reversable_cnt << "\n";
        std::cout << "is_draw(): " << is_draw() << "\n";
    }
};