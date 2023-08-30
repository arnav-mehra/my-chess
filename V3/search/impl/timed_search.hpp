#pragma once

#include "../search.hpp"

#include <chrono>
#include <thread>
#include <future>

template<class Color>
MoveScore Search::search(
    Board& b,
    Context& ctx,
    U16 depth,
    double time_left,
    double increment
) {
    constexpr bool turn = std::is_same<Color, White>::value;

    // estimate time to move.

    auto start_time = std::chrono::system_clock::now();
    constexpr double EXP_LATENCY = 0.1;
    constexpr double INC_USAGE = 0.8;
    constexpr double MOVE_OVERHEAD = 20.0;
    double target_time = (time_left / MOVE_OVERHEAD) + increment * INC_USAGE - EXP_LATENCY;
    std::cout << "info string search_time " << target_time << "\n";
    auto end_time = start_time + std::chrono::duration<double>(target_time);

    // initial search.

    Search::stop_search = false;
    MoveScore best = nega_max<Color>(b, ctx, 1, -INFINITY, INFINITY);

    // iterative deepening.

    constexpr I16 INIT_ASPIRATION = 300;
    I16 aspiration = INIT_ASPIRATION;

    for (int d = 2; d <= depth;) {
        if (best.score == INFINITY || best.score == -INFINITY) break;

        // Short circuit time: if already used 2/3 of time, can't afford next depth, just return current best to save time.

        auto curr_time = std::chrono::system_clock::now();
        std::chrono::duration<double> used_time = curr_time - start_time;
        if (used_time.count() >= target_time * 0.67) break;

        // Create async thread

        Search::stop_search = false;
        std::future<MoveScore> fut = std::async(
            std::launch::async,
            nega_max<Color>,
            std::ref(b),
            std::ref(ctx),
            d,
            best.score - aspiration,
            best.score + aspiration
        );
        std::future_status status = fut.wait_until(end_time);

        // Get status of thread after wait_until.

        switch (status) {
            case std::future_status::timeout: {
                Search::stop_search = true;
                return best;
            }
            case std::future_status::ready: {
                MoveScore new_best = fut.get();

                bool aspiration_failed = (
                    new_best.score <= best.score - aspiration
                    || new_best.score >= best.score + aspiration
                );

                if (aspiration_failed) {
                    best = new_best;
                    aspiration *= 8;
                    continue;
                }

                d++;
                aspiration = INIT_ASPIRATION / d;
                best = new_best;
            }
        }

        // print result

        std::cout << "info depth " << d << "\n";
        std::cout << "info score cp " << best.score * (turn ? 1 : -1) << "\n";
        std::cout << "info pv " << best.move.to_string() << "\n";
        
    }

    return best;
}