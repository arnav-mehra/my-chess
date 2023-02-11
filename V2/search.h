#ifndef SEARCH_H
#define SEARCH_H
#include <chrono>
using namespace std;


class Search {

    public:

        Board* b;
        int nodes=0; int transposedNodes=0; 
        int16_t eval=0; uint8_t quiescenceDepth=0;
        bool useTranspositionTable; bool useQuiescentSearch;
        
        

        Search(Board board, bool useTT, bool useQui) {
            b = &board;
            useTranspositionTable = useTT;
            useQuiescentSearch = useQui;
        }
        

        //all possibilities, more for benchmarking
        int perft(uint8_t depth) {
            int n = 0;
            if (depth == 0) { return 1; }

            vector<Move> moves;
            b -> moveGen(moves);
            
            for (int8_t i=0; i<moves.size(); i++) {
                b -> move(moves[i]);
                n += perft(depth-1);
                b -> unmove(moves[i]);
            }

            return n;
        }
        
        //plain call to negamax
        Move abNegaMaxCall(uint8_t depth) {
            nodes = 0;
            int16_t alpha = -10000, beta = 10000;
            int8_t alphaIndex = -1;
            
            vector<Move> moves = legalMoveGen();

            for (int8_t i=0; i<moves.size(); i++) {
                b -> move(moves[i]);
                int16_t score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
                b -> unmove(moves[i]);
                
                if (score > alpha) {
                    alpha = score;
                    alphaIndex = i;
                }
            }

            if (alphaIndex == -1) { // no move found
                return Move(b -> isCheck());
            } else {
                eval = alpha;
                return moves[alphaIndex];
            }
        }

        //tries to find the best move given a runtime, iterative deepening in my ass
        Move getBestMoveTimed(int maxTime) {

            int depth = 1;
            vector<Move> possibleMoves = legalMoveGen();
            int bestMoveIndex = 0;

            auto t1 = chrono::system_clock::now().time_since_epoch();
            cout << chrono::duration_cast<chrono::milliseconds>(t1 - t1).count();
            int tdiff = 0;

            //doesnt work!!!
            // while (tdiff < maxTime + depth*300) {
            //     depth++;
            //     // bestMoveIndex = abNegaMaxPreferredCall(depth, possibleMoves, bestMoveIndex);
            //     // chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - t1).count();
                
            //     tdiff =  chrono::duration_cast<chrono::milliseconds>(
            //         chrono::system_clock::now().time_since_epoch()-t1
            //     ).count();
            // }

            return possibleMoves[bestMoveIndex];
        }
        Move iterativeCall(int targetDepth) {
            vector<Move> possibleMoves = legalMoveGen();
            int bestMoveIndex = 0;

            for (int depth=1; depth <= targetDepth; depth++) {
                vector<int16_t> m = abNegaMaxPreferredCall(depth, possibleMoves, bestMoveIndex);
                bestMoveIndex = m[0];
                eval = m[1];
            }
            return possibleMoves[bestMoveIndex];
        }
        
        //legal move gen
        vector<Move> legalMoveGen() {
            
            vector<Move> legalMoves;
            bool currentlyCheck = b -> isCheck();

            vector<Move> moves;
            b -> moveGen(moves);

            for (int8_t i=0; i<moves.size(); i++) {
                b -> move(moves[i]);
                
                b -> toggleMove();
                if (!(b -> isCheck())) { //move is legal -> compute eval
                    b -> toggleMove();
                    legalMoves.push_back(moves[i]);
                } else {
                    b -> toggleMove();
                }

                b -> unmove(moves[i]);
            }

            return legalMoves;            
        }


    private:

        // capture sequences
        int16_t quiesce(int16_t alpha, int16_t beta, uint8_t extendedDepth) {
            if (extendedDepth > quiescenceDepth) { quiescenceDepth = extendedDepth; }            

            int16_t stand_pat = b -> evaluate(); nodes++;
            if (stand_pat >= beta) { return beta; }
            if (alpha < stand_pat) { alpha = stand_pat; }

            vector<Move> moves;
            b -> captureGen(moves);

            for (int8_t i=0; i<moves.size(); i++) {
                b -> move(moves[i]);
                int16_t score = -quiesce(-beta, -alpha, extendedDepth+1);
                b -> unmove(moves[i]);

                if (score >= beta) { return beta; }
                if (score > alpha) { alpha = score; }
            }

            return alpha;
        }

        //negamax
        int16_t alphaBetaNegaMax(int16_t alpha, int16_t beta, uint8_t depth) {
            
            if (depth == 0) { 
                if (useQuiescentSearch) {
                    return quiesce(alpha, beta, 0);
                } else {
                    nodes++; 
                    return b -> evaluate();
                }
            }

            vector<Move> moves;
            b -> moveGen(moves);
            if (moves.size() == 0) { return 0; }

            for (int8_t i=0; i<moves.size(); i++) {
                b -> move(moves[i]);
                int16_t score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
                b -> unmove(moves[i]);

                if (score >= beta) { return beta; }
                if (score > alpha) { alpha = score; }
            }

            return alpha;
        }
        
        //for iterative deepening, takes in the current best move to eval first
        vector<int16_t> abNegaMaxPreferredCall(uint8_t depth, vector<Move> &moves, int currBestIndex) {
            int16_t alpha = -10000, beta = 10000;
            int8_t alphaIndex = -1;

            //check currBest first for move ordering
            b -> move(moves[currBestIndex]);
            int16_t currScore = -alphaBetaNegaMax(-beta, -alpha, depth-1);
            if (currScore > alpha) {
                alpha = currScore;
                alphaIndex = currBestIndex;
            }
            b -> unmove(moves[currBestIndex]);


            for (int8_t i=0; i<moves.size(); i++) {
                if (i != currBestIndex) {
                    b -> move(moves[i]);
                    int16_t score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
                    if (score > alpha) {
                        alpha = score;
                        alphaIndex = i;
                    }
                    b -> unmove(moves[i]);
                }
            }

            return {alphaIndex, alpha};
        }
};

#endif


//ab nega, d=0
// if (useTranspositionTable) {
//     // medium accuracy, add to table or use precalc eval
//     array<uint64_t,7> boardKey(b -> getBoardKey());
//     if (boardEvalTable.count(boardKey)) {
//         transposedNodes++;
//         return (b -> turn? 1: -1) * (boardEvalTable.find(boardKey) -> second)[0];
//     } else {
//         int16_t score = quiesce(alpha, beta, 0);
//         boardEvalTable.insert(pair<array<uint64_t,7>, array<int,2>>(boardKey, {b -> turn? score : -score, 0}));
//         return score;
//     }
// } else {
//     return quiesce(alpha, beta, 0); 
// }

//ab nega
// if (useTranspositionTable) {
//     array<uint64_t,7> boardKey(b -> getBoardKey());
//     if (!boardEvalTable.count(boardKey)) { // no key found, calc and add to table
//         score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
//         boardEvalTable.insert(pair<array<uint64_t,7>, array<int,2>>(boardKey, {b -> turn? score : -score, depth}));
//     } else {
//         array<int, 2> foundVal(boardEvalTable.find(boardKey) -> second);
//         if (foundVal[1] > depth) { //foundVal remaining depth was higher, so more accurate -> use eval
//             score = (b -> turn? 1: -1) * foundVal[0]; transposedNodes++;
//         } else { // less accurate -> calc eval and replace existing
//             score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
//             boardEvalTable.find(boardKey) -> second = {b -> turn? score : -score, depth};
//         }
//         score = b -> turn? (boardEvalTable.find(boardKey) -> second)[0] : -(boardEvalTable.find(boardKey) -> second)[0];
//         score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
//     }
// } else {
//     score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
// }


//quies
// if (useTranspositionTable) {
//     //lowest accuracy eval, try to replace with known eval
//     array<uint64_t,7> boardKey(b -> getBoardKey());
//     if (boardEvalTable.count(boardKey)) {
//         transposedNodes++;
//         score = (b -> turn? 1: -1) * (boardEvalTable.find(boardKey) -> second)[0];
//     } else {
//         score = -quiesce(-beta, -alpha, extendedDepth+1);
//     }
// } else {
//     score = -quiesce(-beta, -alpha, extendedDepth+1);
// }
