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
  //   "r1b2rk1/pp1q1ppp/3b4/6B1/1p1p4/1QnP1N2/P3BPPP/2R2RK1"
  // );
  b.initializeBoard();
  b.printBoard();

  Search s(b);
  // cout << s.perft(4, true) << '\n';
  Move bestMove = s.abNegaMaxCall(7, true);
  
  printMove(bestMove);
  // //quirkyStatement(s.eval);
  cout << '\n' << "Nodes Searched: " << s.nodes << '\n';
  cout << "Evaluation: " << s.eval/100.0 << '\n';
}