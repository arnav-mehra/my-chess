using namespace std;



void printGrid (uint64_t num) {
  for (int i=0; i<64; i++) {
    cout << num%2 << " ";
    num /= 2;
    if (i%8==7) {
      cout << '\n';
    }
  }
}
void printNumGrid () {
  for (int i=0; i<8; i++) {
    cout << '\n';
    for (int j=0; j<8; j++) {
      cout << (j+i*8<10? "0" : "") << j+i*8 << " ";
    }
  }
}

void printTable(uint64_t table[64][4096]) {
  for (int i=0; i<64; i++) {
    for (int j=0; j<4096; j++) {
      if (table[i][j] != 0) {
        cout << "\n";
        printGrid(table[i][j]);
        cout << "\n";
      }
    }
  }
}

void printMoves(vector<Move> moves) {
  for (int i=0; i<moves.size(); i++) {
    switch (moves[i].piece) {
      case 0: cout << "P"; break;
      case 1: cout << "N"; break;
      case 2: cout << "B"; break;
      case 3: cout << "R"; break;
      case 4: cout << "Q"; break;
      case 5: cout << "K"; break;
      case 6: cout << "O"; break;
      case 7: cout << "O"; break;
    }
    cout << ": (" << (int)(moves[i].fromSq) << ", " << (int)(moves[i].toSq) << ") ";
    switch (moves[i].capture) {
      case 0: cout << "P"; break;
      case 1: cout << "N"; break;
      case 2: cout << "B"; break;
      case 3: cout << "R"; break;
      case 4: cout << "Q"; break;
      case 5: cout << "K"; break;
    }
    cout << '\n';
  }
  cout << " Length: " << moves.size() << '\n';
}

void printMove(Move &m) {
  char pieceChars[] = {'P', 'N', 'B', 'R', 'Q', 'K', 'O', 'O'};
  char letters[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

  char p = pieceChars[m.piece];
  int f = m.fromSq, t = m.toSq;

  if (m.capture != 16) {
    cout << p << 'x' << letters[t%8] << 8-t/8 << '\n';
  } else {
    cout << p << letters[t%8] << 8-t/8 << '\n';
  }
}
void printMovePure(Move move) {
  cout << move.piece << ", " << move.fromSq << ", " << move.toSq << ", " << move.capture << '\n';
}

void quirkyStatement(int eval) {
  if (eval > 500) {
    cout << "LOL, just quit already.";
  } else if (eval > 200) {
    cout << "Bruh, I'm POGing out right now!";
  } else if (eval > 100) {
    cout << "Hmmm, I should be winning...";
  } else if (eval > -100) {
    cout << "Hmmm, this is a fairly equal match...";
  } else if (eval > -200) {
    cout << "Wait, I might be losing.";
  } else if (eval > -350) {
    cout << "Wow, you are good.";
  } else if (eval > -500) {
    cout << "All I can do is keep playing I guess.";
  } else {
    cout << "I surrender.";
  }
  cout << '\n';
}

