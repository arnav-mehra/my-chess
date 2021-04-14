#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include <memory>
#include <vector>
#include <cstdint>


using namespace std;

class Search {

    public:

        Board b;
        int nodes=0;

        vector<vector<int>> abNegaMaxCall(int depth, int turn) {
            int alpha = -100000, beta = 100000;
            int alphaIndex = 0;

            vector<vector<int>> moves = b.moveGen(turn);
            for (int i=0; i<moves.size(); i++) {
                int piece = moves[i][0], capture = moves[i][3];
                uint64_t fromVal = 1ULL << moves[i][1], toVal = 1ULL << moves[i][2];
                
                b.move(piece, fromVal, toVal, capture, turn);
                int score = -alphaBetaNegaMax(-beta, -alpha, depth-1, !turn);
                if (score > alpha) {
                    alpha = score;
                    alphaIndex = i;
                }
                b.unmove(piece, fromVal, toVal, capture, turn);
            }
            return {moves[alphaIndex], {alpha}};
        }
        int evaluate () {

            //material eval
            uint64_t wKing = b.getK(), bKing = b.getk();
            int kingDiff = __builtin_popcountll(wKing) - __builtin_popcountll(bKing);
            if (kingDiff != 0) {
                return 100000*kingDiff;
            }
            uint64_t wKnights = b.getN(), bKnights = b.getn();
            uint64_t wBishops = b.getB(), bBishops = b.getb();
            uint64_t wRooks   = b.getR(), bRooks   = b.getr();
            uint64_t wQueens  = b.getQ(), bQueens  = b.getq();
            uint64_t wPawns   = b.getP(), bPawns   = b.getp();

            int knightDiff = __builtin_popcountll(wKnights) - __builtin_popcountll(bKnights);
            int bishopDiff = __builtin_popcountll(wBishops) - __builtin_popcountll(bBishops);
            int rookDiff   = __builtin_popcountll(wRooks)   - __builtin_popcountll(bRooks);
            int pawnDiff   = __builtin_popcountll(wPawns)   - __builtin_popcountll(bPawns);
            int queenDiff  = __builtin_popcountll(wQueens)  - __builtin_popcountll(bQueens);
            int materialEval = 100*pawnDiff + 305*knightDiff + 333*bishopDiff + 563*rookDiff + 950*queenDiff;

            //positional eval
            int wPieces = __builtin_popcountll(b.getColor(true));
            int bPieces = __builtin_popcountll(b.getColor(false));
            int positionEval = 0;
            bool isEndGame = wPieces + bPieces < 8 || (b.getQ()==0 && b.getq()==0 && wPieces + bPieces < 16);

            while (wPawns != 0)   { positionEval += pawnEvalTable   [isEndGame][bitScanForward(wPawns)];      wPawns &= wPawns-1; }
            while (bPawns != 0)   { positionEval -= pawnEvalTable   [isEndGame][64-bitScanForward(bPawns)];   bPawns &= bPawns-1; }
            while (wKnights != 0) { positionEval += knightEvalTable [isEndGame][bitScanForward(wKnights)];    wKnights &= wKnights-1; }
            while (bKnights != 0) { positionEval -= knightEvalTable [isEndGame][64-bitScanForward(bKnights)]; bKnights &= bKnights-1; }
            while (wBishops != 0) { positionEval += bishopEvalTable [isEndGame][bitScanForward(wBishops)];    wBishops &= wBishops-1; }
            while (bBishops != 0) { positionEval -= bishopEvalTable [isEndGame][64-bitScanForward(bBishops)]; bBishops &= bBishops-1; }
            while (wRooks != 0)   { positionEval += rookEvalTable   [isEndGame][bitScanForward(wRooks)];      wRooks &= wRooks-1; }
            while (bRooks != 0)   { positionEval -= rookEvalTable   [isEndGame][64-bitScanForward(bRooks)];   bRooks &= bRooks-1; }
            while (wQueens != 0)  { positionEval += queenEvalTable  [isEndGame][bitScanForward(wQueens)];     wQueens &= wQueens-1;}
            while (bQueens != 0)  { positionEval -= queenEvalTable  [isEndGame][64-bitScanForward(bQueens)];  bQueens &= bQueens-1;}
            positionEval += kingEvalTable[isEndGame][LOG2(wKing)];
            positionEval -= kingEvalTable[isEndGame][64-LOG2(bKing)];

            return materialEval + positionEval;
        }
    
    
    private:

        int quiesce (int alpha, int beta, bool turn) {
            int stand_pat = turn? evaluate() : -evaluate();
            nodes++;
            return stand_pat;
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
                int score = -quiesce(-beta, -alpha, !turn);
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
        int alphaBetaNegaMax(int alpha, int beta, int depth, int turn) {
            if (depth == 0) {
                return quiesce(alpha, beta, turn);
            }

            vector<vector<int>> moves = b.moveGen(turn);
            for (int i=0; i<moves.size(); i++) {
                int piece = moves[i][0], capture = moves[i][3];
                uint64_t fromVal = 1ULL << moves[i][1], toVal = 1ULL << moves[i][2];
                
                b.move(piece, fromVal, toVal, capture, turn);
                int score = -alphaBetaNegaMax(-beta, -alpha, depth-1, !turn);
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

};


#endif