#include "../util/types.hpp"
#include "../move/Move.hpp"

struct EvalMove {
    I16 eval;
    Move move;
};

namespace TranspositionTable {
    constexpr size_t IDX_BITS = 16;
    constexpr U64    IDX_MASK = ((1ULL << IDX_BITS) - 1ULL);
    constexpr size_t TT_SIZE = 1ULL << IDX_BITS;
    constexpr size_t ENTRY_SIZE = 4; // 4 U64 within a single entry/record

    U64 table[TT_SIZE * ENTRY_SIZE] = {};

    // entry = {
    //      cell 1: most recently used   (high quantity hits),
    //      cell 2: highest depth result (high quality hits)
    // }

    // cell = {
    //      [48-bit hash_val][16-bit depth],
    //      [32-bit move][16-bit eval][16-bit node-type],
    // }

    inline U64* get_entry(U64 hash) {
        U64 idx = hash & IDX_MASK;
        return &table[idx * ENTRY_SIZE];
    }

    inline U64 get_hash_val(U64* cell) {
        return cell[0] >> IDX_BITS;
    }

    inline U16& get_depth(U64* cell) {
                                // { 3, 2, 1, 0, 7, 6, 5, 4 }. Note: flip due to endianness.
        return ((U16*)cell)[0]; // { h, h, h, d, m, m, e, n }.
    }

    inline Move& get_move(U64* cell) {
                                 // { 1,   0, 3,   2 }
        return ((Move*)cell)[3]; // { h, h-d, m, e-n }
    }

    inline I16& get_eval(U64* cell) {
                                // { 3, 2, 1, 0, 7, 6, 5, 4 }. Note: flip due to endianness.
        return ((I16*)cell)[5]; // { h, h, h, d, m, m, e, n }
    }

    inline EvalMove get_eval_move(U64* cell) {
                 // { 3, 2, 1, 0, 7, 6, 5, 4 }. Note: flip due to endianness.
        return { // { h, h, h, d, m, m, e, n }
            get_eval(cell),
            get_move(cell)
        };
    }


    inline U16& get_node_type(U64* cell) {
                                // { 3, 2, 1, 0, 7, 6, 5, 4 }. Note: flip due to endianness.
        return ((U16*)cell)[4]; // { h, h, h, d, m, m, e, n }
    }

    inline U64* get_cell(U64 hash, U16 min_depth) {
        U64* entry = get_entry(hash);
        U64* rcell = &entry[0];
        U64* dcell = &entry[2];

        U64 hash_val = hash >> IDX_BITS;
        U64 rhash  = get_hash_val(rcell);
        U16 rdepth = get_depth(rcell);
        U64 dhash  = get_hash_val(dcell);
        U16 ddepth = get_depth(dcell);

        if (hash_val == dhash) return dcell;
        if (hash_val == rhash) return rcell;
        return nullptr;
    }

    void init_cell(U64* cell, U64 hash, U16 depth, Move move, I16 eval, U32 node_type) {
        cell[0] = hash;
        get_depth(cell)     = depth; // will overwrite hash's 16-bit violation.
        get_move(cell)      = move;
        get_eval(cell)      = eval;
        get_node_type(cell) = node_type;
    }

    inline void set_cell(U64 hash, U16 depth, Move move, I16 eval, U32 node_type) {
        U64* entry = get_entry(hash);
        U64* rcell = &entry[0];
        U64* dcell = &entry[2];
        U64 ddepth = get_depth(dcell);

        U64 new_cell[2];
        init_cell(new_cell, hash, depth, move, eval, node_type);

        if (depth > ddepth) {
            memcpy(dcell, new_cell, 16);
            return;
        }
        memcpy(rcell, new_cell, 16);
    }

};