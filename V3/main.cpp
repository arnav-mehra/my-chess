#include <iostream>
#include <chrono>
#include <map>
#include <fstream>

#include "interface/interface.hpp"
#include "board/impl/index.hpp"
#include "init/init.hpp"
#include "search/evaluate.hpp"
#include "search/search.hpp"
#include "tests/perft.hpp"

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

int main(int argc, char** argv) {
    init();
    
    b.from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    auto [ move, eval ] = Search::search<White>(b, 3);
    cout << "EVAL: " << eval;
    // perft<White>(b, 6);
}