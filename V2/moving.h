using namespace std;


void Board::move (Move &move) {

    uint8_t piece = move.getPiece() + (turn? 0 : 6);

    if (move.getCapture() != 12) { //piece is captured

        //remove captured piece
        pieces[move.getCapture()] &= ~move.getToVal();
        pieces[turn? Piece::BLACK : Piece::WHITE] &= ~move.getToVal();
        //move capturing piece
        pieces[piece] &= ~move.getFromVal();
        pieces[piece] |= move.getToVal();

    } else { //no capture

    if (move.getPiece() >= 8) { // move is promotion
    
        pieces[turn? Piece::W_PAWN : Piece::B_PAWN] &= ~move.getFromVal();
        pieces[piece-7] |= move.getToVal();

    } else { // move is quiet or castle

        // move piece normally
        pieces[piece] &= ~move.getFromVal();
        pieces[piece] |= move.getToVal();

        //castle switch check
        switch (move.getFromSq()) {
        case 0: bLRookSqMoves++; break;
        case 7: bRRookSqMoves++; break;
        case 56: wLRookSqMoves++; break;
        case 53: wRRookSqMoves++; break;
        case 4: {
            if (piece == 12) {
                pieces[Piece::B_ROOK] += 7ULL;
                bLRookSqMoves++;
            } else if (piece == 13) {
                pieces[Piece::B_ROOK] -= 96ULL;
                bRRookSqMoves++;
            }
            bKingSqMoves++; break;
        }
        case 60: {
            if (piece == 6) {
                pieces[Piece::W_ROOK] += 504403158265495552ULL;
                wLRookSqMoves++;
            } else if (piece == 7) { //left castle
                pieces[Piece::W_ROOK] -= 6917529027641081856ULL;
                wRRookSqMoves++; 
            }
            wKingSqMoves++; break;
        }
        }

    }
    
    }

    if (turn) {
        pieces[Piece::WHITE] = pieces[Piece::W_PAWN] ^ pieces[Piece::W_KNIGHT] ^ pieces[Piece::W_BISHOP] ^ pieces[Piece::W_ROOK] ^ pieces[Piece::W_QUEEN] ^ pieces[Piece::W_KING];
    } else {
        pieces[Piece::BLACK] = pieces[Piece::B_PAWN] ^ pieces[Piece::B_KNIGHT] ^ pieces[Piece::B_BISHOP] ^ pieces[Piece::B_ROOK] ^ pieces[Piece::B_QUEEN] ^ pieces[Piece::B_KING];
    }
    
    toggleMove();
}


void Board::unmove (Move &move) {
    toggleMove();

    uint8_t piece = move.getPiece() + (turn? 0 : 6);

    //move piece back
    if (move.getCapture() != 12) { //piece is captured

        //unmove capturing piece
        pieces[piece] |= move.getFromVal();
        pieces[piece] &= ~move.getToVal();
        //place back captured piece
        pieces[move.getCapture()] |= move.getToVal();
        pieces[turn? Piece::BLACK : Piece::WHITE] |= move.getToVal();

    } else {

    if (move.getPiece() >= 8) { // move is promotion
    
        pieces[turn? Piece::W_PAWN : Piece::B_PAWN] |= move.getFromVal();
        pieces[piece-7] &= ~move.getToVal();

    } else { // move is quiet or castle

        // move piece normally
        pieces[piece] |= move.getFromVal();
        pieces[piece] &= ~move.getToVal();

        //castle switch check
        switch (move.getFromSq()) {
        case 0: bLRookSqMoves--; break;
        case 7: bRRookSqMoves--; break;
        case 56: wLRookSqMoves--; break;
        case 53: wRRookSqMoves--; break;
        case 4: {
            if (piece == 12) { //left castle
                pieces[Piece::B_ROOK] -= 7ULL;
                bLRookSqMoves--;
            } else if (piece == 13) { // right castle
                pieces[Piece::B_ROOK] += 96ULL;
                bRRookSqMoves--;
            }
            bKingSqMoves--; break;
        }
        case 60: {
            if (piece == 6) { //left castle
                pieces[Piece::W_ROOK] -= 504403158265495552ULL;
                wLRookSqMoves--;
            } else if (piece == 7) { // right castle
                pieces[Piece::W_ROOK] += 6917529027641081856ULL;
                wRRookSqMoves--;
            }
            wKingSqMoves--; break;
        }
        }
        
    }

    }
    
    if (turn) {
        pieces[Piece::WHITE] = pieces[Piece::W_PAWN] ^ pieces[Piece::W_KNIGHT] ^ pieces[Piece::W_BISHOP] ^ pieces[Piece::W_ROOK] ^ pieces[Piece::W_QUEEN] ^ pieces[Piece::W_KING];
    } else {
        pieces[Piece::BLACK] = pieces[Piece::B_PAWN] ^ pieces[Piece::B_KNIGHT] ^ pieces[Piece::B_BISHOP] ^ pieces[Piece::B_ROOK] ^ pieces[Piece::B_QUEEN] ^ pieces[Piece::B_KING];
    }
}