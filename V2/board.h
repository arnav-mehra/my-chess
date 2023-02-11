#ifndef Board_H
#define Board_H

using namespace std;


class Board { 
  
  public:


    bool turn = true;
    uint64_t pieces[16] = {
      0, 0, 0, 0, 0, 0, //white pieces
      0, 0, 0, 0, 0, 0,  //black pieces
      0, 0, //colors
      0, 0 //combined, empty
    };
    uint8_t wKingSqMoves=0; uint8_t wLRookSqMoves=0; uint8_t wRRookSqMoves=0; 
    uint8_t bKingSqMoves=0; uint8_t bLRookSqMoves=0; uint8_t bRRookSqMoves=0;


    // generate all or just capturing moves
    void moveGen (vector<Move> &moves);
    void genCastlingMoves (vector<Move> &moves);
    void genPawnMoves (vector<Move> &moves, uint64_t cPawns);
    void genKnightMoves (vector<Move> &moves, uint64_t cKnights);
    void genBishopMoves (vector<Move> &moves, uint64_t cBishops);
    void genRookMoves (vector<Move> &moves, uint64_t cRooks);
    void genQueenMoves (vector<Move> &moves, uint64_t cQueens);
    void genKingMoves (vector<Move> &moves, uint8_t kingSquare);
    
    void captureGen (vector<Move> &moves);
    void genPawnCaptures (vector<Move> &moves, uint64_t cPawns);
    void genKnightCaptures (vector<Move> &moves, uint64_t cKnights);
    void genBishopCaptures (vector<Move> &moves, uint64_t cBishops);
    void genRookCaptures (vector<Move> &moves, uint64_t cRooks);
    void genQueenCaptures (vector<Move> &moves, uint64_t cQueens);
    void genKingCaptures (vector<Move> &moves, uint8_t currSquare);
    




    bool canRightCastle();
    bool canLeftCastle();

    bool isCheck();
    bool isCheck (uint8_t currSquare);

    constexpr void toggleMove() { turn = !turn; }
    void move (Move &move);
    void unmove (Move &move);
    int16_t evaluate();
    inline array<uint64_t,7> getBoardKey();

    //other functions for setting up and printing board
    constexpr void initializeBoard();
    constexpr void setBoard (char boardArr[64]);
    void setBoardFen (string FENStr);
    void printBoard();
    constexpr void clear();



  private: 

    //returns piece captured as piece[] index
    int8_t getCapturedPiece(int8_t currSq) {
      uint64_t currVal = 1ULL << currSq;
      int8_t end = turn? 12 : 6;
      for (int8_t start = turn? 6 : 0; start < end; start++) {
        if (pieces[start] & currVal) { return start; }
      }
      return 16;
    }
    
    //just shorthand methods, might want to make macro
    inline uint64_t getP() { return pieces[0];  }
    inline uint64_t getp() { return pieces[6];  }
    inline uint64_t getN() { return pieces[1];  }
    inline uint64_t getn() { return pieces[7];  }
    inline uint64_t getB() { return pieces[2];  }
    inline uint64_t getb() { return pieces[8];  }
    inline uint64_t getR() { return pieces[3];  }
    inline uint64_t getr() { return pieces[9];  }
    inline uint64_t getQ() { return pieces[4];  }
    inline uint64_t getq() { return pieces[10]; }
    inline uint64_t getK() { return pieces[5];  }
    inline uint64_t getk() { return pieces[11]; }
    inline uint64_t getOccupied() { return pieces[14]; }
    inline uint64_t getUnoccupied() { return pieces[15]; }
    inline uint64_t getTurn() { return pieces[turn? 12 : 13]; }
    inline uint64_t getNotTurn() { return pieces[turn? 13 : 12]; }

    constexpr void setWhite() { pieces[Piece::WHITE] = pieces[Piece::W_PAWN] ^ pieces[Piece::W_KNIGHT] ^ pieces[Piece::W_BISHOP] ^ pieces[Piece::W_ROOK] ^ pieces[Piece::W_QUEEN] ^ pieces[Piece::W_KING]; }
    constexpr void setBlack() { pieces[Piece::BLACK] = pieces[Piece::B_PAWN] ^ pieces[Piece::B_KNIGHT] ^ pieces[Piece::B_BISHOP] ^ pieces[Piece::B_ROOK] ^ pieces[Piece::B_QUEEN] ^ pieces[Piece::B_KING]; }
    constexpr void setOccupied() { pieces[Piece::OCCUP] = pieces[Piece::BLACK] | pieces[Piece::WHITE]; }
    constexpr void setUnoccupied() { pieces[Piece::UNOCC] = ~pieces[Piece::OCCUP]; }
};

#endif