using namespace std;


void Board::move (Move &move) {
    
    uint8_t piece = move.getPiece() + (turn? 0 : 6);
    // eval += move.evalDiff;

    if (move.getCapture() != 12) { //piece is captured

        //remove captured piece
        pieces[move.getCapture()] &= ~move.getToVal();
        pieces[turn? Piece::BLACK : Piece::WHITE] &= ~move.getToVal();
        //move capturing piece
        pieces[piece] &= ~move.getFromVal();
        pieces[piece] |= move.getToVal();

    } else if (move.getPiece() >= 8) { // move is promotion
        
        pieces[turn? Piece::W_PAWN : Piece::B_PAWN] &= ~move.getFromVal();
        pieces[piece-7] |= move.getToVal();
        
    } else if (move.getPiece() >= 6) { // move is castle
        
        pieces[turn? Piece::W_KING : Piece::B_KING] &= ~move.getFromVal();
        pieces[turn? Piece::W_KING : Piece::B_KING] |= move.getToVal();
        switch(piece) {
            case 6:
                pieces[Piece::W_ROOK] += 504403158265495552ULL;
                wLRookSqMoves++; wKingSqMoves++; break;
            case 7:
                pieces[Piece::W_ROOK] -= 6917529027641081856ULL;
                wRRookSqMoves++; wKingSqMoves++; break;
            case 12:
                pieces[Piece::B_ROOK] += 7ULL;
                bLRookSqMoves++; bKingSqMoves++; break;
            case 13:
                pieces[Piece::B_ROOK] -= 96ULL;
                bRRookSqMoves++; bKingSqMoves++; break;
        }

    } else { // move is quiet
        
        pieces[piece] &= ~move.getFromVal();
        pieces[piece] |= move.getToVal();

    }


    // deals with castling rights
    switch (move.getFromSq()) {
        case 0: bLRookSqMoves++; break;
        case 7: bRRookSqMoves++; break;
        case 56: wLRookSqMoves++; break;
        case 53: wRRookSqMoves++; break;
        case 4: bKingSqMoves++; break;
        case 60: wKingSqMoves++; break;
    }

    // updates convenience boards
    if (turn) {
        pieces[Piece::WHITE] = pieces[Piece::W_PAWN] ^ pieces[Piece::W_KNIGHT] ^ pieces[Piece::W_BISHOP] ^ pieces[Piece::W_ROOK] ^ pieces[Piece::W_QUEEN] ^ pieces[Piece::W_KING];
    } else {
        pieces[Piece::BLACK] = pieces[Piece::B_PAWN] ^ pieces[Piece::B_KNIGHT] ^ pieces[Piece::B_BISHOP] ^ pieces[Piece::B_ROOK] ^ pieces[Piece::B_QUEEN] ^ pieces[Piece::B_KING];
    }
    pieces[Piece::OCCUP] = pieces[Piece::WHITE] ^ pieces[Piece::BLACK];
    pieces[Piece::UNOCC] = ~pieces[Piece::OCCUP];

    toggleMove();
    
}


void Board::unmove (Move &move) {
    toggleMove();

    uint8_t piece = move.getPiece() + (turn? 0 : 6);
    // eval -= move.evalDiff;

    if (move.getCapture() != 12) { //piece is captured

        //unmove capturing piece
        pieces[piece] |= move.getFromVal();
        pieces[piece] &= ~move.getToVal();
        //place back captured piece
        pieces[move.getCapture()] |= move.getToVal();
        pieces[turn? Piece::BLACK : Piece::WHITE] |= move.getToVal();
        
    } else if (move.getPiece() >= 8) { // move is promotion
        
        pieces[turn? Piece::W_PAWN : Piece::B_PAWN] |= move.getFromVal();
        pieces[piece-7] &= ~move.getToVal();

    } else if (move.getPiece() >= 6) { // move is castling
        
        pieces[turn? Piece::W_KING : Piece::B_KING] |= move.getFromVal();
        pieces[turn? Piece::W_KING : Piece::B_KING] &= ~move.getToVal();
        switch(piece) {
            case 6: // white left castle
                pieces[Piece::W_ROOK] -= 504403158265495552ULL;
                wLRookSqMoves--; wKingSqMoves--; break;
            case 7: // white right castle
                pieces[Piece::W_ROOK] += 6917529027641081856ULL;
                wRRookSqMoves--; wKingSqMoves--; break;
            case 12: // black left castle
                pieces[Piece::B_ROOK] -= 7ULL;
                bLRookSqMoves--; bKingSqMoves--; break;
            case 13: // black right castle
                pieces[Piece::B_ROOK] += 96ULL;
                bRRookSqMoves--; bKingSqMoves--; break;
        }

    } else { // move is quiet, just move normally

        pieces[piece] |= move.getFromVal();
        pieces[piece] &= ~move.getToVal();
        
    }


    // deals with castling rights
    switch (move.getFromSq()) {
        case 0: bLRookSqMoves--; break;
        case 7: bRRookSqMoves--; break;
        case 56: wLRookSqMoves--; break;
        case 53: wRRookSqMoves--; break;
        case 4: bKingSqMoves--; break;
        case 60: wKingSqMoves--; break;
    }

    // updates convenience boards
    if (turn) {
        pieces[Piece::WHITE] = pieces[Piece::W_PAWN] ^ pieces[Piece::W_KNIGHT] ^ pieces[Piece::W_BISHOP] ^ pieces[Piece::W_ROOK] ^ pieces[Piece::W_QUEEN] ^ pieces[Piece::W_KING];
    } else {
        pieces[Piece::BLACK] = pieces[Piece::B_PAWN] ^ pieces[Piece::B_KNIGHT] ^ pieces[Piece::B_BISHOP] ^ pieces[Piece::B_ROOK] ^ pieces[Piece::B_QUEEN] ^ pieces[Piece::B_KING];
    }
    pieces[Piece::OCCUP] = pieces[Piece::WHITE] ^ pieces[Piece::BLACK];
    pieces[Piece::UNOCC] = ~pieces[Piece::OCCUP];

}