using namespace std;


inline array<uint64_t,7> Board::getBoardKey() {
    // array<uint64_t,7> boardKey;
    if (turn) {
        return {
            pieces[12], pieces[13],
            getP() ^ getp() ^ //pawns only take up bits 8-55, 0-4 can be used for castling rights
            (wKingSqMoves==0 && wRRookSqMoves==0) ^ ((wKingSqMoves==0 && wLRookSqMoves==0) << 1) ^ 
            ((bKingSqMoves==0 && bRRookSqMoves==0) << 2) ^ ((bKingSqMoves==0 && bLRookSqMoves==0) << 3),
            getN() ^ getn(), getB() ^ getb(), 
            getR() ^ getr(), getQ() ^ getq()
        };
    } else {
        return {
            pieces[13], pieces[12],
            getP() ^ getp() ^ //pawns only take up bits 8-55, 0-4 can be used for castling rights
            (wKingSqMoves==0 && wRRookSqMoves==0) ^ ((wKingSqMoves==0 && wLRookSqMoves==0) << 1) ^ 
            ((bKingSqMoves==0 && bRRookSqMoves==0) << 2) ^ ((bKingSqMoves==0 && bLRookSqMoves==0) << 3),
            getN() ^ getn(), getB() ^ getb(), 
            getR() ^ getr(), getQ() ^ getq()
        };
    }
}


class BoardEvalEntry {
    array<uint64_t,7> position;
    int depth;
    int eval;
};