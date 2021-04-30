#include <vector> 
#include <cstdint>
#include <iostream>
#include <array>
#include <map>
#include <algorithm>

#include "MagicMoves.h"
#include "data.h"

#include "piece.h"
#include "move.h"
#include "board.h"

#include "printer.h"
#include "transposition.h"
#include "generation.h"
#include "moving.h"
#include "evaluation.h"
#include "search.h"

using namespace std;



int main() {
  initmagicmoves();

  Board b; 
  b.initializeBoard();
  // b.setBoardFen(
  //   "2kr1b2/Pb1n1p2/5p2/2p5/8/3BPP2/P1PKr3/R5q1"
  // );
  b.printBoard();

  bool useTranpositionTable = false;
  bool useQuiescentSearch = true;
  Search s(b, useTranpositionTable, useQuiescentSearch);
  
  // // b.addZobristKey();

  // // cout << s.perft(6) << '\n';
  // // Move bestMove = s.getBestMoveTimed(1000);
  cout << s.perft(6);

  // uint8_t depth = 8;
  // Move bestMove = s.abNegaMaxCall(depth);


  // printMove(bestMove);
  // // quirkyStatement(s.eval);
  // cout << '\n' << "Nodes Evaluated: " << s.nodes << '\n';
  // cout << "Evaluation: " << s.eval/100.0 << '\n';
  // cout << "Depth: " << depth+0 << " / " << depth + s.quiescenceDepth << '\n';
}


