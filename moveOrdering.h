#include <vector>
#include <algorithm>
using namespace std;

int captureVal[6] = {100, 250, 300, 500, 900, 10000};
int quietVal[12] = {
    90, -10, -10, -10, //promo
    2, 3, 3, 0, 1, -20, //quiet
    7, 5 //castle
};


inline bool compareMoves(vector<int> move1, vector<int> move2) {
    int c1 = move1[3], p1 = move1[0];
    int c2 = move2[3], p2 = move2[0];
    return 
        (c1 == -1? quietVal[p1+4] : (captureVal[c1] - captureVal[p1])) 
        > 
        (c2 == -1? quietVal[p2+4] : (captureVal[c2] - captureVal[p2]))
    ;
}

inline vector<vector<int>> orderMoves(vector<vector<int>> moves) {
    sort(moves.begin(), moves.end(), compareMoves);
    return moves;
}


//all pieces, 0-5 are also captures
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
//left castle: 6
//right castle: 7


//old: 1553529