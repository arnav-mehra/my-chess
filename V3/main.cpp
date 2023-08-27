#include "board/impl/index.hpp"
#include "move/impl/index.hpp"
#include "init/init.hpp"
#include "search/evaluate.hpp"
#include "search/impl/index.hpp"
#include "tests/perft.hpp"
#include "tests/timer.hpp"

#include <iostream>
#include <chrono>
#include <map>
#include <fstream>
#include <cmath>

using namespace std;

static Board b;

U16 depth = 9;
string testing_fen[] = {
    "startpos",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
    "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1",
    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"
};

int main(int argc, char** argv) {
    init();
    // CLI();

    time_fn([]() {
        for (string& fen : testing_fen) {
            bool turn;
            Context ctx = b.from_fen(fen, turn);
            if (turn) Search::search<White>(b, ctx, depth);
                 else Search::search<Black>(b, ctx, depth);
            // if (turn) Perft::run<White>(b, ctx, depth);
            //      else Perft::run<Black>(b, ctx, depth);
        }
    });

    // float bf = (Search::nodes + 0.0) / (Search::nodes - Search::leaves + 0.0);
    U64 num = 0, den = 0;
    for (int i = 0; i <= depth; i++) {
        if (i != depth) num += Search::node_depth_hist[i];
        if (i != 0) den += Search::node_depth_hist[i];
    }
    std::cout << "\nnodes:\t"  << Search::nodes
              << "\nleaves:\t" << Search::leaves
              << "\nbf:\t" << (num + 0.0) / (den + 0.0)
              << "\n";

    std::cout << "\nkiller hits:\t"   << KillerTable::hits
              << "\nkiller misses:\t" << KillerTable::misses
              << "\nkiller hitrate:\t"  << (KillerTable::hits + 0.0) / (KillerTable::hits + KillerTable::misses + 0.0)
              << "\n";

    std::cout << "\ntt hits:\t"     << TranspositionTable::hits
              << "\ntt misses:\t"   << TranspositionTable::misses
              << "\ntt hitrate:\t"   << (TranspositionTable::hits + 0.0) / (TranspositionTable::hits + TranspositionTable::misses + 0.0);
}