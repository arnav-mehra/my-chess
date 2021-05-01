using namespace std;


//move generation

void Board::moveGen (vector<Move> &moves) {
    
    //king precondition
    
    uint64_t cKing = turn? getK() : getk();
    if (cKing == 0) { printBoard(); return; }
    
    // castling
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
      
    //piece positions
    uint64_t cPawns = turn? getP() : getp();
    uint64_t cKnights = turn? getN() : getn(); uint64_t cBishops = turn? getB() : getb();
    uint64_t cRooks = turn? getR() : getr(); uint64_t cQueen = turn? getQ() : getq();
    uint64_t up1 = (turn? cPawns >> 8 : cPawns << 8) & getUnoccupied();
    uint64_t up2 = (turn? up1 >> 8 : up1 << 8) & getUnoccupied() & getPawnDouble(turn);
    
    //pawn attacks/captures
    while (cPawns != 0) {
        uint64_t attacks = getPawnAttacks(turn, lsb(cPawns)) & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(
                    0, lsb(cPawns), lsb(attacks), 
                    getCapturedPiece(lsb(attacks))
                )
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

    //knight moves
    while (cKnights != 0) {
        uint8_t currSquare = lsb(cKnights);
        uint64_t attacks = knightMoves[currSquare] & getNotTurn();
        uint64_t openMoves = knightMoves[currSquare] & getUnoccupied();
        while (attacks != 0) {
          moves.push_back(Move(1, currSquare, lsb(attacks), getCapturedPiece(lsb(attacks))));
          attacks &= attacks-1;
        }
        while (openMoves != 0) {
          moves.push_back(Move(1, currSquare, lsb(openMoves)));
          openMoves &= openMoves-1;
        }
        cKnights &= cKnights-1;
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

    //bishop moves
    while (cBishops != 0) {
        uint64_t bishopMoves = Bmagic(lsb(cBishops), getOccupied());
        uint64_t attacks = bishopMoves & getNotTurn();
        uint64_t openMoves = bishopMoves & getUnoccupied();
        while (attacks != 0) {
          moves.push_back(Move(2, lsb(cBishops), lsb(attacks), getCapturedPiece(lsb(attacks))));
          attacks &= attacks-1;
        }
        while (openMoves != 0) {
          moves.push_back(Move(2, lsb(cBishops), lsb(openMoves)));
          openMoves &= openMoves-1;
        }
        cBishops &= cBishops-1;
    }

    //queen moves
    while (cQueen != 0) {
        uint8_t currSquare = lsb(cQueen);
        uint64_t queenMoves = Bmagic(currSquare, getOccupied()) ^ Rmagic(currSquare, getOccupied());
        uint64_t attacks = queenMoves & getNotTurn();
        uint64_t openMoves = queenMoves & getUnoccupied();
        while (attacks != 0) {
          moves.push_back(Move(4, currSquare, lsb(attacks), getCapturedPiece(lsb(attacks))));
          attacks &= attacks-1;
        }
        while (openMoves != 0) {
          moves.push_back(Move(4, currSquare, lsb(openMoves)));
          openMoves &= openMoves-1;
        }
        cQueen &= cQueen-1;
    }

    //rook moves
    while (cRooks != 0) {
        uint64_t rookMoves = Rmagic(lsb(cRooks), getOccupied());
        uint64_t attacks = rookMoves & getNotTurn();
        uint64_t openMoves = rookMoves & getUnoccupied();
        while (attacks != 0) {
          moves.push_back(Move(3, lsb(cRooks), lsb(attacks), getCapturedPiece(lsb(attacks))));
          attacks &= attacks-1;
        }
        while (openMoves != 0) {
          moves.push_back(Move(3, lsb(cRooks), lsb(openMoves)));
          openMoves &= openMoves-1;
        }
        cRooks &= cRooks-1;
    }

    //king moves
    // uint8_t kingSquare = lsb(turn? getK() : getk());
    // uint64_t attacks = kingMoves[kingSquare] & getNotTurn();
    // uint64_t openMoves = kingMoves[kingSquare] & getUnoccupied();
    // while (attacks != 0) {
    //     moves.push_back(Move(5, kingSquare, lsb(attacks), getCapturedPiece(lsb(attacks))));
    //     attacks &= attacks-1;
    // }
    // while (openMoves != 0) {
    //     moves.push_back(Move(5, kingSquare, lsb(openMoves)));
    //     openMoves &= openMoves-1;
    // }
    if ((turn? getK() : getk())) {
        uint8_t currSquare = lsb(turn? getK() : getk());
        uint64_t attacks = kingMoves[currSquare] & getNotTurn();
        uint64_t openMoves = kingMoves[currSquare] & getUnoccupied();
        while (attacks != 0) {
            uint8_t toSquare = lsb(attacks);
            int8_t capture = getCapturedPiece(toSquare);
            moves.push_back(Move(5, currSquare, toSquare, capture));
            attacks &= attacks-1;
        }
        while (openMoves != 0) {
            uint8_t toSquare = lsb(openMoves);
            moves.push_back(Move(5, currSquare, toSquare));
            openMoves &= openMoves-1;
        }
    }
    
    
    sort(moves.begin(), moves.end(), [] (Move& move1, Move& move2) {
        return move1.priority > move2.priority;
    });
}

void Board::captureGen (vector<Move> &moves) {
    
    //king captures, if king was captured, give 0 moves
    uint64_t cKing = turn? getK() : getk();
    if (cKing) {
        uint8_t currSquare = lsb(cKing);
        uint64_t attacks = kingMoves[currSquare] & getNotTurn();
        while (attacks != 0) {
            uint8_t toSquare = lsb(attacks);
            moves.push_back(Move(5, currSquare, toSquare, getCapturedPiece(toSquare)));
            attacks &= attacks-1;
        }
    } else {
        return;
    }

    //compute bitboards
    uint64_t cPawns = turn? getP() : getp(); 
    uint64_t cKnights = turn? getN() : getn(); uint64_t cBishops = turn? getB() : getb(); 
    uint64_t cRooks = turn? getR() : getr(); uint64_t cQueen = turn? getQ() : getq();

    //get attacks
    while (cPawns != 0) {
        uint64_t attacks = getPawnAttacks(turn, lsb(cPawns)) & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(
                    0, lsb(cPawns), lsb(attacks), 
                    getCapturedPiece(lsb(attacks)))
                );
            attacks &= attacks-1;
        }
        cPawns &= cPawns-1;
    }
    while (cKnights != 0) {
        uint64_t attacks = knightMoves[lsb(cKnights)] & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(
                    1, lsb(cKnights), lsb(attacks), 
                    getCapturedPiece(lsb(attacks)))
                );
            attacks &= attacks-1;
        }
        cKnights &= cKnights-1;
    }
    while (cBishops != 0) {
        uint64_t attacks = Bmagic(lsb(cBishops), getOccupied()) & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(2, lsb(cBishops), lsb(attacks), 
                getCapturedPiece(lsb(attacks)))
            );
            attacks &= attacks-1;
        }
        cBishops &= cBishops-1;
    }
    while (cRooks != 0) {
        uint64_t attacks = Rmagic(lsb(cRooks), getOccupied()) & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(3, lsb(cRooks), lsb(attacks), 
                getCapturedPiece(lsb(attacks)))
            );
            attacks &= attacks-1;
        }
        cRooks &= cRooks-1;
    }
    while (cQueen != 0) {
        uint8_t currSquare = lsb(cQueen);
        uint64_t attacks = (Bmagic(currSquare, getOccupied()) ^ Rmagic(currSquare, getOccupied())) & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(4, currSquare, lsb(attacks), 
                getCapturedPiece(lsb(attacks)))
            );
            attacks &= attacks-1;
        }
        cQueen &= cQueen-1;
    }
    
    sort(moves.begin(), moves.end(), [] (Move& move1, Move& move2) {
        return move1.priority > move2.priority;
    });
}



//castling conditions

bool Board::canRightCastle() {
    if (
        (getRightCastleChecks(turn) & ( getUnoccupied() ^ (turn? getK() : getk()) ))
        == getRightCastleChecks(turn)
    ) {
        uint64_t checks = getRightCastleChecks(turn);
        while (checks != 0) {
            if (isCheck(lsb(checks))) { return false; }
            checks &= checks-1;
        }
        return true;
    } else { return false; }
}
bool Board::canLeftCastle() {
    if (
        (getLeftCastleChecks(turn) & ( getUnoccupied() ^ (turn? getK() : getk()) ))
        == getLeftCastleChecks(turn)
    ) {
        uint64_t checks = getLeftCastleChecks(turn);
        while (checks != 0) {
            if (isCheck(lsb(checks))) { return false; }
            checks &= checks-1;
        }
        return true;
    } else { return false; }
}


bool Board::isCheck (uint8_t currSquare) {
    uint64_t combined = ~(getUnoccupied() ^ (turn? getK() : getk()));
    uint64_t diagonalAttackers = (turn? getq() ^ getb() : getQ() ^ getB()) & Bmagic(currSquare, combined);
    if (diagonalAttackers != 0) { return true;}
    uint64_t straightAttackers = (turn? getq() ^ getr() : getQ() ^ getR()) & Rmagic(currSquare, combined);
    if (straightAttackers != 0) { return true;}

    uint64_t knightAttackers = (turn? getn() : getN()) & knightMoves[currSquare];
    if (knightAttackers != 0) { return true; }
    uint64_t pawnAttackers = (turn? getp() : getP()) & getPawnAttacks(turn, currSquare);
    return pawnAttackers != 0;
}
bool Board::isCheck () {
    uint8_t currSquare = lsb(turn? getK() : getk());
    uint64_t combined = pieces[Piece::BLACK] ^ pieces[Piece::WHITE];

    uint64_t diagonalAttackers = (turn? getq() ^ getb() : getQ() ^ getB()) & Bmagic(currSquare, combined);
    if (diagonalAttackers != 0) { return true;}
    uint64_t straightAttackers = (turn? getq() ^ getr() : getQ() ^ getR()) & Rmagic(currSquare, combined);
    if (straightAttackers != 0) { return true;}

    uint64_t knightAttackers = (turn? getn() : getN()) & knightMoves[currSquare];
    if (knightAttackers != 0) { return true;}
    uint64_t pawnAttackers = (turn? getp() : getP()) & getPawnAttacks(turn, currSquare);
    return pawnAttackers != 0;
}