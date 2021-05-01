using namespace std;


pair<array<uint64_t,7>, int> Board::getBoardKey() {
    array<uint64_t,7> zKey = {
        pieces[12], pieces[13],
        getP() ^ getp() ^ //pawns only take up bits 8-55, 0-4 can be used for castling rights
        (wKingSqMoves==0 && wRRookSqMoves==0) ^ ((wKingSqMoves==0 && wLRookSqMoves==0) << 1) ^ 
        ((bKingSqMoves==0 && bRRookSqMoves==0) << 2) ^ ((bKingSqMoves==0 && bLRookSqMoves==0) << 3),
        getN() ^ getn(), getB() ^ getb(), 
        getR() ^ getr(), getQ() ^ getq()
    };
    
    return pair<array<uint64_t,7>, int>(
        zKey, 
        turn?
            whiteZKeyTable.count(zKey)? whiteZKeyTable.find(zKey) -> second : 11000
        :
            blackZKeyTable.count(zKey)? blackZKeyTable.find(zKey) -> second : 11000
    );
}

inline void addBoardEvalPair(pair<array<uint64_t,7>, int> boardEvalPair, bool turn) {
    if (turn) {
        whiteZKeyTable.insert(boardEvalPair);
    } else {
        blackZKeyTable.insert(boardEvalPair);
    }
}