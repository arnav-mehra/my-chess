#pragma once

#include "../board/impl/index.hpp"
#include "../move/impl/index.hpp"
#include "evaluate.hpp"
#include <algorithm>

#define INFINITY INT_MAX

namespace Search {
    Move best_move;
    U64 branches = 0;
    U64 nodes = 0;

    // template<class Color>
    // int quiscence(Board &b, int alpha, int beta) {
    //     constexpr bool turn = std::is_same<Color, White>::value;
        
    //     MoveList ml; b.gen_moves<Color, Gen::CAPTURES>(ml);
    //     ml.sort();
        
    //     int eval = -INFINITY;
    //     int legal_caps = 0;

    //     for (int i = 0; i < ml.size(); i++) {
    //         b.do_move<Color>(ml[i], ctx);

    //         if (b.get_checks<Color>() == 0ULL) { // filter out illegal moves
    //             legal_caps++;

    //             int temp_eval = turn ? -quiscence<Black>(b, -beta, -alpha)
    //                                  : -quiscence<White>(b, -beta, -alpha);
    //             eval = std::max(eval, temp_eval);
    //             alpha = std::max(alpha, eval);
    //         }

    //         b.undo_move<Color>(ml[i]);
    //         if (alpha >= beta) break;
    //     }

    //     if (legal_caps == 0) {
    //         return turn ? Evaluate::piece_val(b) : -Evaluate::piece_val(b);
    //     }

    //     return eval;
    // }

    template<class Color, bool IsRoot = true>
    int search(
        Board &b,
        Context &ctx,
        int depth,
        int alpha = -INFINITY,
        int beta = INFINITY
    ) {
        constexpr bool turn = std::is_same<Color, White>::value;

        if (depth == 0) {
            return (turn ? 1 : -1) * Evaluate::piece_val(b);
        }

        MoveList ml; b.gen_moves<Color, Gen::PSEUDOS>(ml, ctx);
        ml.sort();

        int best_score = -INFINITY;
        int legal_move_count = 0;

        for (int i = 0; i < ml.size(); i++) {
            Move& move = ml[i];

            Context new_ctx = ctx;
            b.do_move<Color>(move, new_ctx);

            if (b.get_checks<Color>()) { // filter out illegal moves
                b.undo_move<Color>(move);
                continue;
            }

            legal_move_count++;

            int score = turn ? -search<Black, false>(b, new_ctx, depth - 1, -beta, -alpha)
                             : -search<White, false>(b, new_ctx, depth - 1, -beta, -alpha);

            b.undo_move<Color>(move);

            if (score > best_score) {
                if constexpr (IsRoot) best_move = move;
                best_score = score;
            }
            alpha = std::max(alpha, score);
            if (alpha >= beta) break;
        }

        if (legal_move_count > 0) {
            nodes++;
            branches += legal_move_count;
        }

        if constexpr (IsRoot) { // invert to normal for root call.
            return (turn ? 1 : - 1) * best_score;
        }
        return best_score;
    }
};