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
  
  // b.setBoardFen(
  //   "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"
  //   // "rnbqk2r/pppp2pp/5p2/4p3/3Pn3/2P1PP2/PP4PP/RNBQK1NR"
  // );
  
  
  
  // b.setBoardFen(
  //   "2kr1b2/P4p2/5p2/2p1n3/8/3BPK2/P1P5/R5q1"
  // );
  b.initializeBoard();
  b.printBoard();

  Search s(b);
<<<<<<< Updated upstream
  cout << s.perft(6, true) << '\n';
  // Move bestMove = s.abNegaMaxCall(8, true);
=======
  cout << s.perft(3, true) << '\n';
  // Move bestMove = s.abNegaMaxCall(7, true);
>>>>>>> Stashed changes
  // for (int i=0; i<1000000; i++) {
  //   vector<Move>m;
  //   b.moveGen(true, m);
  // }
  // printMove(bestMove);
  // // //quirkyStatement(s.eval);
  // cout << '\n' << "Nodes Searched: " << s.nodes << '\n';
  // cout << "Evaluation: " << s.eval/100.0 << '\n';
}