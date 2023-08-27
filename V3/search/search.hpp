#pragma once

#include "../board/impl/index.hpp"
#include "../move/impl/index.hpp"
#include "evaluate.hpp"
#include "../board/TranspositionTable.hpp"
#include <algorithm>
#include <unordered_set>

#define INFINITY INT16_MAX

namespace Search {

    // constants

    constexpr U16 NULL_DEPTH_REDUCTION = 3;

    // stats

    U64 node_depth_hist[MAX_DEPTH] = {};
    U64 quiesce_nodes = 0;
    U64 negamax_nodes = 0;

    // search

    template<class Color>
    static MoveScore search(
        Board& b,
        Context& ctx,
        U16 depth
    );

    // mini-max searches

    template<class Color>
    MoveScore nega_max(
        Board& b,
        Context& ctx,
        U16 depth,
        I16 alpha,
        I16 beta
    );

    template<class Color>
    MoveScore nega_scout(
        Board& b,
        Context& ctx,
        U16 depth,
        I16 alpha,
        I16 beta
    );

    template<class Color>
    I16 quiesce(
        Board& b,
        Context& ctx,
        I16 alpha,
        I16 beta
    );
};