#ifndef Board_H
#define Board_H

#include "moveData.h"
#include "moveOrdering.h"
#include <vector>
#include <iostream>
#include <cstdint>
using namespace std;

#define LOG2(X) ((unsigned) (8*sizeof (unsigned long long) - __builtin_clzll((X)) - 1))

//queen promo: -4
//rook promo: -3
//bishop promo: -2
//knight promo: -1
//pawn: 0
//knight: 1
//bishop: 2
//rook: 3
//queen: 4
//king: 5



class Board { public:

  uint64_t white, black, pawns, knights, bishops, rooks, queens, kings, unmoved, enpassant;


  vector<vector<int>> moveGen (bool turn) {

    uint64_t combined  = black | white;
    uint64_t empty  = ~combined;
    vector<vector<int>> moves;
    bool checked = false;

    //king positions
    uint64_t cKing = turn? getK() : getk();
    if (cKing == 0) { return {}; }
    if (checked) {
      int currSquare = LOG2(cKing);
      uint64_t attacks = kingMoves[currSquare] & getColor(!turn);
      uint64_t openMoves = kingMoves[currSquare] & empty;
      while (attacks != 0) {
        int toSquare = bitScanForward(attacks);
        int capture = getPiece(toSquare);
        if (capture == -1) { cout << "kings\n"; }
        moves.push_back({5, currSquare, toSquare, capture});
        attacks &= attacks-1;
      }
      while (openMoves != 0) {
        int toSquare = bitScanForward(openMoves);
        moves.push_back({5, currSquare, toSquare, -1});
        openMoves &= openMoves-1;
      }
    }


    //piece positions
    uint64_t cPawns = turn? getP() : getp();
    uint64_t up1 = (turn? cPawns >> 8 : cPawns << 8) & empty;
    uint64_t up2 = (turn? up1 >> 8 & OgPawnPos : up1 << 8 & OgPawnPos2) & empty;
    
    uint64_t cKnights = turn? getN() : getn(), cBishops = turn? getB() : getb(), 
    cRooks = turn? getR() : getr(), cQueen = turn? getQ() : getq();

    //pawn attacks/captures
    while (cPawns != 0) { 
      int currSquare = bitScanForward(cPawns);
      uint64_t attacks = turn? wPawnAttacks[currSquare] & black : bPawnAttacks[currSquare] & white;
      while (attacks != 0) {
        int toSquare = bitScanForward(attacks);
        int capture = getPiece(toSquare);
        if (capture == -1) { cout << "pawns\n"; }
        moves.push_back({0, currSquare, toSquare, capture});
        attacks &= attacks-1;
      }
      cPawns &= cPawns-1;
    }

    //pawn forward 2
    while (up2 != 0) { 
      int toSquare = bitScanForward(up2);
      int fromSquare = turn? toSquare+16 : toSquare-16;
      moves.push_back({0, fromSquare, toSquare, -1});
      up2 &= up2-1;
    }
    
    //knight moves
    while (cKnights != 0) {
      int currSquare = bitScanForward(cKnights);
      uint64_t attacks = knightMoves[currSquare] & getColor(!turn);
      uint64_t openMoves = knightMoves[currSquare] & empty;
      while (attacks != 0) {
        int toSquare = bitScanForward(attacks);
        int capture = getPiece(toSquare);
        if (capture == -1) { cout << "knights\n"; }
        moves.push_back({1, currSquare, toSquare, capture});
        attacks &= attacks-1;
      }
      while (openMoves != 0) {
        int toSquare = bitScanForward(openMoves);
        moves.push_back({1, currSquare, toSquare, -1});
        openMoves &= openMoves-1;
      }
      cKnights &= cKnights-1;
    }
    
    //pawn moves 1
    while (up1 != 0) {
      int toSquare = bitScanForward(up1);
      int fromSquare = turn? toSquare+8 : toSquare-8;
      if (turn? toSquare < 8 : toSquare > 55) {
        moves.push_back({-4, fromSquare, toSquare, -1});
        moves.push_back({-3, fromSquare, toSquare, -1});
        moves.push_back({-2, fromSquare, toSquare, -1});
        moves.push_back({-1, fromSquare, toSquare, -1});
      } else {
        moves.push_back({0, fromSquare, toSquare, -1});
      }
      up1 &= up1-1;
    }

    //bishop moves, unchecked
    while (cBishops != 0) {
      int currSquare = bitScanForward(cBishops);
      uint64_t bishopMoves = Bmagic(currSquare, combined);
      uint64_t attacks = bishopMoves & getColor(!turn);
      uint64_t openMoves = bishopMoves & empty;
      while (attacks != 0) {
        int toSquare = bitScanForward(attacks);
        int capture = getPiece(toSquare);
        if (capture == -1) { cout << "bishops\n"; }
        moves.push_back({2, currSquare, toSquare, capture});
        attacks &= attacks-1;
      }
      while (openMoves != 0) {
        int toSquare = bitScanForward(openMoves);
        moves.push_back({2, currSquare, toSquare, -1});
        openMoves &= openMoves-1;
      }
      cBishops &= cBishops-1;
    }

    //queen moves, unchecked
    while (cQueen != 0) {
      int currSquare = bitScanForward(cQueen);
      uint64_t queenMoves = Bmagic(currSquare, combined) ^ Rmagic(currSquare, combined);
      uint64_t attacks = queenMoves & getColor(!turn);
      uint64_t openMoves = queenMoves & empty;
      while (attacks != 0) {
        int toSquare = bitScanForward(attacks);
        int capture = getPiece(toSquare);
        if (capture == -1) { cout << "queens\n"; }
        moves.push_back({4, currSquare, toSquare, capture});
        attacks &= attacks-1;
      }
      while (openMoves != 0) {
        int toSquare = bitScanForward(openMoves);
        moves.push_back({4, currSquare, toSquare, -1});
        openMoves &= openMoves-1;
      }
      cQueen &= cQueen-1;
    }

    //rook moves
    while (cRooks != 0) {
      int currSquare = bitScanForward(cRooks);
      uint64_t rookMoves = Rmagic(currSquare, combined);
      uint64_t attacks = rookMoves & getColor(!turn);
      uint64_t openMoves = rookMoves & empty;
      while (attacks != 0) {
        int toSquare = bitScanForward(attacks);
        int capture = getPiece(toSquare);
        if (capture == -1) { cout << "rooks\n"; }
        moves.push_back({3, currSquare, toSquare, capture});
        attacks &= attacks-1;
      }
      while (openMoves != 0) {
        int toSquare = bitScanForward(openMoves);
        moves.push_back({3, currSquare, toSquare, -1});
        openMoves &= openMoves-1;
      }
      cRooks &= cRooks-1;
    }

    //king moves, at end for ordering
    if (!checked) {
      int currSquare = LOG2(cKing);
      uint64_t attacks = kingMoves[currSquare] & getColor(!turn);
      uint64_t openMoves = kingMoves[currSquare] & empty;
      while (attacks != 0) {
        int toSquare = bitScanForward(attacks);
        int capture = getPiece(toSquare);
        if (capture == -1) { cout << "kings\n"; }
        moves.push_back({5, currSquare, toSquare, capture});
        attacks &= attacks-1;
      }
      while (openMoves != 0) {
        int toSquare = bitScanForward(openMoves);
        moves.push_back({5, currSquare, toSquare, -1});
        openMoves &= openMoves-1;
      }
    }
    
    return moves;
  }
  vector<vector<int>> captureGen (bool turn) {
    uint64_t combined = black | white;
    vector<vector<int>> moves;

    uint64_t cKing = turn? getK() : getk();
    if (cKing) {
      int currSquare = LOG2(cKing);
      uint64_t attacks = kingMoves[currSquare] & getColor(!turn);
      while (attacks != 0) {
        int toSquare = bitScanForward(attacks);
        int capture = getPiece(toSquare);
        moves.push_back({5, currSquare, toSquare, capture});
        attacks &= attacks-1;
      }
    } else {
      return {};
    }
    
    uint64_t cPawns = turn? getP() : getp(), 
      cKnights = turn? getN() : getn(), cBishops = turn? getB() : getb(), 
      cRooks = turn? getR() : getr(), cQueen = turn? getQ() : getq();

    while (cPawns != 0) { 
      int currSquare = bitScanForward(cPawns);
      uint64_t attacks = turn? wPawnAttacks[currSquare] & black : bPawnAttacks[currSquare] & white;
      while (attacks != 0) {
        int toSquare = bitScanForward(attacks);
        int capture = getPiece(toSquare);
        moves.push_back({0, currSquare, toSquare, capture});
        attacks &= attacks-1;
      }
      cPawns &= cPawns-1;
    }
    while (cKnights != 0) {
      int currSquare = bitScanForward(cKnights);
      uint64_t attacks = knightMoves[currSquare] & getColor(!turn);
      while (attacks != 0) {
        int toSquare = bitScanForward(attacks);
        int capture = getPiece(toSquare);
        moves.push_back({1, currSquare, toSquare, capture});
        attacks &= attacks-1;
      }
      cKnights &= cKnights-1;
    }
    while (cBishops != 0) {
      int currSquare = bitScanForward(cBishops);
      uint64_t attacks = Bmagic(currSquare, combined) & getColor(!turn);
      while (attacks != 0) {
        int toSquare = bitScanForward(attacks);
        int capture = getPiece(toSquare);
        moves.push_back({2, currSquare, toSquare, capture});
        attacks &= attacks-1;
      }
      cBishops &= cBishops-1;
    }
    while (cRooks != 0) {
      int currSquare = bitScanForward(cRooks);
      uint64_t attacks = Rmagic(currSquare, combined) & getColor(!turn);
      while (attacks != 0) {
        int toSquare = bitScanForward(attacks);
        int capture = getPiece(toSquare);
        moves.push_back({3, currSquare, toSquare, capture});
        attacks &= attacks-1;
      }
      cRooks &= cRooks-1;
    }
    while (cQueen != 0) {
      int currSquare = bitScanForward(cQueen);
      uint64_t attacks = (Bmagic(currSquare, combined) ^ Rmagic(currSquare, combined)) & getColor(!turn);
      while (attacks != 0) {
        int toSquare = bitScanForward(attacks);
        int capture = getPiece(toSquare);
        moves.push_back({4, currSquare, toSquare, capture});
        attacks &= attacks-1;
      }
      cQueen &= cQueen-1;
    }
    
    return orderMoves(moves);
  }


  void move (int piece, uint64_t fromVal, uint64_t toVal, int capture, bool turn) {
    signed long long deltaVal = toVal - fromVal;

    //remove from unmoved, clear enpassantable
    enpassant = 0;
    if (unmoved & fromVal != 0) {
      unmoved -= fromVal;
    }

    //if capture, remove captured piece on color bitboard
    if (capture != -1) {
      if (turn) { black -= toVal; } else { white -= toVal; }
      switch (capture) {
        case 0: pawns -= toVal; break;
        case 1: knights -= toVal; break;
        case 2: bishops -= toVal; break;
        case 3: rooks -= toVal; break;
        case 4: queens -= toVal; break;
        case 5: kings -= toVal; break;
      }
    }
    
    //move piece
    if (turn) { white += deltaVal; } else { black += deltaVal; }
    switch (piece) {
      case 0: pawns += deltaVal; break;
      case 1: knights += deltaVal; break;
      case 2: bishops += deltaVal; break;
      case 3: rooks += deltaVal; break;
      case 4: queens += deltaVal; break;
      case 5: kings += deltaVal; break;
      case -1: pawns -= fromVal; knights += toVal; break;
      case -2: pawns -= fromVal; bishops += toVal; break;
      case -3: pawns -= fromVal; rooks += toVal; break;
      case -4: pawns -= fromVal; queens += toVal; break;
    }
  }
  void unmove (int piece, uint64_t fromVal, uint64_t toVal, int capture, bool turn) {
    signed long long deltaVal = fromVal - toVal;
    //move piece back
    if (turn) { white += deltaVal; } else { black += deltaVal; }
    switch (piece) {
      case 0: pawns += deltaVal; break;
      case 1: knights += deltaVal; break;
      case 2: bishops += deltaVal; break;
      case 3: rooks += deltaVal; break;
      case 4: queens += deltaVal; break;
      case 5: kings += deltaVal; break;
      case -1: pawns += fromVal; knights -= toVal; break;
      case -2: pawns += fromVal; bishops -= toVal; break;
      case -3: pawns += fromVal; rooks -= toVal; break;
      case -4: pawns += fromVal; queens -= toVal; break;
    }

    //place captured piece back
    if (capture != -1) {
      if (turn) { black += toVal; } else { white += toVal; }
      switch (capture) {
        case 0: pawns += toVal; break;
        case 1: knights += toVal; break;
        case 2: bishops += toVal; break;
        case 3: rooks += toVal; break;
        case 4: queens += toVal; break;
        case 5: kings += toVal; break;
      }
    }
  }


  int getPiece(int currSq) {
    uint64_t currVal = 1ULL << currSq;
    if (pawns & currVal) { return 0; } 
    else if (knights & currVal) { return 1; } 
    else if (bishops & currVal) { return 2; } 
    else if (rooks & currVal) { return 3; } 
    else if (queens & currVal) { return 4; } 
    else if (kings & currVal) { return 5; } 
    else { cout << "BAD CODE\n"; return -1; }
  }
  uint64_t getP() { return white & pawns;   }
  uint64_t getp() { return black & pawns;   }
  uint64_t getN() { return white & knights; }
  uint64_t getn() { return black & knights; }
  uint64_t getB() { return white & bishops; }
  uint64_t getb() { return black & bishops; }
  uint64_t getR() { return white & rooks;   }
  uint64_t getr() { return black & rooks;   }
  uint64_t getQ() { return white & queens;  }
  uint64_t getq() { return black & queens;  }
  uint64_t getK() { return white & kings;   }
  uint64_t getk() { return black & kings;   }
  uint64_t getColor (bool turn) { if (turn) { return white; } else { return black; }}


  void initializeBoard() {
    white   = UINT64_C(18446462598732840960);
    black   = UINT64_C(65535);
    pawns   = UINT64_C(71776119061282560);
    knights = UINT64_C(4755801206503243842);
    bishops = UINT64_C(2594073385365405732);
    rooks   = UINT64_C(9295429630892703873);
    queens  = UINT64_C(576460752303423496);
    kings   = UINT64_C(1152921504606846992);
    unmoved = UINT64_C(18446462598732906495);
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
    unmoved = white | black;
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
    uint64_t currSquare = UINT64_C(1);
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
      unmoved = UINT64_C(0);
    }
  int evaluate () {
    //material eval
    uint64_t wKing = getK(), bKing = getk();
    int kingDiff = __builtin_popcountll(wKing) - __builtin_popcountll(bKing);
    if (kingDiff != 0) {
        return 100000*kingDiff;
    }
    uint64_t wKnights = getN(), bKnights = getn();
    uint64_t wBishops = getB(), bBishops = getb();
    uint64_t wRooks   = getR(), bRooks   = getr();
    uint64_t wQueens  = getQ(), bQueens  = getq();
    uint64_t wPawns   = getP(), bPawns   = getp();

    int knightDiff = __builtin_popcountll(wKnights) - __builtin_popcountll(bKnights);
    int bishopDiff = __builtin_popcountll(wBishops) - __builtin_popcountll(bBishops);
    int rookDiff   = __builtin_popcountll(wRooks)   - __builtin_popcountll(bRooks);
    int pawnDiff   = __builtin_popcountll(wPawns)   - __builtin_popcountll(bPawns);
    int queenDiff  = __builtin_popcountll(wQueens)  - __builtin_popcountll(bQueens);
    int materialEval = 100*pawnDiff + 305*knightDiff + 333*bishopDiff + 563*rookDiff + 950*queenDiff;

    //positional eval
    int wPieces = __builtin_popcountll(getColor(true));
    int bPieces = __builtin_popcountll(getColor(false));
    int positionEval = 0;
    bool isEndGame = wPieces + bPieces < 8 || (getQ()==0 && getq()==0 && wPieces + bPieces < 16);

    while (wPawns != 0)   { positionEval += pawnEvalTable   [isEndGame][bitScanForward(wPawns)];      wPawns &= wPawns-1; }
    while (bPawns != 0)   { positionEval -= pawnEvalTable   [isEndGame][64-bitScanForward(bPawns)];   bPawns &= bPawns-1; }
    while (wKnights != 0) { positionEval += knightEvalTable [isEndGame][bitScanForward(wKnights)];    wKnights &= wKnights-1; }
    while (bKnights != 0) { positionEval -= knightEvalTable [isEndGame][64-bitScanForward(bKnights)]; bKnights &= bKnights-1; }
    while (wBishops != 0) { positionEval += bishopEvalTable [isEndGame][bitScanForward(wBishops)];    wBishops &= wBishops-1; }
    while (bBishops != 0) { positionEval -= bishopEvalTable [isEndGame][64-bitScanForward(bBishops)]; bBishops &= bBishops-1; }
    while (wRooks != 0)   { positionEval += rookEvalTable   [isEndGame][bitScanForward(wRooks)];      wRooks &= wRooks-1; }
    while (bRooks != 0)   { positionEval -= rookEvalTable   [isEndGame][64-bitScanForward(bRooks)];   bRooks &= bRooks-1; }
    while (wQueens != 0)  { positionEval += queenEvalTable  [isEndGame][bitScanForward(wQueens)];     wQueens &= wQueens-1;}
    while (bQueens != 0)  { positionEval -= queenEvalTable  [isEndGame][64-bitScanForward(bQueens)];  bQueens &= bQueens-1;}
    positionEval += kingEvalTable[isEndGame][LOG2(wKing)];
    positionEval -= kingEvalTable[isEndGame][64-LOG2(bKing)];

    return materialEval + positionEval;
  }
};

#endif