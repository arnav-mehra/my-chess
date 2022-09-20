#pragma once

inline void Board::moveGen(MoveList &moves) {
    if (getK() == 0ULL || getk() == 0ULL) return; // king precondition
    // gen moves
    genCastlingMoves(moves);
    genPawnMoves(moves);
    genKnightMoves(moves);
    genBishopMoves(moves);
    genQueenMoves(moves);
    genRookMoves(moves);
    genKingMoves(moves);    
    // moves.sort(); // sort moves
}

inline void Board::genPawnMoves(MoveList &moves) {
    MoveType::Piece piece = turn ? MoveType::W_PAWN : MoveType::B_PAWN;
    U64 cPawns = pieces[piece];
    U64 up1 = (turn ? cPawns >> 8 : cPawns << 8) & getUnoccupied();
    U64 up2 = (turn ? up1 >> 8 : up1 << 8) & getUnoccupied() & getPawnDouble(turn);

    // captures
    while (cPawns) {
        U32 fromSq = lsb(cPawns);
        U64 attacks = getPawnAttacks(turn, fromSq) & getNotColor();
        while (attacks) {
            U32 toSq = lsb(attacks);
            auto capture = getCapturedPiece(toSq);
            moves.push(Move(piece, capture, fromSq, toSq));
            attacks &= attacks - 1ULL;
        }
        cPawns &= cPawns - 1ULL;
    }

    // forward pawn moves
    if (turn) {
        while (up1) {
            U32 toSq = lsb(up1);
            U32 fromSq = toSq + 8U;
            if (toSq < 8U) {
                moves.push(Move(MoveType::KNIGHT_PROMO, fromSq, toSq));
                // moves.push(Move(MoveType::BISHOP_PROMO, fromSq, toSq));
                // moves.push(Move(MoveType::ROOK_PROMO, fromSq, toSq));
                moves.push(Move(MoveType::QUEEN_PROMO, fromSq, toSq));
            } else {
                moves.push(Move(piece, fromSq, toSq));
            }
            up1 &= up1 - 1ULL;
        }
        while (up2) {
            U32 toSq = lsb(up2);
            U32 fromSq = toSq + 16U;
            moves.push(Move(piece, fromSq, toSq));
            up2 &= up2 - 1ULL;
        }
    } else {
        while (up1) {
            U32 toSq = lsb(up1);
            U32 fromSq = toSq - 8U;
            if (toSq > 55U) {
                moves.push(Move(MoveType::KNIGHT_PROMO, fromSq, toSq));
                // moves.push(Move(MoveType::BISHOP_PROMO, fromSq, toSq));
                // moves.push(Move(MoveType::ROOK_PROMO, fromSq, toSq));
                moves.push(Move(MoveType::QUEEN_PROMO, fromSq, toSq));
            } else {
                moves.push(Move(piece, fromSq, toSq));
            }
            up1 &= up1 - 1ULL;
        }
        while (up2) {
            U32 toSq = lsb(up2);
            U32 fromSq = toSq - 16U;
            moves.push(Move(piece, fromSq, toSq));
            up2 &= up2 - 1ULL;
        }
    }
}

inline void Board::genKnightMoves(MoveList &moves) {
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
        U64 openMoves = knightMoves[fromSq] & getUnoccupied();
        while (openMoves) {
            U32 toSq = lsb(openMoves);
            moves.push(Move(piece, fromSq, toSq));
            openMoves &= openMoves - 1;
        }
        cKnights &= cKnights - 1;
    }
}

inline void Board::genBishopMoves(MoveList &moves) {
    MoveType::Piece piece = turn ? MoveType::W_BISHOP : MoveType::B_BISHOP;
    U64 cBishops = pieces[piece];
    while (cBishops) {
        U32 fromSq = lsb(cBishops);
        U64 bishopMoves = Bmagic(fromSq, getOccupied());
        U64 attacks = bishopMoves & getNotColor();
        while (attacks) {
            U32 toSq = lsb(attacks);
            auto capture = getCapturedPiece(toSq);
            moves.push(Move(piece, capture, fromSq, toSq));
            attacks &= attacks - 1;
        }
        U64 openMoves = bishopMoves & getUnoccupied();
        while (openMoves) {
            U32 toSq = lsb(openMoves);
            moves.push(Move(piece, fromSq, toSq));
            openMoves &= openMoves - 1;
        }
        cBishops &= cBishops - 1;
    }
}

inline void Board::genRookMoves(MoveList &moves) {
    MoveType::Piece piece = turn ? MoveType::W_ROOK : MoveType::B_ROOK;
    U64 cRooks = pieces[piece];
    while (cRooks) {
        U32 fromSq = lsb(cRooks);
        U64 rookMoves = Rmagic(fromSq, getOccupied());
        U64 attacks = rookMoves & getNotColor();
        while (attacks) {
            U32 toSq = lsb(attacks);
            auto capture = getCapturedPiece(toSq);
            moves.push(Move(piece, capture, fromSq, toSq));
            attacks &= attacks - 1;
        }
        U64 openMoves = rookMoves & getUnoccupied();
        while (openMoves) {
            U32 toSq = lsb(openMoves);
            moves.push(Move(piece, fromSq, toSq));
            openMoves &= openMoves - 1;
        }
        cRooks &= cRooks - 1;
    }
}

inline void Board::genQueenMoves(MoveList &moves) {
    MoveType::Piece piece = turn ? MoveType::W_QUEEN : MoveType::B_QUEEN;
    U64 cQueens = pieces[piece];
    while (cQueens) {
        U32 currSquare = lsb(cQueens);
        U64 queenMoves = Qmagic(currSquare, getOccupied());
        U64 attacks = queenMoves & getNotColor();
        while (attacks) {
            U32 toSq = lsb(attacks);
            auto capture = getCapturedPiece(toSq);
            moves.push(Move(piece, capture, currSquare, toSq));
            attacks &= attacks - 1;
        }
        U64 openMoves = queenMoves & getUnoccupied();
        while (openMoves) {
            U32 toSq = lsb(openMoves);
            moves.push(Move(piece, currSquare, toSq));
            openMoves &= openMoves - 1;
        }
        cQueens &= cQueens - 1;
    }
}

inline void Board::genKingMoves(MoveList &moves) {
    MoveType::Piece piece = turn ? MoveType::W_KING : MoveType::B_KING;
    U32 fromSq = lsb(pieces[piece]);
    U64 attacks = kingMoves[fromSq] & getNotColor();
    while (attacks) {
        U32 toSq = lsb(attacks);
        auto capture = getCapturedPiece(toSq);
        moves.push(Move(piece, capture, fromSq, toSq));
        attacks &= attacks - 1;
    }
    U64 openMoves = kingMoves[fromSq] & getUnoccupied();
    while (openMoves) {
        U32 toSq = lsb(openMoves);
        moves.push(Move(piece, fromSq, toSq));
        openMoves &= openMoves - 1;
    }
}