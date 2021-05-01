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
  // uint64_t cKing = 0;
  // cout << (__builtin_popcount(cKing)? lsb(cKing) : -1);
  // cout << log2(cKing);
  // initmagicmoves();

  Board b; 
  // b.initializeBoard();
  b.setBoard("rnbqkbnrpppp.ppp............p......P............PPP.PPPPRNBQKBNR");
  b.printBoard();

  bool useTranpositionTable = false;
  bool useQuiescentSearch = true;
  Search s(b, useTranpositionTable, useQuiescentSearch);
  
  // cout << s.perft(3);
  // cout << cap;
  vector<Move> m;
  b.moveGen(m);
  printMove(m[m.size()-1]);
  b.move(m[m.size()-1]);
  b.printBoard();
  b.unmove(m[m.size()-1]);
  b.printBoard();
  // uint8_t depth = 7;
  // Move bestMove = s.abNegaMaxCall(depth);

  // printMove(bestMove);
  // cout << '\n' << "Nodes Evaluated: " << s.nodes << '\n';
  // cout << "Evaluation: " << s.eval/100.0 << '\n';
  // cout << "Depth: " << depth+0 << " / " << depth + s.quiescenceDepth << '\n';
}

// quirkyStatement(s.eval);
// Move bestMove = s.getBestMoveTimed(1000);
// printMoves(s.legalMoveGen());

// b.setBoardFen(
//   "2kr1b2/Pb1n1p2/5p2/2p5/8/3BPP2/P1PKr3/R5q1"
// );