#pragma once

#include "../util/data.hpp"
#include "../util/types.hpp"
#include "../util/util.hpp"
#include "../util/conversion.hpp"
#include <iostream>
#include <string>

struct Move {
    static U32 MOVE_MASK;

    U32 data; // [-----SC----|R|CAPT|FLAG|-FROM-|--TO--]

    Move();
    Move(U32 data);
    Move(Square from, Square to, Flag flag);

    template<Flag flag>
    static Move make(Square from, Square to);

    Piece  get_capture();
    U32    get_reversable();
    Flag   get_flag();
    Square get_from();
    Square get_to();
    U32&   get_raw();
    U32    get_masked();

    U64 get_from_bit();
    U64 get_to_bit();

    template<class Color> void set_score_capt(void* b, Move& priority, U16 depth);
    U32 get_move_score(Piece pc, Piece capt, Flag flag, U16 depth, Move& priority);
    U32 get_capture_score(Piece pc, Piece capt, Flag flag, U16 depth, Move& priority);
    U32 get_quiet_score(Piece pc, Flag flag, U16 depth, Move& priority);

    bool operator==(const Move& m);
    bool operator> (const Move& m);

    std::string to_string();
};

U32 Move::MOVE_MASK = (1U << 21) - 1U;