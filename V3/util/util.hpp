#pragma once

#include "types.hpp"
#include <functional>

// Index of next square / number of trailing zeros.
U32 lsb(U64 b) {
    return __builtin_ctzll(b);
}

// BitScan via g++ intrinsics
Square pop_lsb(U64 &b) {
    Square sq = lsb(b);
    b &= b - 1;
    return sq;
}

// Index of next square / number of trailing zeros.
inline int pop_count(U64 b) {
    return __builtin_popcountll(b);
}

template<int sh>
U64 shift(U64 bb) {
    if constexpr (sh > 0) return bb << sh;
    else                  return bb >> (-sh);
}