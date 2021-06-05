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
#include "interface.h"
#include "transposition.h"
#include "generation.h"
#include "moving.h"
#include "evaluation.h"
#include "search.h"

using namespace std;



int main() {
  initmagicmoves();
  init_tables();

  Board b; 
  b.initializeBoard();
  // b.setBoardFen("8/pk5p/2r3p1/2pP1pP1/3p4/R2K3P/5P2/8");
  b.printBoard();

  // b.turn = false;
  bool useTranpositionTable = true; //removed
  bool useQuiescentSearch = true;
  Search s(b, useTranpositionTable, useQuiescentSearch);


  uint8_t depth = 7;
  Move bestMove = s.iterativeCall(depth);
  // Move bestMove = s.abNegaMaxCall(depth);
  

  printMove(bestMove);
  cout << '\n' << "Nodes Evaluated: " << s.nodes << '\n';
  // cout << "Nodes Transposed: " << s.transposedNodes << '\n';
  cout << "Evaluation: " << s.eval/100.0 << '\n';
  cout << "Depth: " << depth+0 << " / " << depth + s.quiescenceDepth << '\n';
}