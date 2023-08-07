#include "../../util/types.hpp"

class BoardArr {
public:
    U8 arr[64];

    Piece get(int i) {
        return (Piece)arr[i];
    }

    void set(int i, Piece pc) {
        arr[i] = (U8)pc;
    }
};