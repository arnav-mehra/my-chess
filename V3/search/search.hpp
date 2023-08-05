#pragma once

#include "../board/impl/index.hpp"
#include "evaluate.hpp"
#include <algorithm>

#define INFINITY INT_MAX

int max_depth = 0;

namespace Search {
    // MoveList mls[20];

    template<class Color>
    int quiscence(Board &b, int alpha, int beta) {
        constexpr bool turn = std::is_same<Color, White>::value;
        
        MoveList ml; b.gen_moves<Color, Gen::CAPTURES>(ml);
        ml.sort();
        
        int eval = -INFINITY;
        int legal_caps = 0;

        for (int i = 0; i < ml.size(); i++) {
            b.do_move<Color>(ml[i]);

            if (b.get_checks<Color>() == 0ULL) { // filter out illegal moves
                legal_caps++;

                int temp_eval = turn ? -quiscence<Black>(b, -beta, -alpha)
                                     : -quiscence<White>(b, -beta, -alpha);
                eval = std::max(eval, temp_eval);
                alpha = std::max(alpha, eval);
            }

            b.undo_move<Color>(ml[i]);
            if (alpha >= beta) break;
        }

        if (legal_caps == 0) {
            return turn ? Evaluate::piece_val(b) : -Evaluate::piece_val(b);
        }

        return eval;
    }

    template<class Color>
    int nega_max(Board &b, int depth, int alpha, int beta) {
        constexpr bool turn = std::is_same<Color, White>::value;

        if (depth == 0) {
            return turn ? Evaluate::piece_val(b) : -Evaluate::piece_val(b);
            // return turn ? -quiscence<Black>(b, -beta, -alpha)
                        // : -quiscence<White>(b, -beta, -alpha);
        }
        
        MoveList ml; b.gen_moves<Color, Gen::PSEUDOS>(ml);

        int eval = -INFINITY;

        for (int i = 0; i < ml.size(); i++) {
            b.do_move<Color>(ml[i]);

            if (b.get_checks<Color>() == 0ULL) { // filter out illegal moves
                int temp_eval = turn ? -nega_max<Black>(b, depth - 1, -beta, -alpha)
                                     : -nega_max<White>(b, depth - 1, -beta, -alpha);
                eval = std::max(eval, temp_eval);
                alpha = std::max(alpha, eval);
            }

            b.undo_move<Color>(ml[i]);
            if (alpha >= beta) break;
        }

        return eval;
    }

    template<class Color>
    std::pair<Move, int> search(Board &b, int depth) {
        constexpr bool turn = std::is_same<Color, White>::value;

        MoveList ml; b.gen_moves<Color, Gen::PSEUDOS>(ml);
        // ml.sort()

        Move move;
        int eval = -INFINITY;
        int alpha = -INFINITY;
        int beta = INFINITY;

        for (int i = 0; i < ml.size(); i++) {
            b.do_move<Color>(ml[i]);

            if (b.get_checks<Color>() == 0ULL) { // filter out illegal moves
                int temp_eval = turn ? -nega_max<Black>(b, depth - 1, -beta, -alpha)
                                     : -nega_max<White>(b, depth - 1, -beta, -alpha);
                if (temp_eval > eval) {
                    eval = temp_eval;
                    move = ml[i];
                }
                alpha = std::max(alpha, eval);
            }
            
            b.undo_move<Color>(ml[i]);
            if (alpha >= beta) break;
        }

        return { move, eval };
    }
};