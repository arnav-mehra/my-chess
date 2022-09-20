#pragma once

inline void Board::toggleTurn() {
    turn = !turn;
}

inline void Board::incrementFromDist(U32 fromSq) {
    fromDist[fromSq]++;
}

inline void Board::decrementFromDist(U32 fromSq) {
    fromDist[fromSq]--;
}

inline void Board::move(Move &move) {
    performMove(move);
    incrementFromDist(move.getFromSq());
    toggleTurn();
}

inline void Board::unmove(Move &move) {
    toggleTurn();
    decrementFromDist(move.getFromSq());
    performMove(move);
}

inline void Board::performMove(Move &move) {
    // CAPTURES
    if (move.getCapture() != MoveCapture::NONE) {
        // remove captured piece (captured piece disappears)
        pieces[move.getCapture()] ^= move.getToVal();
        pieces[getNotColor()] ^= move.getToVal();
        // move capturing piece (piece moved normally)
        U64 moveXOR = move.getToVal() ^ move.getFromVal();
        pieces[move.getPiece()] ^= moveXOR;
        // color update (piece moved normally)
        pieces[getColor()] ^= moveXOR;
        // occupancy update (capturing piece disappears)
        pieces[Piece::OCCUP] ^= move.getFromVal();
        pieces[Piece::UNOCC] = ~pieces[Piece::OCCUP];
        return;
    }

    switch (move.getPiece()) {
        // PROMOTION
        case MoveType::KNIGHT_PROMO: 
        case MoveType::BISHOP_PROMO:
        case MoveType::QUEEN_PROMO:
        case MoveType::ROOK_PROMO: {
            // remove pawn (promo piece disappears)
            pieces[turn ? Piece::W_PAWN : Piece::B_PAWN] ^= move.getFromVal();
            // add promo piece (upgraded piece appears)
            Piece::Piece promo = (Piece::Piece) (move.getPiece() - (turn ? 15U : 9U));
            pieces[promo] ^= move.getToVal();
            // color update (piece moves normally)
            U64 moveXOR = move.getToVal() ^ move.getFromVal();
            pieces[getColor()] ^= moveXOR;
            pieces[Piece::OCCUP] ^= moveXOR;
            break;
        }
        // CASTLING
        case MoveType::WHITE_LEFT_CASTLE: {
            pieces[Piece::WHITE] ^= 2089670227099910144ULL;
            pieces[Piece::OCCUP] ^= 2089670227099910144ULL;
            pieces[Piece::W_KING] ^= 1441151880758558720ULL;
            pieces[Piece::W_ROOK] ^= 648518346341351424ULL;
            break;
        }
        case MoveType::WHITE_RIGHT_CASTLE: {
            pieces[Piece::WHITE] ^= 17293822569102704640ULL;
            pieces[Piece::OCCUP] ^= 17293822569102704640ULL;
            pieces[Piece::W_KING] ^= 5764607523034234880ULL;
            pieces[Piece::W_ROOK] ^= 11529215046068469760ULL;
            break;
        }
        case MoveType::BLACK_LEFT_CASTLE: {
            pieces[Piece::BLACK] ^= 29ULL;
            pieces[Piece::OCCUP] ^= 29ULL;
            pieces[Piece::B_KING] ^= 20ULL;
            pieces[Piece::B_ROOK] ^= 9ULL;
            break;
        }
        case MoveType::BLACK_RIGHT_CASTLE: {
            pieces[Piece::BLACK] ^= 240ULL;
            pieces[Piece::OCCUP] ^= 240ULL;
            pieces[Piece::B_KING] ^= 80ULL;
            pieces[Piece::B_ROOK] ^= 160ULL;
            break;
        }
        // QUIET: pieces just moves (normal movement)
        default: {
            U64 moveXOR = move.getToVal() ^ move.getFromVal();
            pieces[move.getPiece()] ^= moveXOR;
            pieces[getColor()] ^= moveXOR;
            pieces[Piece::OCCUP] ^= moveXOR;
            break;
        }
    }

    // update unoccupied board
    pieces[Piece::UNOCC] = ~pieces[Piece::OCCUP];
}