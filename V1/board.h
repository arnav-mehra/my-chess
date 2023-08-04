#ifndef Board_H
#define Board_H

#define LOG2(X) ((unsigned) (8*sizeof (unsigned long long) - __builtin_clzll((X)) - 1))

using namespace std;



class Board { 
  
  public:

    uint64_t white, black, pawns, knights, bishops, rooks, queens, kings, enpassant;
    uint8_t wKingMoves=0, wLRookMoves=0, wRRookMoves=0, bKingMoves=0, bLRookMoves=0, bRRookMoves=0;


    void moveGen (bool turn, vector<Move> &moves);
    void captureGen (bool turn, vector<Move> &moves);

    
    void move (Move &move, bool turn) {
      enpassant = 0;      

      //if capture, remove captured piece on color bitboard
      if (move.capture != -1) {
        if (turn) { black -= move.toVal; } else { white -= move.toVal; }
        switch (move.capture) {
          case 0: pawns -= move.toVal; break;
          case 1: knights -= move.toVal; break;
          case 2: bishops -= move.toVal; break;
          case 3: rooks -= move.toVal; break;
          case 4: queens -= move.toVal; break;
          case 5: kings -= move.toVal; break;
        }
      }
      
      //move piece
      if (turn) { white += move.deltaVal; } else { black += move.deltaVal; }
      switch (move.piece) {
        case 0: pawns += move.deltaVal; break;
        case 1: knights += move.deltaVal; break;
        case 2: bishops += move.deltaVal; break;
        case 3: 
          rooks += move.deltaVal;
          if (turn) { 
            if (move.fromSq%8 < 4) { wLRookMoves++; } else { wRRookMoves++; }
          } else {
            if (move.fromSq%8 < 4) { bLRookMoves++; } else { bRRookMoves++; }
          }
          break;
        case 4: queens += move.deltaVal; break;
        case 5: 
          kings += move.deltaVal; 
          if (turn) { wKingMoves++; } else { bKingMoves++; } 
          break;
        case 6: { //left castle
          kings += move.deltaVal; 
          signed long long rookShift = (move.fromVal >> 1) - (move.toVal >> 2);
          rooks += rookShift;
          if (turn) { 
            white += rookShift;
            wLRookMoves++; wRRookMoves++; wKingMoves++;
          } else { 
            black += rookShift;
            bLRookMoves++; bRRookMoves++; bKingMoves++;
          }
          break;
        }
        case 7: { //right castle
          kings += move.deltaVal;
          signed long long rookShift = (move.fromVal << 1) - (move.toVal << 1);
          rooks += rookShift;
          if (turn) { 
            white += rookShift;
            wLRookMoves++; wRRookMoves++; wKingMoves++;
          } else { 
            black += rookShift;
            bLRookMoves++; bRRookMoves++; bKingMoves++;
          }
          break;
        }
        case -1: pawns -= move.fromVal; knights += move.toVal; break;
        case -2: pawns -= move.fromVal; bishops += move.toVal; break;
        case -3: pawns -= move.fromVal; rooks += move.toVal; break;
        case -4: pawns -= move.fromVal; queens += move.toVal; break;
      }
    }
    void unmove (Move &move, bool turn) {
      
      //move piece back
      if (turn) { white -= move.deltaVal; } else { black -= move.deltaVal; }

      switch (move.piece) {
        case 0: pawns -= move.deltaVal; break;
        case 1: knights -= move.deltaVal; break;
        case 2: bishops -= move.deltaVal; break;
        case 3: 
          rooks -= move.deltaVal;
          if (turn) { 
            if (move.fromSq%8 < 4) { wLRookMoves--; } else { wRRookMoves--; }
          } else {
            if (move.fromSq%8 < 4) { bLRookMoves--; } else { bRRookMoves--; }
          }
          break;
        case 4: queens -= move.deltaVal; break;
        case 5: 
          kings -= move.deltaVal; 
          if (turn) { wKingMoves--; } else { bKingMoves--; }
          break;
        case 6: { //
          kings -= move.deltaVal; 
          signed long long rookShift = (move.toVal >> 2) - (move.fromVal >> 1);
          rooks += rookShift;
          if (turn) {
            white += rookShift;
            wLRookMoves--; wRRookMoves--; wKingMoves--;
          } else {
            black += rookShift;
            bLRookMoves--; bRRookMoves--; bKingMoves--;
          }
          break;
        }
        case 7: {
          kings -= move.deltaVal;
          signed long long rookShift = (move.toVal << 1) - (move.fromVal << 1);
          rooks += rookShift;
          if (turn) { 
            white += rookShift;
            wLRookMoves--; wRRookMoves--; wKingMoves--;
          } else { 
            black += rookShift;
            bLRookMoves--; bRRookMoves--; bKingMoves--;
          }
          break;
        }
        case -1: pawns += move.fromVal; knights -= move.toVal; break;
        case -2: pawns += move.fromVal; bishops -= move.toVal; break;
        case -3: pawns += move.fromVal; rooks -= move.toVal; break;
        case -4: pawns += move.fromVal; queens -= move.toVal; break;
      }

      //place captured piece back
      if (move.capture != -1) {
        if (turn) { black += move.toVal; } else { white += move.toVal; }
        switch (move.capture) {
          case 0: pawns += move.toVal; break;
          case 1: knights += move.toVal; break;
          case 2: bishops += move.toVal; break;
          case 3: rooks += move.toVal; break;
          case 4: queens += move.toVal; break;
          case 5: kings += move.toVal; break;
        }
      }
    }


    int16_t evaluate () {
      //king precondition
      uint64_t wKing = getK(), bKing = getk();
      if (wKing == 0) { return -10000; }
      if (bKing == 0) { return 10000; }


      int16_t eval = 0;

      uint64_t wKnights = getN(), bKnights = getn();
      uint64_t wBishops = getB(), bBishops = getb();
      uint64_t wRooks   = getR(), bRooks   = getr();
      uint64_t wQueens  = getQ(), bQueens  = getq();
      uint64_t wPawns   = getP(), bPawns   = getp();

      uint8_t wPieces = __builtin_popcountll(getColor(true));
      uint8_t bPieces = __builtin_popcountll(getColor(false));
      bool isEndGame = (wPieces + bPieces < 8) || (getQ()==0 && getq()==0 && wPieces + bPieces < 16);

      while (wPawns != 0)   { eval += pawnEvalTable   [isEndGame][bitScanForward(wPawns)]      + 100; wPawns &= wPawns - 1;   }
      while (bPawns != 0)   { eval -= pawnEvalTable   [isEndGame][64-bitScanForward(bPawns)]   + 100; bPawns &= bPawns-1;     }
      while (wKnights != 0) { eval += knightEvalTable [isEndGame][bitScanForward(wKnights)]    + 305; wKnights &= wKnights-1; }
      while (bKnights != 0) { eval -= knightEvalTable [isEndGame][64-bitScanForward(bKnights)] + 305; bKnights &= bKnights-1; }
      while (wBishops != 0) { eval += bishopEvalTable [isEndGame][bitScanForward(wBishops)]    + 333; wBishops &= wBishops-1; }
      while (bBishops != 0) { eval -= bishopEvalTable [isEndGame][64-bitScanForward(bBishops)] + 333; bBishops &= bBishops-1; }
      while (wRooks != 0)   { eval += rookEvalTable   [isEndGame][bitScanForward(wRooks)]      + 563; wRooks &= wRooks-1;     }
      while (bRooks != 0)   { eval -= rookEvalTable   [isEndGame][64-bitScanForward(bRooks)]   + 563; bRooks &= bRooks-1;     }
      while (wQueens != 0)  { eval += queenEvalTable  [isEndGame][bitScanForward(wQueens)]     + 950; wQueens &= wQueens-1;   }
      while (bQueens != 0)  { eval -= queenEvalTable  [isEndGame][64-bitScanForward(bQueens)]  + 950; bQueens &= bQueens-1;   }
      eval += kingEvalTable[isEndGame][LOG2(wKing)] - kingEvalTable[isEndGame][64-LOG2(bKing)];

      return eval;
    }

    bool canRightCastle (bool turn, uint64_t &empty);
    bool canLeftCastle (bool turn, uint64_t &empty);
    bool isCheck (bool turn, uint8_t currSquare);


    void initializeBoard() {
      white   = UINT64_C(18446462598732840960);
      black   = UINT64_C(65535);
      pawns   = UINT64_C(71776119061282560);
      knights = UINT64_C(4755801206503243842);
      bishops = UINT64_C(2594073385365405732);
      rooks   = UINT64_C(9295429630892703873);
      queens  = UINT64_C(576460752303423496);
      kings   = UINT64_C(1152921504606846992);
      enpassant = UINT64_C(0);
    }
    void setBoard (char boardArr[64]) {
      clear();
      for (int i=0; i<64; i++) {
        uint64_t val = 1ULL << i;
        switch (boardArr[i]) {
          case 'r': black+=val; rooks+=val; break;
          case 'n': black+=val; knights+=val; break;
          case 'b': black+=val; bishops+=val; break;
          case 'q': black+=val; queens+=val; break;
          case 'k': black+=val; kings+=val; break;
          case 'p': black+=val; pawns+=val; break;
          
          case 'R': white+=val; rooks+=val; break;
          case 'N': white+=val; knights+=val; break;
          case 'B': white+=val; bishops+=val; break;
          case 'Q': white+=val; queens+=val; break;
          case 'K': white+=val; kings+=val; break;
          case 'P': white+=val; pawns+=val; break;
        }
      }
    }
    void setBoardFen (string FENStr) {
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
    
    void printBoard () {
      uint64_t currSquare = 1ULL;
      cout << '\n';

      for (int i=0; i<64; i++) {
        bool color = white & currSquare;
        if (pawns & currSquare) {
          cout << (color? "P" : "p");
        } else if (knights & currSquare) {
          cout << (color? "N" : "n");
        } else if (bishops & currSquare) {
          cout << (color? "B" : "b");
        } else if (rooks & currSquare) {
          cout << (color? "R" : "r");
        } else if (queens & currSquare) {
          cout << (color? "Q" : "q");
        } else if (kings & currSquare) {
          cout << (color? "K" : "k");
        } else {
          cout << ".";
        }
        cout << " ";
        
        if (i%8 == 7) {
          cout << '\n';
        }
        currSquare *= 2;
      }
    }
    void clear() {
      white   = UINT64_C(0);
      black   = UINT64_C(0);
      pawns   = UINT64_C(0);
      knights = UINT64_C(0);
      bishops = UINT64_C(0);
      rooks   = UINT64_C(0);
      queens  = UINT64_C(0);
      kings   = UINT64_C(0);
    }
    
    

  private: 

    uint8_t getPiece(uint8_t currSq) {
      uint64_t currVal = 1ULL << currSq;
      if (pawns & currVal) { return 0; } 
      else if (knights & currVal) { return 1; } 
      else if (bishops & currVal) { return 2; } 
      else if (rooks & currVal) { return 3; } 
      else if (queens & currVal) { return 4; } 
      else if (kings & currVal) { return 5; } 
      else { return -1; }
    }
    
    inline uint64_t getP() { return white & pawns;   }
    inline uint64_t getp() { return black & pawns;   }
    inline uint64_t getN() { return white & knights; }
    inline uint64_t getn() { return black & knights; }
    inline uint64_t getB() { return white & bishops; }
    inline uint64_t getb() { return black & bishops; }
    inline uint64_t getR() { return white & rooks;   }
    inline uint64_t getr() { return black & rooks;   }
    inline uint64_t getQ() { return white & queens;  }
    inline uint64_t getq() { return black & queens;  }
    inline uint64_t getK() { return white & kings;   }
    inline uint64_t getk() { return black & kings;   }
    inline uint64_t getColor (bool turn) { return turn? white : black; }

};

#endif