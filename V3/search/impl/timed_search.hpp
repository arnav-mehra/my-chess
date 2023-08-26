#pragma once

#include "../search.hpp"

#include <chrono>

template<class Color>
MoveScore Search::search(
    Board& b,
    Context& ctx,
    U16 depth
) {
    constexpr bool turn = std::is_same<Color, White>::value;

    double t = 0;
    MoveScore best = { Move(), -INFINITY };

    for (int i = 1; i <= depth; i++) {
        auto start = std::chrono::system_clock::now();
        
        best = nega_max<Color>(b, ctx, i, -INFINITY, INFINITY);

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        t += elapsed_seconds.count();

        std::cout << "info depth " << i << "\n";
        std::cout << "info score cp " << best.score * (turn ? 1 : -1) << "\n";
        std::cout << "info pv " << best.move.to_string() << "\n";
        // std::cout << i << ": "
        //           << best.move.to_string() << " - "
        //           << (best.eval * (turn ? 1 : -1)) << '\n';
        // if (t > 0.5) break;
    }

    return best;
}