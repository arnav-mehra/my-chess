#include "Move.hpp"

#include <array>

// Move DS: [-----SC-----|CAPT|FLAG|-FROM-|--TO--]

constexpr bool USE_KILLER_TABLE = false;

// namespace KillerTable {
//     constexpr size_t MAX_DEPTH = 32;
//     constexpr size_t SLOTS = 2;
//     constexpr U32 SLOT_MASK = SLOTS - 1;

//     U64 hits = 0;
//     U64 fill = 0;
//     U64 misses = 0;
//     U64 calls = 0;

//     U32 table[MAX_DEPTH][SLOTS];
//     int ptrs[MAX_DEPTH] = {};

//     bool has_quiet(Move& m, U16 depth) {
//         if constexpr (!USE_KILLER_TABLE) return false;

//         U32 target = m.get_masked();
//         int first_slot = ptrs[depth];
//         int slot = first_slot;
//         while (1) {
//             slot = (slot + 1) & SLOT_MASK;
//             if (table[depth][slot] == target) {
//                 hits++;
//                 return true;
//             }
//             if (slot == first_slot) break;
//         }

//         misses++;
//         return false;
//     }

//     bool has_move(Move& m, U16 depth) {
//         if constexpr (!USE_KILLER_TABLE) return false;

//         if (m.get_capture() != Piece::NA) return false;

//         return has_quiet(m, depth);
//     }

//     void add_move(Move& m, U16 depth) {
//         if constexpr (!USE_KILLER_TABLE) return;

//         if (m.get_capture() != Piece::NA) return;
//         if (has_move(m, depth)) return;

//         int slot = ptrs[depth]; // get final slot
//         ptrs[depth] = (ptrs[depth] + SLOTS - 1) & SLOT_MASK; // decrement ptr
//         table[depth][slot] = m.get_masked(); // add move
//     }
// };

namespace KillerTable {
    constexpr size_t MAX_DEPTH = 32;
    constexpr size_t SLOTS = 2;

    U64 hits = 0;
    U64 fill = 0;
    U64 misses = 0;
    U64 calls = 0;

    U32 table[MAX_DEPTH][SLOTS];

    bool has_move(Move& m, U16 depth) {
        if constexpr (!USE_KILLER_TABLE) return false;

        U32 target = m.get_masked();

        if ((table[depth][0] == target) | (table[depth][1] == target)) {
            hits++;
            return true;
        }

        misses++;
        return false;
    }

    bool has_quiet(Move& m, U16 depth) {
        return has_move(m, depth);
    }

    void add_move(Move& m, U16 depth) {
        if constexpr (!USE_KILLER_TABLE) return;

        if (m.get_capture() != Piece::NA) return;

        for (int i = SLOTS - 1; i >= 1; i--) {
            table[depth][i] = table[depth][i - 1];
        }
        table[depth][0] = m.get_masked();
    }
};