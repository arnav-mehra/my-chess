// FULL CREDIT OF MAGIC ALGO TO Eugene Kotlov
// https://www.talkchess.com/forum3/viewtopic.php?f=7&t=69947\

#pragma once

#include "../util/types.hpp"
#include "../util/data.hpp"
#include "../util/util.hpp"
#include "../util/assertion.hpp"

#define COLLISION_DETECTION false

#define R_ADDR_MASK 0x000000000001FC00ULL
#define B_ADDR_MASK 0x0000000000001FE0ULL

namespace KMAGICS {
    U64 b_mask[64];
    U64 r_mask[64];

    U64 b_attacks[5248] = {};
    U64 r_attacks[102400] = {};
    U64 b_kmask[64];
    U64 r_kmask[64];

    U64 r_magic[64] = {
        0xd1800040008053ea,0xd48020004000f383,0xd700084020013500,0xd600084022007292,
        0xd500080003009331,0xd600020008049b50,0xd50001000400fa00,0xd10000810000d3e6,
        0xd64c80004000a39b,0xda03004000806f01,0xd802001022004b80,0xd802001020408e00,
        0xd81880430a087ffe,0xd82900080c00ab00,0xd844000d6802afb4,0xd60200006ce6b3f6,
        0xd44000800064bbde,0xd8200040100067c3,0xd820008010006b81,0xd84202001020c3f8,
        0xd9600e000a00c6e0,0xd862008004008680,0xd80144000208e310,0xd4080200038303f4,
        0xd400800100210bc1,0xd8201000400027c7,0xd820010100102bc0,0xd803402a000ee600,
        0xd94a003a00128aa0,0xdb0400808004ea00,0xda008dac0002ef58,0xd40100010000cbb2,
        0xd4008040008014e0,0xd810002000400fcc,0xd810080020200403,0xd805001001001ffd,
        0xda9300a801002ffd,0xd802001400804e80,0xd802000802007b7c,0xd6e7000081003bc2,
        0xd7ffe7fed9fe300b,0xd806010080420064,0xd8a0020400301001,0xd850008100080800,
        0xdafffdfefff02020,0xdbffffbe5fde6008,0xd812408002008100,0xd7ffffdbeb7a4012,
        0xd5fffa7fffdf43f0,0xdbfffbf7ffd323f0,0xdbffffeea7ff27e0,0xdb9ffdbfcbdb2be0,
        0xdb3fff9f6feb2fe0,0xdbffff3bdffd33e0,0xdaefffdfbf733fe0,0xd65fffffbeff4bf0,
        0xd3ffffd77fff13ef,0xd44e8100400163f9,0xd7ffffdfffbf6bf7,0xd7ffff9dffbf73fa,
        0xd7ffffd7ffaf7bfd,0xd7cffffefff783ff,0xd77fffff77ed8bfd,0xd3228785022553fe
    };

    U64 b_magic[64] = {
        0xe858a9ebe999f09f,0xec1090011101ecdf,0xec0848030061ecff,0xed2806004041ed1f,
        0xec0403084001ed3f,0xec05040a4001ed5f,0xee011808040ded7f,0xe885040100a9f0df,
        0xee2840020d05eabf,0xec0a021c1801eadf,0xec2028080101ed9f,0xec8251050201edbf,
        0xed0404102801eddf,0xec0082080405edff,0xec860094515bee1f,0xee8d088c0145ee3f,
        0xec2005684215ea7f,0xed1404878489eaff,0xe428033000cff21b,0xe42401180141f29f,
        0xe7eefeb7eebff31f,0xe4226092d015ebb2,0xee0092040245ee5f,0xec820009b201ee7f,
        0xef7840035911ee9f,0xed100a4c7041ea5f,0xe50370004483f380,0xde04080004012410,
        0xdc02040002018213,0xe40802000991b41f,0xecae008001dfef1f,0xee04e20642c1ea3f,
        0xec65a0300c89ef3f,0xed4c0c8c06c7ef5f,0xe463080802750b3f,0xde12200800010811,
        0xdc3006020011e018,0xe592411200010782,0xec0a1a120803ea97,0xed7df1f23a85eebf,
        0xec1108084001eedf,0xee0402121001eeff,0xe402004a0801e69c,0xe69816012401e60a,
        0xe42010020201f194,0xe7bfb7e7fb5dec3f,0xeca0180102c1e71f,0xee180b21a185ea1f,
        0xec0108080209ef7f,0xec1104018443ef9f,0xec8520e18831efbf,0xee572ca9f5fdefdf,
        0xec00a06aa303efff,0xec0418100c81f01f,0xec4004082201f03f,0xec0850010601f05f,
        0xeb0104010495f11f,0xec0500440181f07f,0xedd27463f5b7ecbf,0xef48054001e40727,
        0xeea3d65ffdd74753,0xec9c7e9c4dc1e77f,0xec0060600e03eb1f,0xe9900c100401f15f
    };

    std::pair<int, int> BISHOP_DIRS[4] = {
        { 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 }
    };

    std::pair<int, int> ROOK_DIRS[4] = {
        { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }
    };

    U64 get_attacks_from_blockers(Square sq, U64 blockers, std::pair<int, int> (&dirs)[4]) {
        U64 res = 0;
        int r = sq >> 3, c = sq & 0b111;
        for (auto [ rd, cd ] : dirs) {
            for (
                int i = r + rd, j = c + cd;
                j >= 0 && j < 8 && i >= 0 && i < 8;
                i += rd, j += cd
            ) {
                int to_sq = (i << 3) | j;
                U64 to_sq_bit = 1ULL << to_sq;
                res |= to_sq_bit;
                if ((to_sq_bit & blockers) != 0ULL) break;
            }
        }
        return res;
    }

    U64 map_bits_to_mask(U32 bits, U64 mask) { // (bits = 010101, mask = NxxNNxxxxNNN) -> 0xx10xxxx101.
        U64 res = 0;
        while (mask) {
            Square sq = pop_lsb(mask);
            res |= (1ULL << sq) * (bits & 1);
            bits >>= 1;
        }
        return res;
    }

    void set_r_attacks(int sq, U64 blockers) {
        U64 attacks = get_attacks_from_blockers(sq, blockers, ROOK_DIRS);

        U64 magic = r_magic[sq];
        U64 addr  = magic & R_ADDR_MASK;
        int shift = magic >> 58;

        blockers  &= r_kmask[sq];  // regular mask to special kmask.
        blockers  *= magic;
        blockers >>= shift;
        blockers  += addr;
        
        U64& cell = r_attacks[blockers];
        if constexpr (COLLISION_DETECTION) {
            assert("MAGIC ROOK COLLISION", cell == 0 || cell == attacks);
        }
        cell = attacks;
    }

    void set_b_attacks(int sq, U64 blockers) {
        U64 attacks = get_attacks_from_blockers(sq, blockers, BISHOP_DIRS);

        U64 magic = b_magic[sq];
        U64 addr = magic & B_ADDR_MASK;
        int shift = magic >> 58;

        blockers  &= b_kmask[sq]; // regular mask to special kmask.
        blockers  *= magic;
        blockers >>= shift;
        blockers  += addr;

        U64& cell = b_attacks[blockers];
        if constexpr (COLLISION_DETECTION) {
            assert("MAGIC BISHOP COLLISION", cell == 0 || cell == attacks);
        }
        cell = attacks;
    }

    void write_att(Square sq, bool is_b) {
        U64 mask = is_b ? b_mask[sq] : r_mask[sq];
        int cnt = pop_count(mask);

        for (U32 i = 0; i < (1U << cnt); i++) {
            U64 blockers = map_bits_to_mask(i, mask);
            if (is_b) set_b_attacks(sq, blockers);
            else      set_r_attacks(sq, blockers);
        }
    }

    constexpr void init() {
        for (Square i = 0; i < NUM_SQUARES; i++) {
            int rank = 7 - (i >> 3), file = i & 0b111;
            r_mask[i] = BB_SETS::RANK[rank] ^ BB_SETS::FILE[file];
            b_mask[i] = BB_SETS::F_DIAGONAL[(7 - rank) + file]
                      ^ BB_SETS::B_DIAGONAL[(7 - rank) - file + 7];

            // mask optimization: exclude ends.
            U64 z = 0x007E7E7E7E7E7E00ull;
            U64 xx = 1ULL << i;
            z |= xx & BB_SETS::RANK[7] ? BB_SETS::RANK[7] : 0ull;
            z |= xx & BB_SETS::RANK[0] ? BB_SETS::RANK[0] : 0ull;
            z |= xx & BB_SETS::FILE[7] ? BB_SETS::FILE[7] : 0ull;
            z |= xx & BB_SETS::FILE[0] ? BB_SETS::FILE[0] : 0ull;
            r_kmask[i] = r_mask[i] & z & 0x7EFFFFFFFFFFFF7Eull; // exclude corners
            b_kmask[i] = b_mask[i] & z;

            write_att(i, false);
            write_att(i, true);
        }
    }

    U64 get_r_attacks(Square sq, U64 occ) {
        U64 magic = r_magic[sq];
        U64 addr  = magic & R_ADDR_MASK;
        int shift = magic >> 58;

        occ  &= r_kmask[sq];
        occ  *= magic;
        occ >>= shift;
        occ  += addr;
        return r_attacks[occ];
    }

    U64 get_b_attacks(Square sq, U64 occ) {
        U64 magic = b_magic[sq];
        U64 addr  = magic & B_ADDR_MASK;
        int shift = magic >> 58;

        occ  &= b_kmask[sq];
        occ  *= magic;
        occ >>= shift;
        occ  += addr;
        return b_attacks[occ];
    }

    U64 get_q_attacks(Square sq, U64 occ) {
        return get_r_attacks(sq, occ) | get_b_attacks(sq, occ);
    }
};