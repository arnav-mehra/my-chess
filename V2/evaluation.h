#pragma once

S16 Board::evaluate() {
    //king precondition
    if (getK() == 0) { return -10000; }
    if (getk() == 0) { return 10000; }

    // return turn?
    //     (pop(pieces[Piece::W_PAWN])   - pop(pieces[Piece::B_PAWN]))   * 100 +
    //     (pop(pieces[Piece::W_KNIGHT]) - pop(pieces[Piece::B_KNIGHT])) * 305 +
    //     (pop(pieces[Piece::W_BISHOP]) - pop(pieces[Piece::B_BISHOP])) * 333 + 
    //     (pop(pieces[Piece::W_ROOK])   - pop(pieces[Piece::B_ROOK]))   * 563 +
    //     (pop(pieces[Piece::W_QUEEN])  - pop(pieces[Piece::B_QUEEN]))  * 950
    // :
    //     (pop(pieces[Piece::B_PAWN])   - pop(pieces[Piece::W_PAWN]))   * 100 +
    //     (pop(pieces[Piece::B_KNIGHT]) - pop(pieces[Piece::W_KNIGHT])) * 305 +
    //     (pop(pieces[Piece::B_BISHOP]) - pop(pieces[Piece::W_BISHOP])) * 333 + 
    //     (pop(pieces[Piece::B_ROOK])   - pop(pieces[Piece::W_ROOK]))   * 563 +
    //     (pop(pieces[Piece::B_QUEEN])  - pop(pieces[Piece::W_QUEEN]))  * 950
    // ;


    int midEval = 0;
    int endEval = 0;
    int gamePhase = 0;

    int sq;
    U64 pieceBitboard;
    for (int n = 0; n < 12; n++) {
        pieceBitboard = pieces[n];
        gamePhase += gamephaseInc[n] * pop(pieceBitboard);
        while (pieceBitboard != 0) {
            sq = lsb(pieceBitboard);
            midEval += mg_table[n][sq];
            endEval += eg_table[n][sq];
            pieceBitboard &= pieceBitboard-1;
        }
    }
    // // peSTO's
    // U64 wKnights = getN(); U64 bKnights = getn();
    // U64 wBishops = getB(); U64 bBishops = getb();
    // U64 wRooks   = getR(); U64 bRooks   = getr();
    // U64 wQueens  = getQ(); U64 bQueens  = getq();
    // U64 wPawns   = getP(); U64 bPawns   = getp();

    // // while (wPawns != 0) { addWhiteEvals(1, lsb(wPawns)); wPawns &= wPawns-1; }
    // // while (bKnights != 0) { addBlackEvals(7, lsb(bKnights)); bKnights &= bKnights-1; }
    // // while (wKnights != 0) { addWhiteEvals(1, lsb(wKnights)); wKnights &= wKnights-1; }
    // // while (bKnights != 0) { addBlackEvals(7, lsb(bKnights)); bKnights &= bKnights-1; }
    // // while (wBishops != 0) { addWhiteEvals(2, lsb(wBishops)); wBishops &= wBishops-1; }
    // // while (bBishops != 0) { addBlackEvals(8, lsb(bBishops)); bBishops &= bBishops-1; }
    // // while (wRooks != 0)   { addWhiteEvals(3, lsb(wRooks));   wRooks &= wRooks-1;     }
    // // while (bRooks != 0)   { addBlackEvals(9, lsb(bRooks));   bRooks &= bRooks-1;     }
    // // while (wQueens != 0)  { addWhiteEvals(4, lsb(wQueens));  wQueens &= wQueens-1;   }
    // // while (bQueens != 0)  { addBlackEvals(10,lsb(bQueens));  bQueens &= bQueens-1;   }
    // // addWhiteEvals(5, lsb(getK())); addBlackEvals(11, lsb(getk()));

    if (gamePhase > 24) { gamePhase = 24; }
    return (turn? 1: -1) * (midEval * gamePhase + endEval * (24 - gamePhase))/24;
}