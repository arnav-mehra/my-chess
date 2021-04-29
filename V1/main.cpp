#include <vector> 
#include <cstdint>
#include <iostream>

#include "MagicMoves.h"
#include "moveData.h"
#include "move.h"
#include "board.h"
#include "moveGen.h"
#include "search.h"
#include "printer.h"

using namespace std;



int main() {
	initmagicmoves();

  Board b; 
  b.initializeBoard();
  // b.printBoard();
  // b.setBoardFen(
  //   "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"
  //   // "rnbqk2r/pppp2pp/5p2/4p3/3Pn3/2P1PP2/PP4PP/RNBQK1NR"
  // );
  
  
  Search s(b);

  cout << s.perft(6, true) << '\n';
  // Move bestMove = s.abNegaMaxCall(6, true);
  // for (int i=0; i<1000000; i++) {
  //   vector<Move>m;
  //   b.moveGen(true, m);
  // }
  // printMove(bestMove);
  // //quirkyStatement(s.eval);
  // cout << '\n' << "Nodes Searched: " << s.nodes << '\n';
  // cout << "Evaluation: " << s.eval/100.0 << '\n';
}