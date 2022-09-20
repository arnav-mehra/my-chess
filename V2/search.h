#pragma once

class Search {
public:
    Board b;

    int nodes=0;
    int transposedNodes=0; 

    S16 eval=0;
    U8 quiescenceDepth=0;

    bool useTranspositionTable;
    bool useQuiescentSearch;
    MoveList moveLists[100];

    Search(Board board, bool useTT, bool useQui) {
        b = board;
        useTranspositionTable = useTT;
        useQuiescentSearch = useQui;
    }

    void runPerft(U8 depth) {
        auto t0 = chrono::system_clock::now();
        int res = perft(depth);
        auto tf = chrono::system_clock::now();
        double knodes = res * 0.001;
        std::chrono::duration<double> diff = tf - t0;
        double seconds = diff.count();

        cout << '\n';
        cout << res << " NODES\n";
        cout << seconds << " SECONDS\n";
        cout << knodes / seconds << " kNPS";
    }
    
    //plain call to negamax
    Move abNegaMaxCall() {
        nodes = 0;
        S16 alpha = -10000, beta = 10000;
        S8 alphaIndex = -1;
        
        MoveList moves = legalMoveGen();

        for (S8 i=0; i<moves.size(); i++) {
            b.move(moves[i]);
            S16 score = -alphaBetaNegaMax(-beta, -alpha, DEPTH-1);
            b.unmove(moves[i]);
            
            if (score > alpha) {
                alpha = score;
                alphaIndex = i;
            }
        }

        if (alphaIndex == -1) { // no move found
            return Move(b.isCheck());
        } else {
            eval = alpha;
            return moves[alphaIndex];
        }
    }

    //tries to find the best move given a runtime, iterative deepening in my ass
    Move getBestMoveTimed(int maxTime) {

        int depth = 1;
        MoveList possibleMoves = legalMoveGen();
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
        MoveList possibleMoves = legalMoveGen();
        int bestMoveIndex = 0;

        for (int depth=1; depth <= targetDepth; depth++) {
            vector<S16> m = abNegaMaxPreferredCall(depth, possibleMoves, bestMoveIndex);
            bestMoveIndex = m[0];
            eval = m[1];
        }
        return possibleMoves[bestMoveIndex];
    }
    
    //legal move gen
    MoveList legalMoveGen() {
        
        MoveList legalMoves;
        bool currentlyCheck = b.isCheck();

        MoveList moves;
        b.moveGen(moves);

        for (S8 i=0; i<moves.size(); i++) {
            b.move(moves[i]);
            
            b.toggleTurn();
            if (!(b.isCheck())) { //move is legal.compute eval
                b.toggleTurn();
                legalMoves.push(moves[i]);
            } else {
                b.toggleTurn();
            }

            b.unmove(moves[i]);
        }

        return legalMoves;            
    }

    // print eval, depth, etc.
    void printStats() {
        cout << '\n' << "Nodes Evaluated: " << nodes << '\n';
        cout << "Nodes Transposed: " << transposedNodes << '\n';
        cout << "Evaluation: " << eval/100.0 << '\n';
        cout << "Depth: " << DEPTH+0 << " / " << DEPTH + quiescenceDepth << '\n';
    }

private:
    // all possibilities, more for benchmarking
    int perft(U8 depth) {
        if (depth == 0) { return 1; }

        MoveList& moves = moveLists[depth];
        moves.clear();
        b.moveGen(moves);

        int n = 0;
        for (S8 i = 0; i < moves.size(); i++) {
            b.move(moves[i]);
            n += perft(depth - 1);
            b.unmove(moves[i]);
        }
        return n;
    }

    // capture sequences
    S16 quiesce(S16 alpha, S16 beta, U8 extendedDepth) {
        if (extendedDepth > quiescenceDepth) { quiescenceDepth = extendedDepth; }            

        S16 stand_pat = b.evaluate(); nodes++;
        if (stand_pat >= beta) { return beta; }
        if (alpha < stand_pat) { alpha = stand_pat; }

        MoveList moves;
        b.captureGen(moves);

        for (S8 i=0; i<moves.size(); i++) {
            b.move(moves[i]);
            S16 score = -quiesce(-beta, -alpha, extendedDepth+1);
            // if (useTranspositionTable) {
            //     //lowest accuracy eval, try to replace with known eval
            //     U64 boardKey(b.getBoardKey());
            //     if (boardEvalTable.count(boardKey)) {
            //         transposedNodes++;
            //         score = (b.turn? 1: -1) * (boardEvalTable.find(boardKey).second)[0];
            //     } else {
            //         score = -quiesce(-beta, -alpha, extendedDepth+1);
            //     }
            // } else {
            //     score = -quiesce(-beta, -alpha, extendedDepth+1);
            // }
            b.unmove(moves[i]);

            if (score >= beta) { return beta; }
            if (score > alpha) { alpha = score; }
        }

        return alpha;
    }

    //negamax
    S16 alphaBetaNegaMax(S16 alpha, S16 beta, U8 depth) {
        
        if (depth == 0) { 
            if (useQuiescentSearch) {
                return quiesce(alpha, beta, 0);
            } else {
                nodes++; 
                return b.evaluate();
            }
        }

        

        MoveList moves;
        b.moveGen(moves);
        if (moves.size() == 0) { return 0; }

        for (S8 i=0; i<moves.size(); i++) {
            b.move(moves[i]);
            S16 score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
            // if (useTranspositionTable) {
            //     U64 boardKey(b.getBoardKey());
            //     if (!boardEvalTable.count(boardKey)) { // no key found, calc and add to table
            //         score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
            //         boardEvalTable.insert(pair<U64, array<int,2>>(boardKey, {b.turn? score : -score, depth}));
            //     } else {
            //         // transposedNodes++;
            //         array<int, 2> foundVal(boardEvalTable.find(boardKey).second);
            //         if (foundVal[1] >= depth) { //foundVal remaining depth was higher, so more accurate.use eval
            //             score = (b.turn? 1: -1) * foundVal[0]; transposedNodes++;
            //         } else { // less accurate.calc eval and replace existing
            //             score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
            //             boardEvalTable.find(boardKey).second = {b.turn? score : -score, depth};
            //         }
            //         score = b.turn? (boardEvalTable.find(boardKey).second)[0] : -(boardEvalTable.find(boardKey).second)[0];
            //         // score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
            //     }
            // } else {
            //     score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
            // }
            b.unmove(moves[i]);

            if (score >= beta) { return beta; }
            if (score > alpha) { alpha = score; }
        }

        return alpha;
    }
    
    //for iterative deepening, takes in the current best move to eval first
    vector<S16> abNegaMaxPreferredCall(U8 depth, MoveList &moves, int currBestIndex) {
        S16 alpha = -10000, beta = 10000;
        S8 alphaIndex = -1;

        //check currBest first for move ordering
        b.move(moves[currBestIndex]);
        S16 currScore = -alphaBetaNegaMax(-beta, -alpha, depth-1);
        if (currScore > alpha) {
            alpha = currScore;
            alphaIndex = currBestIndex;
        }
        b.unmove(moves[currBestIndex]);


        for (S8 i=0; i<moves.size(); i++) {
            if (i != currBestIndex) {
                b.move(moves[i]);
                S16 score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
                if (score > alpha) {
                    alpha = score;
                    alphaIndex = i;
                }
                b.unmove(moves[i]);
            }
        }

        return {alphaIndex, alpha};
    }
};


//ab nega, d=0
// if (useTranspositionTable) {
//     // medium accuracy, add to table or use precalc eval
//     array<U64,7> boardKey(b.getBoardKey());
//     if (boardEvalTable.count(boardKey)) {
//         transposedNodes++;
//         return (b.turn? 1: -1) * (boardEvalTable.find(boardKey).second)[0];
//     } else {
//         S16 score = quiesce(alpha, beta, 0);
//         boardEvalTable.insert(pair<array<U64,7>, array<int,2>>(boardKey, {b.turn? score : -score, 0}));
//         return score;
//     }
// } else {
//     return quiesce(alpha, beta, 0); 
// }

//ab nega
// if (useTranspositionTable) {
//     array<U64,7> boardKey(b.getBoardKey());
//     if (!boardEvalTable.count(boardKey)) { // no key found, calc and add to table
//         score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
//         boardEvalTable.insert(pair<array<U64,7>, array<int,2>>(boardKey, {b.turn? score : -score, depth}));
//     } else {
//         array<int, 2> foundVal(boardEvalTable.find(boardKey).second);
//         if (foundVal[1] > depth) { //foundVal remaining depth was higher, so more accurate.use eval
//             score = (b.turn? 1: -1) * foundVal[0]; transposedNodes++;
//         } else { // less accurate.calc eval and replace existing
//             score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
//             boardEvalTable.find(boardKey).second = {b.turn? score : -score, depth};
//         }
//         score = b.turn? (boardEvalTable.find(boardKey).second)[0] : -(boardEvalTable.find(boardKey).second)[0];
//         score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
//     }
// } else {
//     score = -alphaBetaNegaMax(-beta, -alpha, depth-1);
// }


//quies
// if (useTranspositionTable) {
//     //lowest accuracy eval, try to replace with known eval
//     array<U64,7> boardKey(b.getBoardKey());
//     if (boardEvalTable.count(boardKey)) {
//         transposedNodes++;
//         score = (b.turn? 1: -1) * (boardEvalTable.find(boardKey).second)[0];
//     } else {
//         score = -quiesce(-beta, -alpha, extendedDepth+1);
//     }
// } else {
//     score = -quiesce(-beta, -alpha, extendedDepth+1);
// }