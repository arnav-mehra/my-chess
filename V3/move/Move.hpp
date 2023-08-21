#pragma once

#include "../util/data.hpp"
#include "../util/types.hpp"
#include "../util/util.hpp"
#include "../util/conversion.hpp"
#include <iostream>
#include <string>

struct Move {
    U32 data; // [-----SC-----|CAPT|FLAG|-FROM-|--TO--]

    Move();
    Move(U32 data);
    Move(Square from, Square to, Flag flag);

    template<Flag flag>
    static Move make(Square from, Square to);

    Piece  get_capture();
    Flag   get_flag();
    Square get_from();
    Square get_to();
    U32    get_raw();

    U64 get_from_bit();
    U64 get_to_bit();

    void set_score_capt(void* b, U16 depth);
    U16 get_move_score(Piece pc, Piece capt, U16 depth);

    bool operator==(const Move& m);
    bool operator> (const Move& m);

    std::string to_string();
};