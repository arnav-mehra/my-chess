using namespace std;


int16_t Board::evaluate() {

    //king precondition
    if (getK() == 0) { return -10000; }
    if (getk() == 0) { return 10000; }

    int16_t eval = 0;

    uint64_t wKnights = getN(); uint64_t bKnights = getn();
    uint64_t wBishops = getB(); uint64_t bBishops = getb();
    uint64_t wRooks   = getR(); uint64_t bRooks   = getr();
    uint64_t wQueens  = getQ(); uint64_t bQueens  = getq();
    uint64_t wPawns   = getP(); uint64_t bPawns   = getp();

    uint8_t const numOfPieces = __builtin_popcountll(pieces[Piece::WHITE] | pieces[Piece::BLACK]);
    bool const isEndGame = (numOfPieces < 8) || (wQueens==0 && bQueens==0 && numOfPieces < 16);

    while (wPawns != 0)   { eval += getPawnEvalTable   (isEndGame, lsb(wPawns))      + 100; wPawns &= wPawns-1;     }
    while (bPawns != 0)   { eval -= getPawnEvalTable   (isEndGame, 64-lsb(bPawns))   + 100; bPawns &= bPawns-1;     }
    while (wKnights != 0) { eval += getKnightEvalTable (isEndGame, lsb(wKnights))    + 305; wKnights &= wKnights-1; }
    while (bKnights != 0) { eval -= getKnightEvalTable (isEndGame, 64-lsb(bKnights)) + 305; bKnights &= bKnights-1; }
    while (wBishops != 0) { eval += getBishopEvalTable (isEndGame, lsb(wBishops))    + 333; wBishops &= wBishops-1; }
    while (bBishops != 0) { eval -= getBishopEvalTable (isEndGame, 64-lsb(bBishops)) + 333; bBishops &= bBishops-1; }
    while (wRooks != 0)   { eval += getRookEvalTable   (isEndGame, lsb(wRooks))      + 563; wRooks &= wRooks-1;     }
    while (bRooks != 0)   { eval -= getRookEvalTable   (isEndGame, 64-lsb(bRooks))   + 563; bRooks &= bRooks-1;     }
    while (wQueens != 0)  { eval += getQueenEvalTable  (isEndGame, lsb(wQueens))     + 950; wQueens &= wQueens-1;   }
    while (bQueens != 0)  { eval -= getQueenEvalTable  (isEndGame, 64-lsb(bQueens))  + 950; bQueens &= bQueens-1;   }
    eval += getKingEvalTable(isEndGame, lsb(getK())) - getKingEvalTable(isEndGame, 64-lsb(getk()));

    return turn? eval : -eval;
}