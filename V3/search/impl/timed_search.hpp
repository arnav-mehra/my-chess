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

    auto start_time = std::chrono::system_clock::now();
    constexpr double EXP_LATENCY = 0.1;
    constexpr double INC_USAGE = 0.8;
    constexpr double MOVE_OVERHEAD = 30.0;
    double target_time = (time_left / MOVE_OVERHEAD) + increment * INC_USAGE - EXP_LATENCY;
    auto end_time = start_time + std::chrono::duration<double>(target_time);

    MoveScore best = nega_max<Color>(b, ctx, 1, -INFINITY, INFINITY);

    for (int i = 2; i <= depth; i++) {
        auto curr_time = std::chrono::system_clock::now();
        std::chrono::duration<double> used_time = curr_time - start_time;
        if (used_time.count() >= target_time * 0.67) break; // if already used 2/3 of time, can't afford next depth, just return.

        // create async thread and wait up to max allowed time.
        std::future<MoveScore> fut = std::async(std::launch::async, nega_max<Color>, std::ref(b), std::ref(ctx), i, -INFINITY, INFINITY);
        std::future_status status = fut.wait_until(end_time);
        switch (status) {
            case std::future_status::timeout: {
                return best;
            }
            case std::future_status::ready: {
                best = fut.get();
            }
        }

        // print result
        std::cout << "info depth " << i << "\n";
        std::cout << "info score cp " << best.score * (turn ? 1 : -1) << "\n";
        std::cout << "info pv " << best.move.to_string() << "\n";
    }

    return best;
}