using namespace std;


//move generation

void Board::moveGen (vector<Move> &moves) {
    
    //occupancy
    uint64_t combined = pieces[Piece::BLACK] ^ pieces[Piece::WHITE];
    uint64_t empty = ~combined;
    
    //king positions
    uint64_t cKing = turn? getK() : getk();
    if (cKing == 0) { return; }

    // castling
    if (turn && wKingSqMoves==0) {
        if (wLRookSqMoves==0 && canLeftCastle(empty + cKing)) {
            moves.push_back(Move(6, 60, 58));
        }
        if (wRRookSqMoves==0 && canRightCastle(empty + cKing)) {
            moves.push_back(Move(7, 60, 62));
        }
    } else if (!turn && bKingSqMoves==0) {
        if (bLRookSqMoves==0 && canLeftCastle(empty + cKing)) {
            moves.push_back(Move(6, 4, 2));
        }
        if (bRRookSqMoves==0 && canRightCastle(empty + cKing)) {
            moves.push_back(Move(7, 4, 6));
        }
    }
      
    //piece positions
    uint64_t cPawns = turn? getP() : getp();
    uint64_t cKnights = turn? getN() : getn(); uint64_t cBishops = turn? getB() : getb();
    uint64_t cRooks = turn? getR() : getr(); uint64_t cQueen = turn? getQ() : getq();
    uint64_t up1 = (turn? cPawns >> 8 : cPawns << 8) & empty;
    uint64_t up2 = (turn? up1 >> 8 : up1 << 8) & empty & getPawnDouble(turn);
    
    //pawn attacks/captures
    while (cPawns != 0) {
        uint64_t attacks = getPawnAttacks(turn, bitScanForward(cPawns)) & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(
                    0, bitScanForward(cPawns), bitScanForward(attacks), 
                    getCapturedPiece(bitScanForward(attacks))
                )
            );
          attacks &= attacks-1;
        }
        cPawns &= cPawns-1;
    }

    //pawn forward 2
    while (up2 != 0) {
        moves.push_back(Move(0, bitScanForward(up2) + (turn? 16 : -16), bitScanForward(up2)));
        up2 &= up2-1;
    }

    //knight moves
    while (cKnights != 0) {
        uint8_t currSquare = bitScanForward(cKnights);
        uint64_t attacks = knightMoves[currSquare] & getNotTurn();
        uint64_t openMoves = knightMoves[currSquare] & empty;
        while (attacks != 0) {
          moves.push_back(Move(1, currSquare, bitScanForward(attacks), getCapturedPiece(bitScanForward(attacks))));
          attacks &= attacks-1;
        }
        while (openMoves != 0) {
          moves.push_back(Move(1, currSquare, bitScanForward(openMoves)));
          openMoves &= openMoves-1;
        }
        cKnights &= cKnights-1;
    }
 
    //pawn moves 1
    while (up1 != 0) {
        uint8_t toSquare = bitScanForward(up1);
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

    //bishop moves, unchecked
    while (cBishops != 0) {
        uint64_t bishopMoves = Bmagic(bitScanForward(cBishops), combined);
        uint64_t attacks = bishopMoves & getNotTurn();
        uint64_t openMoves = bishopMoves & empty;
        while (attacks != 0) {
          moves.push_back(Move(2, bitScanForward(cBishops), bitScanForward(attacks), getCapturedPiece(bitScanForward(attacks))));
          attacks &= attacks-1;
        }
        while (openMoves != 0) {
          moves.push_back(Move(2, bitScanForward(cBishops), bitScanForward(openMoves)));
          openMoves &= openMoves-1;
        }
        cBishops &= cBishops-1;
    }

    //queen moves, unchecked
    while (cQueen != 0) {
        uint8_t currSquare = bitScanForward(cQueen);
        uint64_t queenMoves = Bmagic(currSquare, combined) ^ Rmagic(currSquare, combined);
        uint64_t attacks = queenMoves & getNotTurn();
        uint64_t openMoves = queenMoves & empty;
        while (attacks != 0) {
          moves.push_back(Move(4, currSquare, bitScanForward(attacks), getCapturedPiece(bitScanForward(attacks))));
          attacks &= attacks-1;
        }
        while (openMoves != 0) {
          moves.push_back(Move(4, currSquare, bitScanForward(openMoves)));
          openMoves &= openMoves-1;
        }
        cQueen &= cQueen-1;
    }

    //rook moves
    while (cRooks != 0) {
        uint64_t rookMoves = Rmagic(bitScanForward(cRooks), combined);
        uint64_t attacks = rookMoves & getNotTurn();
        uint64_t openMoves = rookMoves & empty;
        while (attacks != 0) {
          moves.push_back(Move(3, bitScanForward(cRooks), bitScanForward(attacks), getCapturedPiece(bitScanForward(attacks))));
          attacks &= attacks-1;
        }
        while (openMoves != 0) {
          moves.push_back(Move(3, bitScanForward(cRooks), bitScanForward(openMoves)));
          openMoves &= openMoves-1;
        }
        cRooks &= cRooks-1;
    }

    //king moves
    if (cKing) {
        uint8_t currSquare = bitScanForward(cKing);
        uint64_t attacks = kingMoves[currSquare] & getNotTurn();
        uint64_t openMoves = kingMoves[currSquare] & empty;
        while (attacks != 0) {
            moves.push_back(Move(5, currSquare, bitScanForward(attacks), getCapturedPiece(bitScanForward(attacks))));
            attacks &= attacks-1;
        }
        while (openMoves != 0) {
            moves.push_back(Move(5, currSquare, bitScanForward(openMoves)));
            openMoves &= openMoves-1;
        }
    }
      
    sort(moves.begin(), moves.end(), [] (Move& move1, Move& move2) {
        return move1.priority > move2.priority;
    });
}

void Board::captureGen (vector<Move> &moves) {

    //occupancy
    uint64_t combined = pieces[Piece::BLACK] ^ pieces[Piece::WHITE];
    
    //king captures, if king was captured, give 0 moves
    uint64_t cKing = turn? getK() : getk();
    if (cKing) {
        uint8_t currSquare = bitScanForward(cKing);
        uint64_t attacks = kingMoves[currSquare] & getNotTurn();
        while (attacks != 0) {
            uint8_t toSquare = bitScanForward(attacks);
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
        uint64_t attacks = getPawnAttacks(turn, bitScanForward(cPawns)) & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(
                    0, bitScanForward(cPawns), bitScanForward(attacks), 
                    getCapturedPiece(bitScanForward(attacks)))
                );
            attacks &= attacks-1;
        }
        cPawns &= cPawns-1;
    }
    while (cKnights != 0) {
        uint64_t attacks = knightMoves[bitScanForward(cKnights)] & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(
                    1, bitScanForward(cKnights), bitScanForward(attacks), 
                    getCapturedPiece(bitScanForward(attacks)))
                );
            attacks &= attacks-1;
        }
        cKnights &= cKnights-1;
    }
    while (cBishops != 0) {
        uint64_t attacks = Bmagic(bitScanForward(cBishops), combined) & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(2, bitScanForward(cBishops), bitScanForward(attacks), 
                getCapturedPiece(bitScanForward(attacks)))
            );
            attacks &= attacks-1;
        }
        cBishops &= cBishops-1;
    }
    while (cRooks != 0) {
        uint64_t attacks = Rmagic(bitScanForward(cRooks), combined) & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(3, bitScanForward(cRooks), bitScanForward(attacks), 
                getCapturedPiece(bitScanForward(attacks)))
            );
            attacks &= attacks-1;
        }
        cRooks &= cRooks-1;
    }
    while (cQueen != 0) {
        uint8_t currSquare = bitScanForward(cQueen);
        uint64_t attacks = (Bmagic(currSquare, combined) ^ Rmagic(currSquare, combined)) & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(4, currSquare, bitScanForward(attacks), 
                getCapturedPiece(bitScanForward(attacks)))
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

bool Board::canRightCastle (uint64_t empty) {
    if ((getRightCastleChecks(turn) & empty) == getRightCastleChecks(turn)) {
        uint64_t checks = getRightCastleChecks(turn);
        while (checks != 0) {
            uint8_t currSq = bitScanForward(checks);
            if (isCheck(currSq, ~empty)) { return false; }
            checks &= checks-1;
        }
        return true;
    } else { return false; }
}
bool Board::canLeftCastle (uint64_t empty) {
    if ((getLeftCastleChecks(turn) & empty) == getLeftCastleChecks(turn)) {
        uint64_t checks = getLeftCastleChecks(turn);
        while (checks != 0) {
            uint8_t currSq = bitScanForward(checks);
            if (isCheck(currSq, ~empty)) { return false; }
            checks &= checks-1;
        }
        return true;
    } else { return false; }
}


bool Board::isCheck (uint8_t currSquare, uint64_t combined) {
    if (currSquare == 65) { currSquare = bitScanForward(turn? getK() : getk()); }

    uint64_t diagonalAttackers = (turn? getq() ^ getb() : getQ() ^ getB()) & Bmagic(currSquare, combined);
    if (diagonalAttackers != 0) { return true;}
    uint64_t straightAttackers = (turn? getq() ^ getr() : getQ() ^ getR()) & Rmagic(currSquare, combined);
    if (straightAttackers != 0) { return true;}

    uint64_t knightAttackers = (turn? getn() : getN()) & knightMoves[currSquare];
    if (knightAttackers != 0) { return true;}
    uint64_t pawnAttackers = (turn? getp() : getP()) & getPawnAttacks(turn, currSquare);
    return pawnAttackers != 0;
}
bool Board::isCheck (uint8_t currSquare) {
    if (currSquare == 65) { currSquare = bitScanForward(turn? getK() : getk()); }
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