#pragma once

class Board { 
public:
  bool turn = true;
  U64 pieces[16] = {
    0, 0, 0, 0, 0, 0, // white pieces
    0, 0, 0, 0, 0, 0, // black pieces
    0, 0, // white/black
    0, 0 // occupied/unoccupied
  };
  U32 fromDist[64] = {};

  // generate all or just capturing moves
  inline void moveGen(MoveList &moves);
  inline void genPawnMoves(MoveList &moves);
  inline void genKnightMoves(MoveList &moves);
  inline void genBishopMoves(MoveList &moves);
  inline void genRookMoves(MoveList &moves);
  inline void genQueenMoves(MoveList &moves);
  inline void genKingMoves(MoveList &moves);
  // castling and check
  inline void genCastlingMoves(MoveList &moves);
  inline bool isWhiteChecked(U32 currSquare);
  inline bool isBlackChecked(U32 currSquare);
  inline bool isCheck();
  inline bool canBlackLeftCastle();
  inline bool canWhiteLeftCastle();
  inline bool canBlackRightCastle();
  inline bool canWhiteRightCastle();
  // just captures
  inline void captureGen(MoveList &moves);
  inline void genPawnCaptures(MoveList &moves);
  inline void genKnightCaptures(MoveList &moves);
  inline void genBishopCaptures(MoveList &moves);
  inline void genRookCaptures(MoveList &moves);
  inline void genQueenCaptures(MoveList &moves);
  inline void genKingCaptures(MoveList &moves);
  // moving
  inline void toggleTurn();
  inline void incrementFromDist(U32 fromSq);
  inline void decrementFromDist(U32 fromSq);
  inline void performMove(Move &move);
  inline void move(Move &move);
  inline void unmove(Move &move);
  inline U64 getBoardKey();
  inline S16 evaluate();  

  // interface
  void initializeBoard();
  void setBoard(char boardArr[64]);
  void setBoardFen(string FENStr);
  void printBoard();
  void clear();

private:
  // returns piece captured as piece[] index
  inline MoveCapture::Piece getCapturedPiece(U32 currSq) {
    U64 currVal = 1ULL << currSq;
    if (turn) {
      for (U32 start = 6; start < 12; start++)
        if (pieces[start] & currVal)
          return (MoveCapture::Piece) start;
    } else {
      for (U32 start = 0; start < 6; start++)
        if (pieces[start] & currVal)
          return (MoveCapture::Piece) start;
    }
    return MoveCapture::NONE; // error
  }

  inline MoveCapture::Piece checkCapturedPiece(U32 currSq) {
    U64 currVal = 1ULL << currSq;
    if (turn) {
      if (pieces[Piece::WHITE] & currVal)
        return MoveCapture::NONE;
      for (int start = 6; start < 12; start++)
        if (pieces[start] & currVal)
          return (MoveCapture::Piece) start;
    } else {
      if (pieces[Piece::BLACK] & currVal)
        return MoveCapture::NONE;
      for (int start = 0; start < 6; start++)
        if (pieces[start] & currVal)
          return (MoveCapture::Piece) start;
    }
    return MoveCapture::NONE; // error
  }
  
  // shorthand methods
  inline U64& getP() { return pieces[Piece::W_PAWN]; }
  inline U64& getp() { return pieces[Piece::B_PAWN]; }
  inline U64& getN() { return pieces[Piece::W_KNIGHT]; }
  inline U64& getn() { return pieces[Piece::B_KNIGHT]; }
  inline U64& getB() { return pieces[Piece::W_BISHOP]; }
  inline U64& getb() { return pieces[Piece::B_BISHOP]; }
  inline U64& getR() { return pieces[Piece::W_ROOK]; }
  inline U64& getr() { return pieces[Piece::B_ROOK]; }
  inline U64& getQ() { return pieces[Piece::W_QUEEN]; }
  inline U64& getq() { return pieces[Piece::B_QUEEN]; }
  inline U64& getK() { return pieces[Piece::W_KING]; }
  inline U64& getk() { return pieces[Piece::B_KING]; }
  inline U64& getOccupied() { return pieces[Piece::OCCUP]; }
  inline U64& getUnoccupied() { return pieces[Piece::UNOCC]; }
  inline U64& getColorOccup() { return pieces[turn ? Piece::WHITE : Piece::BLACK]; }
  inline U64& getNotColorOccup() { return pieces[turn ? Piece::BLACK : Piece::WHITE]; }
  inline Piece::Piece getColor() { return turn ? Piece::WHITE : Piece::BLACK; }
  inline Piece::Piece getNotColor() { return turn ? Piece::BLACK : Piece::WHITE; }
};