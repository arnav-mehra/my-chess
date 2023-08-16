#include <iostream>
#include <chrono>
#include <map>
#include <fstream>

#include "board/impl/index.hpp"
#include "move/impl/index.hpp"
#include "init/init.hpp"
#include "search/evaluate.hpp"
#include "search/search.hpp"
#include "tests/perft.hpp"
#include "tests/timer.hpp"

using namespace std;

static Board b;

int main(int argc, char** argv) {
    init();

    Context ctx = b.from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");

    // cout << hex << ctx.hash;

    // Perft::run<White>(b, ctx, 5); // , "tests/perft_solution.txt"

    bool turn = true;
    
    // while (true) {
        time_fn([turn, ctx]() {

            Context _ctx = ctx;
            auto [ eval, move ] = turn ? Search::search<White>(b, _ctx, 8)
                                       : Search::search<Black>(b, _ctx, 8);

            cout << "EVAL: " << eval << '\n';
            move.print();
            cout << '\n';

            // if (turn) b.do_move<White>(Search::best_move, _ctx);
            // else      b.do_move<Black>(Search::best_move, _ctx);

            cout << "Branches:\t"       << Search::branches  << '\n';
            cout << "BF:\t\t"           << Search::branches / (Search::nodes + 0.0) << '\n';
            cout << '\n';
            cout << "TT Hits:\t"        << Search::tt_hits     << '\n';
            cout << "TT Partials:\t"    << Search::tt_partials << '\n';
            cout << "TT Misses:\t"      << Search::tt_misses   << '\n';
            cout << '\n';
            cout << "Killer Entries:\t" << KillerTable::fill << '\n';
            cout << "Killer Hits:\t"    << KillerTable::hits << '\n';
            cout << "Killer Misses:\t"  << KillerTable::misses << '\n';
            cout << '\n';
            // cout << "TTHR: " << Search::tt_hits / (Search::branches + 0.0) << '\n';
        });
        turn = !turn;
    // }
    
    // perft<White>(b, 6);

    // U64 cells[2] = {};
    // cout << hex << cells[0] << ' ' << hex << cells[1] << '\n';
    // U16* arr = (U16*)cells;
    // arr[0] = 0xffff;
    // cout << hex << cells[0] << ' ' << hex << cells[1];
    // [ 3210, 7654 ]
    // [ 10, 32 ]
    // [ 0, 1 ]
}