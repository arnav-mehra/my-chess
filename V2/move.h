using namespace std;


int16_t const captureVal[6] = {100, 250, 300, 500, 900, 10000};
int16_t const quietVal[12] = {
    90, -10, -10, -10, //promo
    2, 3, 3, 0, 1, -20, //quiet
    7, 5 //castle
};


class Move { 

    public:

        uint8_t capture; uint8_t piece;
        uint8_t fromSq; uint8_t toSq;        
        int16_t priority;


        Move(uint8_t p, uint8_t f, uint8_t t, uint8_t c) {
            fromSq = f; toSq = t; piece = p; capture = c;
            priority = captureVal[c] - captureVal[p];
        }
        Move(uint8_t p, uint8_t f, uint8_t t) {
            fromSq = f; toSq = t; piece = p; capture = 12;
            priority = quietVal[p+4];
        }
        Move(bool isCheckmate) {
            fromSq = 0; toSq = 0;
            capture = 0; piece = isCheckmate? 69: 70;
            priority = 0;
        }

        uint8_t getCapture() { return capture; }
        uint8_t getPiece() { return piece; }
        uint8_t getFromSq() { return fromSq; }
        uint8_t getToSq() { return toSq; }
        int8_t getPriority() { return priority; }
        uint64_t getFromVal() { return 1ULL << fromSq; }
        uint64_t getToVal() { return 1ULL << toSq; }
};