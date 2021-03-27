#include <vector>
#include <algorithm>
using namespace std;

bool compareMoves(vector<int> move1, vector<int> move2) {
    return (move1[3]-move1[0]) > (move2[3]-move2[0]);
}

vector<vector<int>> orderMoves(vector<vector<int>> moves) {
    sort(moves.begin(), moves.end(), compareMoves);
    return moves;
}