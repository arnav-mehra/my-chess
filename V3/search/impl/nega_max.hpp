#pragma once

#include "../search.hpp"
#include "../../board/TranspositionTable.hpp"

template<class Color>
MoveScore Search::nega_max(
    Board& b,
    Context& ctx,
    U16 depth,
    I16 alpha,
    I16 beta
) {
    constexpr bool turn = std::is_same<Color, White>::value;
    const I16 og_alpha = alpha;
    nodes++;

    if (DrawTable::is_draw()) {
        return { Move(), 0 };
    }

    // TT-lookup: to adjust bounds and get priority move.

    auto [ tt_hit, tt_cell ] = TranspositionTable::get_cell(ctx.hash, depth);
    if (tt_hit && tt_cell->get_depth() >= depth) {
        switch (tt_cell->node_type) {
            case NodeType::EXACT: {
                return { tt_cell->move, tt_cell->score };
            }
            case NodeType::LOWER: alpha = std::max(alpha, tt_cell->score);
            case NodeType::UPPER: beta  = std::min(beta,  tt_cell->score);
        }
        if (alpha >= beta) {
            // KillerTable::add_move(tt_cell->move, depth);
            return { tt_cell->move, tt_cell->score };
        }
    }
    Move priority_move = tt_hit ? tt_cell->move : Move();

    // Quiescence: at nega_max leaf.

    if (depth == 0) {
        I16 score = quiesce<Color>(b, ctx, alpha, beta);
        return { Move(), score };
    }

    // Tests Moves.

    MoveList ml;
    b.gen_moves<Color, Gen::PSEUDOS>(ml, ctx, priority_move, depth);

    MoveScore best = { Move(), -INFINITY };
    I16 legal_move_count = 0;

    for (int i = 0; i < ml.size(); i++) {
        // do move
        Move& move = ml[i];
        Context new_ctx = b.do_move<Color>(move, ctx);

        // filter out illegal moves
        if (b.get_checks<Color>()) { 
            b.undo_move<Color>(move);
            continue;
        }
        legal_move_count++;

        // determine local evaluation
        MoveScore local_best = (
            turn ? nega_max<Black>(b, new_ctx, depth - 1, -beta, -alpha)
                 : nega_max<White>(b, new_ctx, depth - 1, -beta, -alpha)
        );
        local_best.score *= -1;

        // undo move
        b.undo_move<Color>(move);

        // use new evaluation
        if ((best.move.get_raw() == 0) | (local_best.score > best.score)) {
            best = { move, local_best.score };
        }
        alpha = std::max(alpha, best.score);
        if (alpha >= beta) {
            KillerTable::add_move(move, depth);
            break;
        }
    }

    // Checkmate or Stalemate.

    if (legal_move_count == 0) {
        I16 score = b.get_checks<Color>() ? -INFINITY : 0;
        best = { Move(), score };
    }

    // TT-update: with new result.

    TranspositionTable::set_cell(
        tt_cell, ctx.hash, depth,
        best, og_alpha, beta
    );

    return best;
}

template<class Color>
MoveScore Search::nega_scout(
    Board& b,
    Context& ctx,
    U16 depth,
    I16 alpha,
    I16 beta
) {
    constexpr bool turn = std::is_same<Color, White>::value;
    I16 og_alpha = alpha;
    I16 og_beta = beta;
    nodes++;

    if (DrawTable::is_draw()) {
        leaves++;
        return { Move(), 0 };
    }

    // TT-lookup: to adjust bounds and get priority move.

    auto [ tt_hit, tt_cell ] = TranspositionTable::get_cell(ctx.hash, depth);
    if (tt_hit && tt_cell->get_depth() >= depth) {
        switch (tt_cell->node_type) {
            case NodeType::EXACT: return { tt_cell->move, tt_cell->score };
            case NodeType::LOWER: alpha = std::max(alpha, tt_cell->score);
            case NodeType::UPPER: beta  = std::min(beta,  tt_cell->score);
        }
        if (alpha >= beta) return { tt_cell->move, tt_cell->score };
    }
    tt_hits += tt_hit;
    tt_misses += !tt_hit;
    Move priority_move = tt_hit ? tt_cell->move : Move();

    // Quiescence: at nega_max leaf.

    if (depth == 0) {
        leaves++;
        I16 score = quiesce<Color>(b, ctx, alpha, beta);
        return { Move(), score };
    }

    // Tests Moves.

    MoveList ml;
    b.gen_moves<Color, Gen::PSEUDOS>(ml, ctx, priority_move, depth);

    MoveScore best = { Move(), -INFINITY };
    I16 legal_move_count = 0;

    for (int i = 0; i < ml.size(); i++) {
        // do move
        Move& move = ml[i];
        Context new_ctx = b.do_move<Color>(move, ctx);

        // filter out illegal moves
        if (b.get_checks<Color>()) { 
            b.undo_move<Color>(move);
            continue;
        }
        legal_move_count++;

        // determine local evaluation
        MoveScore local_best = (
            turn ? nega_scout<Black>(b, new_ctx, depth - 1, -beta, -og_alpha)
                 : nega_scout<White>(b, new_ctx, depth - 1, -beta, -og_alpha)
        );
        local_best.score *= -1;

        if ((local_best.score > alpha) & (local_best.score < og_beta) & (i > 0)) {
            MoveScore local_best = (
                turn ? nega_scout<Black>(b, new_ctx, depth - 1, -og_beta, -og_alpha)
                    : nega_scout<White>(b, new_ctx, depth - 1, -og_beta, -og_alpha)
            );
            local_best.score *= -1;
        }

        // undo move
        b.undo_move<Color>(move);

        // use new evaluation
        if ((best.move.get_raw() == 0) | (local_best.score > best.score)) {
            best = { move, local_best.score };
        }
        og_alpha = std::max(og_alpha, best.score);
        if (og_alpha >= og_beta) break;
        beta = og_alpha + 1;
    }

    // Checkmate or Stalemate.

    if (legal_move_count == 0) {
        leaves++;
        I16 score = b.get_checks<Color>() ? -INFINITY : 0;
        best = { Move(), score };
    }

    // TT-update: with new result.

    TranspositionTable::set_cell(
        tt_cell, ctx.hash, depth,
        best, og_alpha, beta
    );

    return best;
}