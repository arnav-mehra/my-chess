#include <iostream>
#include "classes/board_impl/interface.hpp"
#include "classes/board_impl/do_move.hpp"
#include "classes/board_impl/undo_move.hpp"
#include "classes/board_impl/gen_moves.hpp"
#include "util/init.hpp"
#include <chrono>
#include <map>
#include <fstream>
using namespace std;

// 00 01 02 03 04 05 06 07
// 08 09 10 11 12 13 14 15
// 16 17 18 19 20 21 22 23
// 24 25 26 27 28 29 30 31
// 32 33 34 35 36 37 38 39
// 40 41 42 43 44 45 46 47
// 48 49 50 51 52 53 54 55 
// 56 57 58 59 60 61 62 63

static Board b;

int main() {

    init();

    // CLI(INITIAL_FEN);
    // CLI("rnbqkb1r/pppppppp/7n/8/8/3P4/PPP1PPPP/RNBQKBNR w KQkq - 0 1");
    // CLI("rnbqkbnr/p1pppppp/8/8/8/2PPPP2/PpNQKPPP/R1B2BNR b HAkq - 0 1");
    // CLI("rn1qk2r/ppPbpppp/5bn1/4pp2/8/1NB2BN1/PPPQPPPP/R3K2R w KQkq - 0 1");
    // CLI("r3k2r/ppnbpppp/3q1bn1/4pp2/8/1NB2BN1/PPPQPPPP/R3K2R b KQkq - 0 1");
    
    b.from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    // b.from_fen(INITIAL_FEN);
    // MoveList ml; b.gen_moves<Black, Gen::PSEUDOS>(ml);
    // ml.print();
    perft<White>(b, 5, "");

    

    // Board b2;
    // b2.from_fen(INITIAL_FEN);
    // perft(b2, 4);

    // b.assert_board_consistency();
    // b.print();

    // MoveList ml = MoveList();
    // b.gen_moves(ml);
    // ml.print();

    // auto start = std::chrono::system_clock::now();
    // U64 N = 10000000000;
    // for (U64 i = 0; i < N; i++) {
    //     Move m1 = Move::from<Flag::PAWN_DOUBLE, Piece::NA>(Piece::NA, 0, 35);
    //     b.do_move<White>(m1);
    //     b.undo_move<White>(m1);
    // }
    // auto end = std::chrono::system_clock::now();
    // std::chrono::duration<double> t_sec = end - start;
    // cout << (N / (t_sec.count() * 1000.0)) << " Kn/S\n";

    // 1B / 3.6S
    // MAX ACHIEVABLE: ~280,000 Kn/S

    // b.print();
}