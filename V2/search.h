#ifndef SEARCH_H
#define SEARCH_H
#include <chrono>
using namespace std;


class Search {

    public:

        Board* b;
        int nodes=0, eval=0;
        Search(Board board) {
            b = &board;
        }


        //all possibilities, more for benchmarking
        int perft(uint8_t depth) {
            int n = 0;
            if (depth == 0) { return 1; }

            vector<Move> moves;
            b -> moveGen(moves);

            for (uint8_t i=0; i<moves.size(); i++) {
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
            uint8_t alphaIndex = -1;
            
            bool currentlyCheck = b -> isCheck(65);
            
            vector<Move> moves;
            b -> moveGen(moves);

            for (uint8_t i=0; i<moves.size(); i++) {
                b -> move(moves[i]);

                b -> toggleMove();
                if (!(b -> isCheck(65))) { //move is legal -> compute eval
                    b -> toggleMove();
                    int16_t score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
                    if (score > alpha) {
                        alpha = score;
                        alphaIndex = i;
                    }
                } else {
                    b -> toggleMove();
                }
                
                b -> unmove(moves[i]);
            }

            if (alphaIndex == -1) { // no move found
                return Move(currentlyCheck);
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
        


    private:

        // capture sequences
        int16_t quiesce(int16_t alpha, int16_t beta) {
            int16_t stand_pat = b -> evaluate();
            if (stand_pat >= beta) { nodes++; return beta; }
            if (alpha < stand_pat) { alpha = stand_pat; }

            vector<Move> moves;
            b -> captureGen(moves);

            for (uint8_t i=0; i<moves.size(); i++)  {
                b -> move(moves[i]);
                int16_t score = -quiesce(-beta, -alpha);
                b -> unmove(moves[i]);

                if (score >= beta) { nodes++; return beta; }
                if (score > alpha) { alpha = score; }
            }

            nodes++;
            return alpha;
        }

        //negamax
        int16_t alphaBetaNegaMax(int16_t alpha, int16_t beta, uint8_t depth) {
            if (depth == 0) { return quiesce(alpha, beta); }

            vector<Move> moves;
            b -> moveGen(moves);

            if (moves.size() == 0) { return 0; }
            for (uint8_t i=0; i<moves.size(); i++) {
                b -> move(moves[i]);
                int16_t score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
                b -> unmove(moves[i]);

                if (score >= beta) { return beta; }
                if (score > alpha) { alpha = score; }
            }

            return alpha;
        }
        
        //legal move gen
        vector<Move> legalMoveGen() {
            
            vector<Move> legalMoves;
            bool currentlyCheck = b -> isCheck(65);

            vector<Move> moves;
            b -> moveGen(moves);

            for (uint8_t i=0; i<moves.size(); i++) {
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

            if (legalMoves.size() == 0) {
                return { Move(currentlyCheck) };
            } else {
                return legalMoves;
            }
            
        }
        
        //for iterative deepening, takes in the current best move to eval first
        int abNegaMaxPreferredCall(uint8_t depth, vector<Move> &moves, int currBestIndex) {
            int16_t alpha = -10000, beta = 10000;
            uint8_t alphaIndex = -1;

            //check currBest first for move ordering
            b -> move(moves[currBestIndex]);
            int16_t currScore = -alphaBetaNegaMax(-beta, -alpha, depth-1);
            if (currScore > alpha) {
                alpha = currScore;
                alphaIndex = currBestIndex;
            }
            b -> unmove(moves[currBestIndex]);


            for (uint8_t i=0; i<moves.size(); i++) {
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