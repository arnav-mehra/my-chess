#pragma once

inline void Board::captureGen(MoveList &moves) {
    if (getK() == 0ULL || getk() == 0ULL) return; // king precondition
    genKingCaptures(moves);
    genPawnCaptures(moves);
    genKnightCaptures(moves);
    genBishopCaptures(moves);
    genRookCaptures(moves);
    genQueenCaptures(moves);
    moves.sort(); // sort moves
}

inline void Board::genPawnCaptures(MoveList &moves) {
    MoveType::Piece piece = turn ? MoveType::W_PAWN : MoveType::B_PAWN;
    U64 cPawns = pieces[piece];
    while (cPawns) {
        U32 fromSq = lsb(cPawns);
        U64 attacks = getPawnAttacks(turn, fromSq) & getNotColor();
        while (attacks) {
            U32 toSq = lsb(attacks);
            auto capture = getCapturedPiece(toSq);
            moves.push(Move(piece, capture, fromSq, toSq));
            attacks &= attacks - 1;
        }
        cPawns &= cPawns - 1;
    }
}

inline void Board::genKnightCaptures(MoveList &moves) {
    MoveType::Piece piece = turn ? MoveType::W_KNIGHT : MoveType::B_KNIGHT;
    U64 cKnights = pieces[piece];
    while (cKnights) {
        U32 fromSq = lsb(cKnights);
        U64 attacks = knightMoves[fromSq] & getNotColor();
        while (attacks) {
            U32 toSq = lsb(attacks);
            auto capture = getCapturedPiece(toSq);
            moves.push(Move(piece, capture, fromSq, toSq));
            attacks &= attacks - 1;
        }
        cKnights &= cKnights - 1;
    }
}

inline void Board::genBishopCaptures(MoveList &moves) {
    MoveType::Piece piece = turn ? MoveType::W_BISHOP : MoveType::B_BISHOP;
    U64 cBishops = pieces[piece];
    while (cBishops) {
        U32 fromSq = lsb(cBishops);
        U64 attacks = Bmagic(fromSq, getOccupied()) & getNotColor();
        while (attacks) {
            U32 toSq = lsb(attacks);
            auto capture = getCapturedPiece(toSq);
            moves.push(Move(piece, capture, fromSq, toSq));
            attacks &= attacks - 1;
        }
        cBishops &= cBishops - 1;
    }
}

inline void Board::genRookCaptures(MoveList &moves) {
    MoveType::Piece piece = turn ? MoveType::W_ROOK : MoveType::B_ROOK;
    U64 cRooks = pieces[piece];
    while (cRooks) {
        U32 fromSq = lsb(cRooks);
        U64 attacks = Rmagic(lsb(cRooks), getOccupied()) & getNotColor();
        while (attacks) {
            U32 toSq = lsb(attacks);
            auto capture = getCapturedPiece(toSq);
            moves.push(Move(piece, capture, fromSq, toSq));
            attacks &= attacks - 1;
        }
        cRooks &= cRooks - 1;
    }
}

inline void Board::genQueenCaptures(MoveList &moves) {
    MoveType::Piece piece = turn ? MoveType::W_QUEEN : MoveType::B_QUEEN;
    U64 cQueens = pieces[piece];
    while (cQueens) {
        U32 fromSq = lsb(cQueens);
        U64 attacks = Qmagic(fromSq, getOccupied()) & getNotColor();
        while (attacks) {
            U32 toSq = lsb(attacks);
            auto capture = getCapturedPiece(toSq);
            moves.push(Move(piece, capture, fromSq, toSq));
            attacks &= attacks - 1;
        }
        cQueens &= cQueens - 1;
    }
}

inline void Board::genKingCaptures(MoveList &moves) {
    MoveType::Piece piece = turn ? MoveType::W_KING : MoveType::B_KING;
    U32 fromSq = lsb(pieces[piece]);
    U64 attacks = kingMoves[fromSq] & getNotColor();
    while (attacks != 0) {
        U32 toSq = lsb(attacks);
        auto capture = getCapturedPiece(toSq);
        moves.push(Move(piece, capture, fromSq, toSq));
        attacks &= attacks - 1;
    }
}