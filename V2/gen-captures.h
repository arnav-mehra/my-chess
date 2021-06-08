using namespace std;


void Board::captureGen (vector<Move> &moves) {

    //king precondition
    if ((turn? getK() : getk()) != 0) {
        genKingCaptures (moves, lsb(turn? getK() : getk()));
    } else { return; }

    //get captures
    genPawnCaptures (moves, turn? getP() : getp());
    genKnightCaptures (moves, turn? getN() : getn());
    genBishopCaptures (moves, turn? getB() : getb());
    genRookCaptures (moves, turn? getR() : getr());
    genQueenCaptures (moves, turn? getQ() : getq());

    //sort moves
    sort(moves.begin(), moves.end(), [] (Move& move1, Move& move2) {
        return move1.priority > move2.priority;
    });

}


void Board::genPawnCaptures (vector<Move> &moves, uint64_t cPawns) {
    while (cPawns != 0) {
        uint64_t attacks = getPawnAttacks(turn, lsb(cPawns)) & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(
                    0, lsb(cPawns), lsb(attacks), getCapturedPiece(lsb(attacks)))
                );
            attacks &= attacks-1;
        }
        cPawns &= cPawns-1;
    }
}
void Board::genKnightCaptures (vector<Move> &moves, uint64_t cKnights) {
    while (cKnights != 0) {
        uint64_t attacks = knightMoves[lsb(cKnights)] & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(
                    1, lsb(cKnights), lsb(attacks), getCapturedPiece(lsb(attacks)))
                );
            attacks &= attacks-1;
        }
        cKnights &= cKnights-1;
    }
}
void Board::genBishopCaptures (vector<Move> &moves, uint64_t cBishops) {
    while (cBishops != 0) {
        uint64_t attacks = Bmagic(lsb(cBishops), getOccupied()) & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(2, lsb(cBishops), lsb(attacks), getCapturedPiece(lsb(attacks)))
            );
            attacks &= attacks-1;
        }
        cBishops &= cBishops-1;
    }
}
void Board::genRookCaptures (vector<Move> &moves, uint64_t cRooks) {
    while (cRooks != 0) {
        uint64_t attacks = Rmagic(lsb(cRooks), getOccupied()) & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(3, lsb(cRooks), lsb(attacks), getCapturedPiece(lsb(attacks)))
            );
            attacks &= attacks-1;
        }
        cRooks &= cRooks-1;
    }
}
void Board::genQueenCaptures (vector<Move> &moves, uint64_t cQueens) {
    while (cQueens != 0) {
        uint8_t currSquare = lsb(cQueens);
        uint64_t attacks = (Bmagic(currSquare, getOccupied()) ^ Rmagic(currSquare, getOccupied())) & getNotTurn();
        while (attacks != 0) {
            moves.push_back(
                Move(4, currSquare, lsb(attacks), getCapturedPiece(lsb(attacks)))
            );
            attacks &= attacks-1;
        }
        cQueens &= cQueens-1;
    }
}
void Board::genKingCaptures (vector<Move> &moves, uint8_t currSquare) {
    uint64_t attacks = kingMoves[currSquare] & getNotTurn();
    while (attacks != 0) {
        moves.push_back(
            Move(5, currSquare, lsb(attacks), getCapturedPiece(lsb(attacks)))
        );
        attacks &= attacks-1;
    }
}
