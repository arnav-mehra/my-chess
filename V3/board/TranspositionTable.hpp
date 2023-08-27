#pragma once

#include "../util/types.hpp"
#include "../move/Move.hpp"

struct MoveScore {
    Move move;
    I16 score;
};

constexpr bool USE_TRANSPOSITION_TABLE = true;

namespace TranspositionTable {

    // Constants

    constexpr size_t IDX_BITS = 20;
    constexpr U64    IDX_MASK = ((1ULL << IDX_BITS) - 1ULL);
    constexpr U64    HV_MASK  = ~IDX_MASK;
    constexpr size_t TT_SIZE  = 1ULL << IDX_BITS;

    // Stats

    U64 hits = 0;
    U64 misses = 0;

    // Data Structures

    enum NodeType : U8 {
        EXACT,
        LOWER,
        UPPER
    };

    struct Cell {
        U64 hash_depth; // [ 45-bit hash | 3-bit gap | 16-bit depth ]
        Move move;
        I16 score;
        NodeType node_type;

        U64 get_hash()  { 
            return hash_depth >> IDX_BITS;
        }
        U16 get_depth() {
            return (U16)(hash_depth & IDX_MASK);
        }
    };

    struct Entry {
        Cell rec_cell; // recency_cell
        Cell dep_cell; // depth_cell
    };

    Entry table[TT_SIZE] = {};

    // Functions

    inline Entry* get_entry(U64 hash) {
        U64 idx = hash & IDX_MASK;
        return &table[idx];
    }

    inline std::pair<bool, Cell*> get_cell(U64 hash, U16 min_depth) {
        if constexpr (!USE_TRANSPOSITION_TABLE) return { false, nullptr };

        Entry* entry = get_entry(hash);
        Cell* rec_cell = &(entry->rec_cell);
        Cell* dep_cell = &(entry->dep_cell);

        U64 hash_val = hash >> IDX_BITS;
        // check with depth cell first (optimal accuracy).
        hits++;
        if (hash_val == dep_cell->get_hash()) return { true, dep_cell };
        // check with recency cell second (optimal hitrate).
        if (hash_val == rec_cell->get_hash()) return { true, rec_cell };
        // complete miss, return replacement cell.
        hits--; misses++;
        Cell* rep_cell = (min_depth >= dep_cell->get_depth()) ? dep_cell : rec_cell;
        return { false, rep_cell };
    }

    inline void set_cell(Cell* cell, U64 hash, U16 depth, MoveScore ms, I16 og_alpha, I16 beta) {
        if constexpr (!USE_TRANSPOSITION_TABLE) return;

        cell->hash_depth = (hash & HV_MASK) | (U64)depth;
        cell->score = ms.score;
        cell->move = ms.move;
        cell->node_type = ms.score <= og_alpha ? NodeType::UPPER
                        : ms.score >= beta     ? NodeType::LOWER
                        : NodeType::EXACT;
    }

    void clear_cells() {
        for (Entry& entry : table) {
            entry.dep_cell.hash_depth = 0ULL;
            entry.rec_cell.hash_depth = 0ULL;
        }
    }
};