#include <algorithm>
#define LOG2(X) ((unsigned) (8*sizeof (unsigned long long) - __builtin_clzll((X)) - 1))

using namespace std;


//move generation

void Board::moveGen (bool turn, vector<Move> &moves) {
    if ((black & white) != 0) { cout << "shit"; }
    //occupancy
    uint64_t combined = black | white;
    uint64_t empty = ~combined;
    
    //king positions
    uint64_t cKing = turn? getK() : getk();
    if (cKing == 0) { return; }

    // castling
    if (turn && wKingMoves==0) {
        kings -= cKing; empty += cKing; white -= cKing;
        if (wLRookMoves==0 && canLeftCastle(true, empty)) {
            moves.push_back(Move(6, 60, 58, -1));
        }
        if (wRRookMoves==0 && canRightCastle(true, empty)) {
            moves.push_back(Move(7, 60, 62, -1));
        }
        kings += cKing; empty -= cKing; white += cKing;
    } else if (!turn && bKingMoves==0) {
        kings -= cKing; empty += cKing; black -= cKing; 
        if (bLRookMoves==0 && canLeftCastle(false, empty)) {
            moves.push_back(Move(6, 4, 2, -1));
        }
        if (bRRookMoves==0 && canRightCastle(false, empty)) {
            moves.push_back(Move(7, 4, 6, -1));
        }
        kings += cKing; empty -= cKing; black += cKing;
    }
      
    //piece positions
    uint64_t cPawns = turn? getP() : getp(), 
        cKnights = turn? getN() : getn(), cBishops = turn? getB() : getb(), 
        cRooks = turn? getR() : getr(), cQueen = turn? getQ() : getq();
    uint64_t up1 = (turn? cPawns >> 8 : cPawns << 8) & empty;
    uint64_t up2 = (turn? up1 >> 8 & OgPawnPos : up1 << 8 & OgPawnPos2) & empty;
      
    //pawn attacks/captures
    while (cPawns != 0) { 
        uint8_t currSquare = bitScanForward(cPawns);
        uint64_t attacks = turn? wPawnAttacks[currSquare] & black : bPawnAttacks[currSquare] & white;
        while (attacks != 0) {
          uint8_t toSquare = bitScanForward(attacks);
          int8_t capture = getPiece(toSquare);
          moves.push_back(Move(0, currSquare, toSquare, capture));
          attacks &= attacks-1;
        }
        cPawns &= cPawns-1;
    }

    //pawn forward 2
    while (up2 != 0) { 
        uint8_t toSquare = bitScanForward(up2);
        uint8_t fromSquare = turn? toSquare+16 : toSquare-16;
        moves.push_back(Move(0, fromSquare, toSquare, -1));
        up2 &= up2-1;
    }
      
    //knight moves
    while (cKnights != 0) {
        uint8_t currSquare = bitScanForward(cKnights);
        uint64_t attacks = knightMoves[currSquare] & getColor(!turn);
        uint64_t openMoves = knightMoves[currSquare] & empty;
        while (attacks != 0) {
          uint8_t toSquare = bitScanForward(attacks);
          int8_t capture = getPiece(toSquare);
          moves.push_back(Move(1, currSquare, toSquare, capture));
          attacks &= attacks-1;
        }
        while (openMoves != 0) {
          uint8_t toSquare = bitScanForward(openMoves);
          moves.push_back(Move(1, currSquare, toSquare, -1));
          openMoves &= openMoves-1;
        }
        cKnights &= cKnights-1;
    }
      
      //pawn moves 1
    while (up1 != 0) {
        uint8_t toSquare = bitScanForward(up1);
        uint8_t fromSquare = turn? toSquare+8 : toSquare-8;
        if (turn? toSquare < 8 : toSquare > 55) {
          moves.push_back(Move(-4, fromSquare, toSquare, -1));
          // moves.push_back({-3, fromSquare, toSquare, -1});
          // moves.push_back({-2, fromSquare, toSquare, -1});
          moves.push_back(Move(-1, fromSquare, toSquare, -1));
        } else {
          moves.push_back(Move(0, fromSquare, toSquare, -1));
        }
        up1 &= up1-1;
    }

      //bishop moves, unchecked
    while (cBishops != 0) {
        uint8_t currSquare = bitScanForward(cBishops);
        uint64_t bishopMoves = Bmagic(currSquare, combined);
        uint64_t attacks = bishopMoves & getColor(!turn);
        uint64_t openMoves = bishopMoves & empty;
        while (attacks != 0) {
          uint8_t toSquare = bitScanForward(attacks);
          int8_t capture = getPiece(toSquare);
          moves.push_back(Move(2, currSquare, toSquare, capture));
          attacks &= attacks-1;
        }
        while (openMoves != 0) {
          uint8_t toSquare = bitScanForward(openMoves);
          moves.push_back(Move(2, currSquare, toSquare, -1));
          openMoves &= openMoves-1;
        }
        cBishops &= cBishops-1;
    }

      //queen moves, unchecked
    while (cQueen != 0) {
        uint8_t currSquare = bitScanForward(cQueen);
        uint64_t queenMoves = Bmagic(currSquare, combined) ^ Rmagic(currSquare, combined);
        uint64_t attacks = queenMoves & getColor(!turn);
        uint64_t openMoves = queenMoves & empty;
        while (attacks != 0) {
          uint8_t toSquare = bitScanForward(attacks);
          int8_t capture = getPiece(toSquare);
          moves.push_back(Move(4, currSquare, toSquare, capture));
          attacks &= attacks-1;
        }
        while (openMoves != 0) {
          uint8_t toSquare = bitScanForward(openMoves);
          moves.push_back(Move(4, currSquare, toSquare, -1));
          openMoves &= openMoves-1;
        }
        cQueen &= cQueen-1;
    }

    //rook moves
    while (cRooks != 0) {
        uint8_t currSquare = bitScanForward(cRooks);
        uint64_t rookMoves = Rmagic(currSquare, combined);
        uint64_t attacks = rookMoves & getColor(!turn);
        uint64_t openMoves = rookMoves & empty;
        while (attacks != 0) {
          uint8_t toSquare = bitScanForward(attacks);
          int8_t capture = getPiece(toSquare);
          moves.push_back(Move(3, currSquare, toSquare, capture));
          attacks &= attacks-1;
        }
        while (openMoves != 0) {
          uint8_t toSquare = bitScanForward(openMoves);
          moves.push_back(Move(3, currSquare, toSquare, -1));
          openMoves &= openMoves-1;
        }
        cRooks &= cRooks-1;
    }

    //king moves, at end for ordering
    if (cKing) {
        uint8_t currSquare = LOG2(cKing);
        uint64_t attacks = kingMoves[currSquare] & getColor(!turn);
        uint64_t openMoves = kingMoves[currSquare] & empty;
        while (attacks != 0) {
            uint8_t toSquare = bitScanForward(attacks);
            int8_t capture = getPiece(toSquare);
            moves.push_back(Move(5, currSquare, toSquare, capture));
            attacks &= attacks-1;
        }
        while (openMoves != 0) {
            uint8_t toSquare = bitScanForward(openMoves);
            moves.push_back(Move(5, currSquare, toSquare, -1));
            openMoves &= openMoves-1;
        }
    }
      
    sort(moves.begin(), moves.end(), [] (Move& move1, Move& move2) {
        return move1.priority > move2.priority;
    });
}

void Board::captureGen (bool turn, vector<Move> &moves) {

    //occupancy
    uint64_t combined = black | white;
    
    //king captures, if king was captured, give 0 moves
    uint64_t cKing = turn? getK() : getk();
    if (cKing) {
        uint8_t currSquare = LOG2(cKing);
        uint64_t attacks = kingMoves[currSquare] & getColor(!turn);
        while (attacks != 0) {
            uint8_t toSquare = bitScanForward(attacks);
            int8_t capture = getPiece(toSquare);
            moves.push_back(Move(5, currSquare, toSquare, capture));
            attacks &= attacks-1;
        }
    } else {
        return;
    }

    //compute bitboards
    uint64_t cPawns = turn? getP() : getp(), 
    cKnights = turn? getN() : getn(), cBishops = turn? getB() : getb(), 
    cRooks = turn? getR() : getr(), cQueen = turn? getQ() : getq();

    //get attacks
    while (cPawns != 0) { 
        uint8_t currSquare = bitScanForward(cPawns);
        uint64_t attacks = turn? wPawnAttacks[currSquare] & black : bPawnAttacks[currSquare] & white;
        while (attacks != 0) {
            uint8_t toSquare = bitScanForward(attacks);
            int8_t capture = getPiece(toSquare);
            moves.push_back(Move(0, currSquare, toSquare, capture));
            attacks &= attacks-1;
        }
        cPawns &= cPawns-1;
    }
    while (cKnights != 0) {
        uint8_t currSquare = bitScanForward(cKnights);
        uint64_t attacks = knightMoves[currSquare] & getColor(!turn);
        while (attacks != 0) {
            uint8_t toSquare = bitScanForward(attacks);
            int8_t capture = getPiece(toSquare);
            moves.push_back(Move(1, currSquare, toSquare, capture));
            attacks &= attacks-1;
        }
        cKnights &= cKnights-1;
    }
    while (cBishops != 0) {
        uint8_t currSquare = bitScanForward(cBishops);
        uint64_t attacks = Bmagic(currSquare, combined) & getColor(!turn);
        while (attacks != 0) {
            uint8_t toSquare = bitScanForward(attacks);
            int8_t capture = getPiece(toSquare);
            moves.push_back(Move(2, currSquare, toSquare, capture));
            attacks &= attacks-1;
        }
        cBishops &= cBishops-1;
    }
    while (cRooks != 0) {
        uint8_t currSquare = bitScanForward(cRooks);
        uint64_t attacks = Rmagic(currSquare, combined) & getColor(!turn);
        while (attacks != 0) {
            uint8_t toSquare = bitScanForward(attacks);
            int8_t capture = getPiece(toSquare);
            moves.push_back(Move(3, currSquare, toSquare, capture));
            attacks &= attacks-1;
        }
        cRooks &= cRooks-1;
    }
    while (cQueen != 0) {
        uint8_t currSquare = bitScanForward(cQueen);
        uint64_t attacks = (Bmagic(currSquare, combined) ^ Rmagic(currSquare, combined)) & getColor(!turn);
        while (attacks != 0) {
            uint8_t toSquare = bitScanForward(attacks);
            int8_t capture = getPiece(toSquare);
            moves.push_back(Move(4, currSquare, toSquare, capture));
            attacks &= attacks-1;
        }
        cQueen &= cQueen-1;
    }
    
    sort(moves.begin(), moves.end(), [] (Move& move1, Move& move2) {
        return move1.priority > move2.priority;
    });
}



//castling conditions

bool Board::canRightCastle (bool turn, uint64_t &empty) {
    if ((rightCastleChecks[turn] & empty) == rightCastleChecks[turn]) {
        uint64_t checks = rightCastleChecks[turn];
        while (checks != 0) {
            uint8_t currSq = bitScanForward(checks);
            if (isCheck(turn, currSq)) { return false; }
            checks &= checks-1;
        }
        return true;
    } else { return false; }
}
bool Board::canLeftCastle (bool turn, uint64_t &empty) {
    if ((leftCastleChecks[turn] & empty) == leftCastleChecks[turn]) {
        uint64_t checks = leftCastleChecks[turn];
        while (checks != 0) {
            uint8_t currSq = bitScanForward(checks);
            if (isCheck(turn, currSq)) { return false; }
            checks &= checks-1;
        }
        return true;
    } else { return false; }
}

bool Board::isCheck (bool turn, uint8_t currSquare) {
    if (currSquare == 65) { currSquare = LOG2(getColor(turn) & kings); }
    uint64_t combined = black | white;

    uint64_t diagonalAttacks = getColor(!turn) & (queens | bishops) & Bmagic(currSquare, combined);
    if (diagonalAttacks != 0) {return true;}
    uint64_t straightAttacks = getColor(!turn) & (queens | rooks) & Rmagic(currSquare, combined);
    if (straightAttacks != 0) {return true;}

    uint64_t knightAttacks = getColor(!turn) & knights & knightMoves[currSquare];
    if (knightAttacks != 0) {return true;}
    uint64_t pawnAttacks = getColor(!turn) & pawns & (turn? wPawnAttacks[currSquare] : bPawnAttacks[currSquare]);
    return pawnAttacks != 0;
}
