#ifndef SEARCH_H
#define SEARCH_H
#include <chrono>
using namespace std;


class Search {

    public:

        Board* b;
        int nodes=0; int16_t eval=0; uint8_t quiescenceDepth=0;
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
                int16_t score;
                if (useTranspositionTable) {
                    pair<array<uint64_t,7>, int> keyFound(b -> getBoardKey());
                    // eval=11000 -> was not found or added, eval!=11000 -> was found and returned real eval
                    if (keyFound.second == 11000) {
                        score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
                        addBoardEvalPair(pair<array<uint64_t,7>, int>(keyFound.first, score), b -> turn);
                    } else {
                        score = keyFound.second;
                    }
                } else {
                    score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
                }
                if (score > alpha) {
                    alpha = score;
                    alphaIndex = i;
                }
                b -> unmove(moves[i]);
            }

            if (alphaIndex == -1) { // no move found
                return Move(b -> isCheck(65));
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
            // while (tdiff < maxTime + depth*300) {
            //     depth++;
            //     bestMoveIndex = abNegaMaxPreferredCall(depth, possibleMoves, bestMoveIndex);
            //     // chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - t1).count();
                
            //     tdiff =  chrono::duration_cast<chrono::milliseconds>(
            //         chrono::system_clock::now().time_since_epoch()-t1
            //     ).count();
            // }

            return possibleMoves[bestMoveIndex];
        }
        
        //legal move gen
        vector<Move> legalMoveGen() {
            
            vector<Move> legalMoves;
            bool currentlyCheck = b -> isCheck(65);

            vector<Move> moves;
            b -> moveGen(moves);

            for (int8_t i=0; i<moves.size(); i++) {
                b -> move(moves[i]);
                
                b -> toggleMove();
                if (!(b -> isCheck(65))) { //move is legal -> compute eval
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

            for (int8_t i=0; i<moves.size(); i++)  {
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
                int16_t score;
                if (useTranspositionTable) {
                    pair<array<uint64_t,7>, int> keyFound(b -> getBoardKey());
                    // eval=11000 -> was not found or added, eval!=11000 -> was found and returned real eval
                    if (keyFound.second == 11000) {
                        score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
                        addBoardEvalPair(pair<array<uint64_t,7>, int>(keyFound.first, score), b -> turn);
                    } else {
                        score = keyFound.second;
                    }
                } else {
                    score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
                }
                b -> unmove(moves[i]);

                if (score >= beta) { return beta; }
                if (score > alpha) { alpha = score; }
            }

            return alpha;
        }
        
        //for iterative deepening, takes in the current best move to eval first
        int abNegaMaxPreferredCall(uint8_t depth, vector<Move> &moves, int currBestIndex) {
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

            return alphaIndex;
        }
};

#endif