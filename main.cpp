#include "MagicMoves.h"
#include "board.h"
#include "search.h"
#include "printer.h"
#include <vector>
#include <math.h>
#include <iostream>
#include <cstdint>
#include <memory>
using namespace std;
int nodes = 0;


int perft(int depth, bool turn, Board b) {
  int nodes = 0;
  if (depth == 0) { return 1; }
  vector<vector<int>> moves = b.moveGen(turn);
  for (int i=0; i<moves.size(); i++) {
    int piece = moves[i][0], capture = moves[i][3];
    uint64_t fromVal = 1ULL << moves[i][1], toVal = 1ULL << moves[i][2];
    
    b.move(piece, fromVal, toVal, capture, turn);
    nodes += perft(depth-1, !turn, b);
    b.unmove(piece, fromVal, toVal, capture, turn);
  }
  return nodes;
}
int quiesce (int alpha, int beta, bool turn, Board b) {
  int stand_pat = turn? b.evaluate() : -b.evaluate();
  if( stand_pat >= beta ) {
    nodes++;
    return beta;
  }
  if( alpha < stand_pat ) {
    alpha = stand_pat;
  }

  vector<vector<int>> moves = b.captureGen(turn);
  for (int i=0; i<moves.size(); i++)  {
    int piece = moves[i][0], capture = moves[i][3];
    uint64_t fromVal = 1ULL << moves[i][1], toVal = 1ULL << moves[i][2];

    b.move(piece, fromVal, toVal, capture, turn);
    int score = -quiesce(-beta, -alpha, !turn, b);
    b.unmove(piece, fromVal, toVal, capture, turn);

    if (score >= beta) {
      nodes++;
      return beta;
    }
    if (score > alpha) {
      alpha = score;
    }
  }
  nodes++;
  return alpha;
}
int alphaBetaNegaMax(int alpha, int beta, int depth, int turn, Board b) {
  if (depth == 0) { return quiesce(alpha, beta, turn, b); }

  vector<vector<int>> moves = b.moveGen(turn);
  if (moves.size() == 0) {
    return 0;
  }
  for (int i=0; i<moves.size(); i++) {
    int piece = moves[i][0], capture = moves[i][3];
    uint64_t fromVal = 1ULL << moves[i][1], toVal = 1ULL << moves[i][2];
    
    b.move(piece, fromVal, toVal, capture, turn);
    int score = -alphaBetaNegaMax(-beta, -alpha, depth-1, !turn, b);
    if (score >= beta) {
      return beta;
    }
    if (score > alpha) {
      alpha = score;
    }
    b.unmove(piece, fromVal, toVal, capture, turn);
  }
  return alpha;
}
vector<vector<int>> abNegaMaxCall(int depth, int turn, Board b) {
  int alpha = -100000, beta = 100000;
  int alphaIndex = 0;

  vector<vector<int>> moves = b.moveGen(turn);
  for (int i=0; i<moves.size(); i++) {
    int piece = moves[i][0], capture = moves[i][3];
    uint64_t fromVal = 1ULL << moves[i][1], toVal = 1ULL << moves[i][2];
    
    b.move(piece, fromVal, toVal, capture, turn);
    int score = -alphaBetaNegaMax(-beta, -alpha, depth-1, !turn, b);
    if (score > alpha) {
        alpha = score;
        alphaIndex = i;
    }
    b.unmove(piece, fromVal, toVal, capture, turn);
  }
  return {moves[alphaIndex], {alpha}};
}


int main() {
	initmagicmoves();

  Board b;
  // b.initializeBoard();
  b.setBoardFen(
    // "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"
    "8/R4pk1/6p1/3KN2p/7P/8/3r2P1/8"
  );
  b.printBoard();
  bool turn = true;
  int depth = 4;

  
  // printMoves(b.moveGen(true));
  vector<vector<int>> bestMoveAndEval = abNegaMaxCall(depth, turn, b);
  // vector<vector<int>> bestMoveAndEval2 = s.abNegaMaxCall(depth, turn);

  printMove(bestMoveAndEval[0]);
  // printMove(bestMoveAndEval2[0]);
  // quirkyStatement(bestMoveAndEval[1][0]);

  cout << '\n' << "Nodes Searched: " << nodes << '\n';
  cout << "Evaluation: " << bestMoveAndEval[1][0]/100.0 << '\n';
  // printNumGrid();
}

