#pragma once

#include "../board/impl/index.hpp"
#include "../move/impl/index.hpp"
#include "evaluate.hpp"
#include "../board/TranspositionTable.hpp"
#include <algorithm>
#include <unordered_set>

#define INFINITY INT16_MAX

class Search {
public:

    // stats

    static U64 leaves;
    static U64 nodes;

    static U64 tt_hits;
    static U64 tt_partials;
    static U64 tt_misses;

    // search

    template<class Color>
    static MoveScore search(
        Board& b,
        Context& ctx,
        U16 depth
    );

private:

    // mini-max searches

    template<class Color>
    static MoveScore nega_max(
        Board& b,
        Context& ctx,
        U16 depth,
        I16 alpha,
        I16 beta
    );

    template<class Color>
    static MoveScore nega_scout(
        Board& b,
        Context& ctx,
        U16 depth,
        I16 alpha,
        I16 beta
    );

    template<class Color>
    static I16 quiesce(
        Board& b,
        Context& ctx,
        I16 alpha,
        I16 beta
    );
};

U64 Search::leaves = 0;
U64 Search::nodes = 0;
U64 Search::tt_hits = 0;
U64 Search::tt_partials = 0;
U64 Search::tt_misses = 0;