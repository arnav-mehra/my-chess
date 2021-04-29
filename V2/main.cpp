#include <vector> 
#include <cstdint>
#include <iostream>

#include "piece.h"
#include "MagicMoves.h"
#include "moveData.h"
#include "move.h"
#include "board.h"
#include "moveGen.h"
#include "moveMaking.h"
#include "evaluation.h"
#include "search.h"
#include "printer.h"

using namespace std;



int main() {
  initmagicmoves();

  Board b; 
  // b.initializeBoard();
  b.setBoardFen(
    "2kr1b2/Pb1n1p2/5p2/2p5/8/3BPP2/P1PKr3/R5q1"
  );
  b.printBoard();
  Search s(b);

  // cout << s.perft(6) << '\n';
  // Move bestMove = s.getBestMoveTimed(1000);
  Move bestMove = s.abNegaMaxCall(4);

  printMove(bestMove);
  quirkyStatement(s.eval);
  cout << '\n' << "Nodes Searched: " << s.nodes << '\n';
  cout << "Evaluation: " << s.eval/100.0 << '\n';
}


