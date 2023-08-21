#pragma once

#include "../board/impl/index.hpp"
#include "../move/impl/index.hpp"
#include "evaluate.hpp"
#include "../board/TranspositionTable.hpp"
#include <algorithm>
#include <unordered_set>

#define INFINITY INT16_MAX

namespace Search {

    // stats

    U64 branches = 0;
    U64 nodes = 0;
    
    U64 tt_hits = 0;
    U64 tt_partials = 0;
    U64 tt_misses = 0;

    // mini-max search

    template<class Color>
    I16 quiesce(
        Board& b,
        Context& ctx,
        I16 alpha,
        I16 beta
    ) {
        constexpr bool turn = std::is_same<Color, White>::value;
        
        I16 stand_pat = (turn ? 1 : -1) * Evaluate::pestos(b);
        if (stand_pat >= beta) return beta;
        if (alpha < stand_pat) alpha = stand_pat;

        MoveList ml;
        b.gen_moves<Color, Gen::CAPTURES>(ml, ctx);
        ml.fill_moves(&b, 0);
        ml.sort();

        nodes++;
        branches += ml.size();

        for (int i = 0; i < ml.size(); i++) {
            Move& move = ml[i];
            Context new_ctx = ctx;
            b.do_move<Color>(move, new_ctx);

            if (b.get_checks<Color>()) { // filter out illegal moves
                b.undo_move<Color>(move);
                continue;
            }

            I16 score = turn ? -quiesce<Black>(b, new_ctx, -beta, -alpha)
                             : -quiesce<White>(b, new_ctx, -beta, -alpha);
            b.undo_move<Color>(move);

            if (score >= beta) return beta;
            alpha = std::max(alpha, score);
        }

        return alpha;
    }

    template<class Color>
    inline EvalMove nega_max_eval(
        Board& b,
        Context& new_ctx,
        U16 depth,
        I16 alpha,
        I16 beta
    );

    template<class Color, bool IsRoot = true>
    EvalMove nega_max(
        Board& b,
        Context& ctx,
        U16 depth,
        Move order_first,
        I16 alpha = -INFINITY,
        I16 beta = INFINITY
    ) {
        constexpr bool turn = std::is_same<Color, White>::value;

        if (depth == 0) {
            return { quiesce<Color>(b, ctx, alpha, beta), Move() };
        }

        MoveList ml;
        b.gen_moves<Color, Gen::PSEUDOS>(ml, ctx);
        ml.fill_moves(&b, depth);
        ml.sort();
        if (order_first.get_raw() != 0) {
            ml.prioritize(order_first);
        }

        EvalMove best = { -INFINITY, Move() };
        I16 legal_move_count = 0;

        for (int i = 0; i < ml.size(); i++) {
            // do move
            Move& move = ml[i];
            Context new_ctx = ctx;
            b.do_move<Color>(move, new_ctx);

            // filter out illegal moves
            if (b.get_checks<Color>()) { 
                b.undo_move<Color>(move);
                continue;
            }
            legal_move_count++;

            // evaluate after move
            EvalMove local_best = nega_max_eval<Color>(b, new_ctx, depth, alpha, beta);

            // undo move
            b.undo_move<Color>(move);

            // use new evaluation
            if (local_best.eval > best.eval) {
                best = { local_best.eval, move };
            }
            alpha = std::max(alpha, local_best.eval);
            if (alpha >= beta) {
                KillerTable::add_move(move, depth);
                break;
            }
        }

        if (legal_move_count == 0) {
            return { 0, Move() };
        } else {
            nodes++;
            branches += legal_move_count;
        }

        if constexpr (IsRoot) { // invert to normal for root call.
            best.eval *= (turn ? 1 : - 1);
        }
        return best;
    }

    template<class Color>
    inline EvalMove nega_max_eval(
        Board& b,
        Context& new_ctx,
        U16 depth,
        I16 alpha,
        I16 beta
    ) {
        constexpr bool turn = std::is_same<Color, White>::value;
        // constexpr auto fn = turn ? nega_max<Black, false> : nega_max<White, false>;

        U64* cell = TranspositionTable::get_cell(new_ctx.hash, depth);

        // TT hit: use eval and/or move of cached result.
        if (cell) {
            if (TranspositionTable::get_depth(cell) >= depth) {
                tt_hits++;

                return TranspositionTable::get_eval_move(cell);
            }
            else {
                tt_partials++;

                Move next_order_first = TranspositionTable::get_move(cell);
                EvalMove local_best = (
                    turn ? nega_max<Black, false>(b, new_ctx, depth - 1, next_order_first, -beta, -alpha)
                         : nega_max<White, false>(b, new_ctx, depth - 1, next_order_first, -beta, -alpha)
                );
                local_best.eval *= -1;

                TranspositionTable::get_eval(cell) = local_best.eval;
                TranspositionTable::get_move(cell) = local_best.move;
                return local_best;
            }
        }

        // TT miss: compute fully and set.
        tt_misses++;

        EvalMove local_best = (
            turn ? nega_max<Black, false>(b, new_ctx, depth - 1, Move(), -beta, -alpha)
                 : nega_max<White, false>(b, new_ctx, depth - 1, Move(), -beta, -alpha)
        );
        local_best.eval *= -1;

        TranspositionTable::set_cell(new_ctx.hash, depth, local_best.move, local_best.eval, 0);
        return local_best;
    }

    template<class Color>
    EvalMove search(
        Board& b,
        Context& ctx,
        U16 depth
    ) {
        double t = 0;
        EvalMove best = { -INFINITY, Move() };

        for (int i = 1; i <= depth; i++) {
            auto start = std::chrono::system_clock::now();
            best = nega_max<Color>(b, ctx, i, best.move);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            t += elapsed_seconds.count();

            // std::cout << "D: " << i << "\t"
            //           << "t: " << elapsed_seconds.count() << "\t"
            //           << "T: " << t << '\n';
            if (t > 0.2) {
                // std::cout << "\nDEPTH: " << i << "/" << depth << '\n';
                break;
            }
        }

        return best;
    }

};