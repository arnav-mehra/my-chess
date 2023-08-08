#pragma once

#include "../util/data.hpp"
#include "kmagics.hpp"
#include "mapped_moves.hpp"
#include "zobrist.hpp"

void init() {
    MAPPED_MOVES::init();
    KMAGICS::init();
    ZOBRIST::init();
}