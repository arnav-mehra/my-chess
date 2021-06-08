#include "gen-captures.h"
#include "gen-moves.h"

using namespace std;


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