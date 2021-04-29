#ifndef SEARCH_H
#define SEARCH_H
#define LOG2(X) ((unsigned) (8*sizeof (unsigned long long) - __builtin_clzll((X)) - 1))

using namespace std;


class Search {

    public:

        Board* b;
        int nodes=0, eval=0;
        
        Search(Board board) {
            b = &board;
        }


        int perft(uint8_t depth, bool turn) {
            int n = 0;
            if (depth == 0) { return 1; }

            vector<Move> moves;
            b -> moveGen(turn, moves);

            for (uint8_t i=0; i<moves.size(); i++) {                
                b -> move(moves[i], turn);
                n += perft(depth-1, !turn);
                b -> unmove(moves[i], turn);
            }

            return n;
        }

        Move abNegaMaxCall(uint8_t depth, bool turn) {
            nodes = 0;
            int16_t alpha = -10000, beta = 10000;
            uint8_t alphaIndex = -1;

            bool currentlyCheck = b -> isCheck(turn, 65);

            vector<Move> moves;
            b -> moveGen(turn, moves);

            for (uint8_t i=0; i<moves.size(); i++) {
                b -> move(moves[i], turn);

                if (!(b -> isCheck(turn, 65))) { //move is legal -> compute eval
                    int16_t score = -alphaBetaNegaMax(-beta, -alpha, depth-1, !turn);
                    if (score > alpha) {
                        alpha = score;
                        alphaIndex = i;
                    }
                }

                b -> unmove(moves[i], turn);
            }

            if (alphaIndex == -1) { // no move found
                return Move(currentlyCheck);
            } else {
                eval = alpha;
                return moves[alphaIndex];
            }
        }

        vector<Move> legalMoveGen(bool turn) {
            vector<Move> legalMoves;

            bool currentlyCheck = b -> isCheck(turn, 65);

            vector<Move> moves;
            b -> moveGen(turn, moves);

            for (uint8_t i=0; i<moves.size(); i++) {
                b -> move(moves[i], turn);

                if (!(b -> isCheck(turn, 65))) { //move is legal -> compute eval
                    legalMoves.push_back(moves[i]);
                }

                b -> unmove(moves[i], turn);
            }

            if (legalMoves.size() == 0) {
                return { Move(currentlyCheck) };
            } else {
                return legalMoves;
            }
            
        }



    private:

        int16_t quiesce(int16_t alpha, int16_t beta, bool turn) {
            int16_t stand_pat = turn? b -> evaluate() : -b -> evaluate();
            if (stand_pat >= beta) { nodes++; return beta; }
            if (alpha < stand_pat) { alpha = stand_pat; }

            vector<Move> moves;
            b -> captureGen(turn, moves);

            for (uint8_t i=0; i<moves.size(); i++)  {
                b -> move(moves[i], turn);
                int16_t score = -quiesce(-beta, -alpha, !turn);
                b -> unmove(moves[i], turn);

                if (score >= beta) { nodes++; return beta; }
                if (score > alpha) { alpha = score; }
            }

            nodes++;
            return alpha;
        }

        int16_t alphaBetaNegaMax(int16_t alpha, int16_t beta, uint8_t depth, bool turn) {
            if (depth == 0) { return quiesce(alpha, beta, turn); }

            vector<Move> moves;
            b -> moveGen(turn, moves);

            if (moves.size() == 0) { return 0; }
            for (uint8_t i=0; i<moves.size(); i++) {
                b -> move(moves[i], turn);
                int16_t score = -alphaBetaNegaMax(-beta, -alpha, depth-1, !turn);
                b -> unmove(moves[i], turn);

                if (score >= beta) { return beta; }
                if (score > alpha) { alpha = score; }
            }

            return alpha;
        }

};

#endif