#include "index.hpp"

int main() {
  initmagicmoves();
  init_tables();
  initZobristKeys();

  Board b; 
  b.initializeBoard();
  // b.turn = false;
  // b.setBoardFen("2r2rk1/pp3pp1/3b3p/4n1q1/4P1b1/2P3N1/PP2QPPP/R4RK1");
  
  bool useTranpositionTable = false;
  bool useQuiescentSearch = true;
  Search s(b, useTranpositionTable, useQuiescentSearch);
  b.printBoard();
  s.runPerft(6);
  b.printBoard();
  // Move bestMove = s.abNegaMaxCall();

  // printMove(bestMove);
  // s.printStats();
}