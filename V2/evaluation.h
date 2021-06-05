using namespace std;
#define addEvals(p, sq) {         \
    midEval += mg_table[p][sq];   \
    endEval += eg_table[p][sq];   \
    gamePhase += gamephaseInc[p]; \
}



int16_t Board::evaluate() {

    //king precondition
    if (getK() == 0) { return -10000; }
    if (getk() == 0) { return 10000; }

    
    // peSTO's
    uint64_t wKnights = getN(); uint64_t bKnights = getn();
    uint64_t wBishops = getB(); uint64_t bBishops = getb();
    uint64_t wRooks   = getR(); uint64_t bRooks   = getr();
    uint64_t wQueens  = getQ(); uint64_t bQueens  = getq();
    uint64_t wPawns   = getP(); uint64_t bPawns   = getp();

    int midEval=0;
    int endEval=0;
    int gamePhase=0;

    while (wPawns != 0)   { addEvals(0, lsb(wPawns));   wPawns &= wPawns-1;     }
    while (bPawns != 0)   { addEvals(6, lsb(bPawns));   bPawns &= bPawns-1;     }
    while (wKnights != 0) { addEvals(1, lsb(wKnights)); wKnights &= wKnights-1; }
    while (bKnights != 0) { addEvals(7, lsb(bKnights)); bKnights &= bKnights-1; }
    while (wBishops != 0) { addEvals(2, lsb(wBishops)); wBishops &= wBishops-1; }
    while (bBishops != 0) { addEvals(8, lsb(bBishops)); bBishops &= bBishops-1; }
    while (wRooks != 0)   { addEvals(3, lsb(wRooks));   wRooks &= wRooks-1;     }
    while (bRooks != 0)   { addEvals(9, lsb(bRooks));   bRooks &= bRooks-1;     }
    while (wQueens != 0)  { addEvals(4, lsb(wQueens));  wQueens &= wQueens-1;   }
    while (bQueens != 0)  { addEvals(10,lsb(bQueens));  bQueens &= bQueens-1;   }
    addEvals(5, lsb(getK())); addEvals(11, lsb(getk()));

    if (gamePhase > 24) { gamePhase = 24; }
    return (turn? 1: -1) * (midEval * gamePhase + endEval * (24 - gamePhase))/24;
}








// purely materialistic
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