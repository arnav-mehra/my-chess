#pragma once

inline void Board::genCastlingMoves(MoveList &moves) {
    if (turn && fromDist[60] == 0U) {
        if (fromDist[56] == 0U && canWhiteLeftCastle())
            moves.push(Move(MoveType::WHITE_LEFT_CASTLE, 60U, 58U));
        if (fromDist[63] == 0U && canWhiteRightCastle())
            moves.push(Move(MoveType::WHITE_RIGHT_CASTLE, 60U, 62U));
    } else if (!turn && fromDist[4] == 0U) {
        if (fromDist[0] == 0U && canBlackLeftCastle())
            moves.push(Move(MoveType::BLACK_LEFT_CASTLE, 4U, 2U));
        if (fromDist[7] == 0U && canBlackRightCastle())
            moves.push(Move(MoveType::BLACK_RIGHT_CASTLE, 4U, 6U));
    }
}

inline bool Board::canWhiteLeftCastle() {
    U64 checks = 2161727821137838080ULL;
    U64 empty = getUnoccupied() ^ getK();
    if ((checks & empty) != checks) return false;
    while (checks) {
        if (isWhiteChecked(lsb(checks))) return false;
        checks &= checks - 1;
    }
    return true;
}

inline bool Board::canBlackLeftCastle() {
    U64 checks = 30ULL;
    U64 empty = getUnoccupied() ^ getk();
    if ((checks & empty) != checks) return false;
    while (checks) {
        if (isBlackChecked(lsb(checks))) return false;
        checks &= checks - 1;
    }
    return true;
}

inline bool Board::canWhiteRightCastle() {
    U64 checks = 8070450532247928832ULL;
    U64 empty = getUnoccupied() ^ getK();
    if ((checks & empty) != checks) return false;
    while (checks) {
        if (isWhiteChecked(lsb(checks))) return false;
        checks &= checks - 1;
    }
    return true;
}

inline bool Board::canBlackRightCastle() {
    U64 checks = 112ULL;
    U64 empty = getUnoccupied() ^ getk();
    if ((checks & empty) != checks) return false;
    while (checks) {
        if (isBlackChecked(lsb(checks))) return false;
        checks &= checks - 1;
    }
    return true;
}

inline bool Board::isWhiteChecked(U32 currSquare) {
    U64 combined = ~(getUnoccupied() ^ getK());
    
    U64 diagonalAttackers = (getq() ^ getb()) & Bmagic(currSquare, combined);
    if (diagonalAttackers) return true;
    
    U64 straightAttackers = (getq() ^ getr()) & Rmagic(currSquare, combined);
    if (straightAttackers) return true;

    U64 knightAttackers = getn() & knightMoves[currSquare];
    if (knightAttackers) return true;

    U64 pawnAttackers = getp() & wPawnAttacks[currSquare]; // not a mistake
    return pawnAttackers;
}

inline bool Board::isBlackChecked(U32 currSquare) {
    U64 combined = ~(getUnoccupied() ^ getk());
    
    U64 diagonalAttackers = (getQ() ^ getB()) & Bmagic(currSquare, combined);
    if (diagonalAttackers) return true;
    
    U64 straightAttackers = (getQ() ^ getR()) & Rmagic(currSquare, combined);
    if (straightAttackers) return true;

    U64 knightAttackers = getN() & knightMoves[currSquare];
    if (knightAttackers) return true;

    U64 pawnAttackers = getP() & getPawnAttacks(turn, currSquare);
    return pawnAttackers;
}

inline bool Board::isCheck() {
    if (turn) {
        U32 kingSq = lsb(getK());
        return isWhiteChecked(kingSq);
    } else {
        U32 kingSq = lsb(getk());
        return isBlackChecked(kingSq);
    }
}