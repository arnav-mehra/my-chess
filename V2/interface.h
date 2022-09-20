#pragma once

<<<<<<< HEAD
=======


<<<<<<< Updated upstream
=======
>>>>>>> 0990fb1dff89ad5922db1d893123056578fe63de
>>>>>>> Stashed changes
void Board::initializeBoard() {
    turn = true;
    pieces[Piece::WHITE] = UINT64_C(18446462598732840960);
    pieces[Piece::BLACK] = UINT64_C(65535);
    //pawns
    pieces[0] = UINT64_C(71776119061282560) & UINT64_C(18446462598732840960);
    pieces[6] = UINT64_C(71776119061282560) & UINT64_C(65535);
    //knights and bishops
    pieces[1] = UINT64_C(4755801206503243842) & UINT64_C(18446462598732840960);
    pieces[7] = UINT64_C(4755801206503243842) & UINT64_C(65535);
    pieces[2] = UINT64_C(2594073385365405732) & UINT64_C(18446462598732840960);
    pieces[8] = UINT64_C(2594073385365405732) & UINT64_C(65535);
    //rooks
    pieces[3] = UINT64_C(9295429630892703873) & UINT64_C(18446462598732840960);
    pieces[9] = UINT64_C(9295429630892703873) & UINT64_C(65535);
    //queen and king
    pieces[4] = UINT64_C(576460752303423496) & UINT64_C(18446462598732840960);
    pieces[10] = UINT64_C(576460752303423496) & UINT64_C(65535);
    pieces[5] = UINT64_C(1152921504606846992) & UINT64_C(18446462598732840960);
    pieces[11] = UINT64_C(1152921504606846992) & UINT64_C(65535);

    pieces[14] = UINT64_C(18446462598732840960) | UINT64_C(65535);
    pieces[15] = ~pieces[14];
}

<<<<<<< HEAD
=======

<<<<<<< Updated upstream
=======
>>>>>>> 0990fb1dff89ad5922db1d893123056578fe63de
>>>>>>> Stashed changes
void Board::setBoard (char boardArr[64]) {
    clear();
    for (int i = 0; i < 64; i++) {
        U64 val = 1ULL << i;
        switch (boardArr[i]) {
            case 'r': pieces[Piece::BLACK]+=val; pieces[Piece::B_ROOK]+=val; break;
            case 'n': pieces[Piece::BLACK]+=val; pieces[Piece::B_KNIGHT]+=val; break;
            case 'b': pieces[Piece::BLACK]+=val; pieces[Piece::B_BISHOP]+=val; break;
            case 'q': pieces[Piece::BLACK]+=val; pieces[Piece::B_QUEEN]+=val; break;
            case 'k': pieces[Piece::BLACK]+=val; pieces[Piece::B_KING]+=val; break;
            case 'p': pieces[Piece::BLACK]+=val; pieces[Piece::B_PAWN]+=val; break;
            
            case 'R': pieces[Piece::WHITE]+=val; pieces[Piece::W_ROOK]+=val; break;
            case 'N': pieces[Piece::WHITE]+=val; pieces[Piece::W_KNIGHT]+=val; break;
            case 'B': pieces[Piece::WHITE]+=val; pieces[Piece::W_BISHOP]+=val; break;
            case 'Q': pieces[Piece::WHITE]+=val; pieces[Piece::W_QUEEN]+=val; break;
            case 'K': pieces[Piece::WHITE]+=val; pieces[Piece::W_KING]+=val; break;
            case 'P': pieces[Piece::WHITE]+=val; pieces[Piece::W_PAWN]+=val; break;
        }
    }
    pieces[Piece::OCCUP] = pieces[Piece::WHITE] ^ pieces[Piece::BLACK];
    pieces[Piece::UNOCC] = ~pieces[Piece::OCCUP];

    if (pieces[Piece::W_KING] != 1ULL << 60) fromDist[60]++;
    if (pieces[Piece::B_KING] != 1ULL << 4) fromDist[4]++;
    if (pieces[Piece::W_ROOK] != 1ULL << 56) fromDist[56]++;
    if (pieces[Piece::W_ROOK] != 1ULL << 63) fromDist[63]++;
    if (pieces[Piece::B_ROOK] != 1ULL << 0) fromDist[0]++;
    if (pieces[Piece::B_ROOK] != 1ULL << 7) fromDist[7]++;
}

void Board::setBoardFen (string FENStr) {
    char boardStr[64];
    for (int i=0, sq=0; i<FENStr.size(); i++) {
        char letter = FENStr[i];
        if (letter != '/') {
            if (isdigit(letter)) {
            sq += letter - 48;
            } else {
            boardStr[sq] = FENStr[i];
            sq++;
            }
        }
    }
    setBoard(boardStr);
}

void Board::printBoard() {
    cout << '\n';
    bool currTurn = turn;

    for (int i = 0; i < 64; i++) {
        turn = false;
        int whitePiece = getCapturedPiece(i);
        turn = true;
        int blackPiece = getCapturedPiece(i);
        
        if (whitePiece != 12) {
            cout << Piece::letters[whitePiece];
        } else if (blackPiece != 12) {
            cout << Piece::letters[blackPiece];
        } else {
            cout << '.';
        }

        cout << " ";
        if (i % 8 == 7) {
            cout << '\n';
        }
    }

    turn = currTurn;
}

void Board::clear() {
<<<<<<< Updated upstream
=======
<<<<<<< HEAD
    for (int i = 0; i < 15; i++) pieces[i] = 0ULL;
    for (int i = 0; i < 64; i++) fromDist[i] = 0U;
    turn = true;
}
=======
>>>>>>> Stashed changes
    for (int i=0; i<15; i++) {
    pieces[i] = 0;
    }
    wKingSqMoves = wLRookSqMoves = wRRookSqMoves = 0; 
    bKingSqMoves = bLRookSqMoves = bRRookSqMoves = 0;
}
>>>>>>> 0990fb1dff89ad5922db1d893123056578fe63de
