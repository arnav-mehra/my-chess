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

    // Context ctx = b.from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    Context ctx = b.from_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
    // cout << hex << ctx.hash;

    Perft::run<White>(b, ctx, 5); // , "tests/perft_solution.txt"

    // bool turn = true;
    
    // while (true) {
        // time_fn([turn]() {
        //     int eval = turn ? Search::search<White>(b, 10)
        //                     : Search::search<Black>(b, 10);
            
        //     cout << "EVAL: " << eval << '\n';
        //     Search::best_move.print();

        //     if (turn) b.do_move<White>(Search::best_move);
        //     else      b.do_move<Black>(Search::best_move);

        //     cout << "BF: " << Search::branches / (Search::nodes + 0.0) << '\n';
        // });
        // turn = !turn;
    // }
    
    // perft<White>(b, 6);
}