using namespace std;


void Board::moveGen (vector<Move> &moves) {
    //king precondition
    if ((turn? getK() : getk()) == 0) { return; }
    
    //gen moves
    genCastlingMoves(moves);
    genPawnMoves(moves, turn? getP() : getp());
    genKnightMoves (moves, turn? getN() : getn());
    genBishopMoves (moves, turn? getB() : getb());
    genQueenMoves (moves, turn? getQ() : getq());
    genRookMoves (moves, turn? getR() : getr());
    genKingMoves (moves, lsb(turn? getK() : getk()));
    
    //sort moves
    sort(moves.begin(), moves.end(), [] (Move& move1, Move& move2) {
        return move1.priority > move2.priority;
    });
}


void Board::genPawnMoves (vector<Move> &moves, uint64_t cPawns) {
    
    uint64_t up1 = (turn? cPawns >> 8 : cPawns << 8) & getUnoccupied();
    uint64_t up2 = (turn? up1 >> 8 : up1 << 8) & getUnoccupied() & getPawnDouble(turn);

    while (cPawns != 0) {
        uint64_t attacks = getPawnAttacks(turn, lsb(cPawns)) & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(0, lsb(cPawns), lsb(attacks), getCapturedPiece(lsb(attacks)))
            );
        attacks &= attacks-1;
        }
        cPawns &= cPawns-1;
    }

    //pawn forward 2
    while (up2 != 0) {
        moves.push_back(Move(0, lsb(up2) + (turn? 16 : -16), lsb(up2)));
        up2 &= up2-1;
    }

    //pawn moves 1
    while (up1 != 0) {
        uint8_t toSquare = lsb(up1);
        if (turn? toSquare < 8 : toSquare > 55) {
            moves.push_back(Move(8, turn? toSquare+8 : toSquare-8, toSquare));
            // moves.push_back({9, fromSquare, toSquare, -1});
            // moves.push_back({10, fromSquare, toSquare, -1});
            moves.push_back(Move(11, turn? toSquare+8 : toSquare-8, toSquare));
        } else {
            moves.push_back(Move(0, turn? toSquare+8 : toSquare-8, toSquare));
        }
        up1 &= up1-1;
    }
}
void Board::genCastlingMoves (vector<Move> &moves) {
    if (turn && wKingSqMoves==0) {
        if (wLRookSqMoves==0 && canLeftCastle()) {
            moves.push_back(Move(6, 60, 58));
        }
        if (wRRookSqMoves==0 && canRightCastle()) {
            moves.push_back(Move(7, 60, 62));
        }
    } else if (!turn && bKingSqMoves==0) {
        if (bLRookSqMoves==0 && canLeftCastle()) {
            moves.push_back(Move(6, 4, 2));
        }
        if (bRRookSqMoves==0 && canRightCastle()) {
            moves.push_back(Move(7, 4, 6));
        }
    }
}
void Board::genKnightMoves (vector<Move> &moves, uint64_t cKnights) {
    while (cKnights != 0) {
        uint8_t currSquare = lsb(cKnights);
        uint64_t attacks = knightMoves[currSquare] & getNotTurn();
        uint64_t openMoves = knightMoves[currSquare] & getUnoccupied();
        while (attacks != 0) {
            moves.push_back(
                Move(1, currSquare, lsb(attacks), getCapturedPiece(lsb(attacks)))
            );
            attacks &= attacks-1;
        }
        while (openMoves != 0) {
            moves.push_back(Move(1, currSquare, lsb(openMoves)));
            openMoves &= openMoves-1;
        }
        cKnights &= cKnights-1;
    }
}
void Board::genBishopMoves (vector<Move> &moves, uint64_t cBishops) {
    while (cBishops != 0) {
        uint64_t bishopMoves = Bmagic(lsb(cBishops), getOccupied());
        uint64_t attacks = bishopMoves & getNotTurn();
        uint64_t openMoves = bishopMoves & getUnoccupied();
        while (attacks != 0) {
            moves.push_back(
                Move(2, lsb(cBishops), lsb(attacks), getCapturedPiece(lsb(attacks)))
            );
            attacks &= attacks-1;
        }
        while (openMoves != 0) {
            moves.push_back(Move(2, lsb(cBishops), lsb(openMoves)));
            openMoves &= openMoves-1;
        }
        cBishops &= cBishops-1;
    }
}
void Board::genRookMoves (vector<Move> &moves, uint64_t cRooks) {
    while (cRooks != 0) {
        uint64_t rookMoves = Rmagic(lsb(cRooks), getOccupied());
        uint64_t attacks = rookMoves & getNotTurn();
        uint64_t openMoves = rookMoves & getUnoccupied();
        while (attacks != 0) {
            moves.push_back(
                Move(3, lsb(cRooks), lsb(attacks), getCapturedPiece(lsb(attacks)))
            );
          attacks &= attacks-1;
        }
        while (openMoves != 0) {
          moves.push_back(Move(3, lsb(cRooks), lsb(openMoves)));
          openMoves &= openMoves-1;
        }
        cRooks &= cRooks-1;
    }
}
void Board::genQueenMoves (vector<Move> &moves, uint64_t cQueens) {
    while (cQueens != 0) {
        uint8_t currSquare = lsb(cQueens);
        uint64_t queenMoves = Bmagic(currSquare, getOccupied()) ^ Rmagic(currSquare, getOccupied());
        uint64_t attacks = queenMoves & getNotTurn();
        uint64_t openMoves = queenMoves & getUnoccupied();
        while (attacks != 0) {
            moves.push_back(
                Move(4, currSquare, lsb(attacks), getCapturedPiece(lsb(attacks)))
            );
            attacks &= attacks-1;
        }
        while (openMoves != 0) {
            moves.push_back(Move(4, currSquare, lsb(openMoves)));
            openMoves &= openMoves-1;
        }
        cQueens &= cQueens-1;
    }
}
void Board::genKingMoves (vector<Move> &moves, uint8_t kingSquare) {
    uint64_t attacks = kingMoves[kingSquare] & getNotTurn();
    uint64_t openMoves = kingMoves[kingSquare] & getUnoccupied();
    while (attacks != 0) {
        moves.push_back(
            Move(5, kingSquare, lsb(attacks), getCapturedPiece(lsb(attacks)))
        );
        attacks &= attacks-1;
    }
    while (openMoves != 0) {
        moves.push_back(Move(5, kingSquare, lsb(openMoves)));
        openMoves &= openMoves-1;
    }
}









