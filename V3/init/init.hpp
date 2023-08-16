#pragma once

#include "../util/data.hpp"
#include "kmagics.hpp"
#include "mapped_moves.hpp"
#include "zobrist.hpp"
#include "pestos.hpp"

void init() {
    MAPPED_MOVES::init();
    KMAGICS::init();
    ZOBRIST::init();
    PeSTOs::init();
}