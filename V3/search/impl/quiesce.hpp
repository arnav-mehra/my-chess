#pragma once

#include "../search.hpp"

template<class Color>
I16 Search::quiesce(
    Board& b,
    Context& ctx,
    I16 alpha,
    I16 beta
) {
    constexpr bool turn = std::is_same<Color, White>::value;
    quiesce_nodes++;

    I16 eval = (I16)(turn ? 1 : -1) * Evaluate::pestos(b);
    
    if (eval >= beta) return beta;
    alpha = std::max(alpha, eval);

    MoveList ml;
    b.gen_order_moves<Color, GenType::CAPTURES>(ml, ctx);
    if (ml.size() == 0) return eval;

    for (int i = 0; i < ml.size(); i++) {
        // do move
        Move& move = ml[i];
        Context new_ctx = b.do_move<Color>(move, ctx);

        // filter out illegal moves
        if (b.get_checks<Color>()) { 
            b.undo_move<Color>(move);
            continue;
        }

        // evaluate after move
        I16 local_eval = turn ? -quiesce<Black>(b, new_ctx, -beta, -alpha)
                              : -quiesce<White>(b, new_ctx, -beta, -alpha);

        // undo move
        b.undo_move<Color>(move);

        // use new evaluation
        if (local_eval >= beta) return beta;
        alpha = std::max(alpha, local_eval);
    }

    return alpha;
}