using namespace std;


int16_t captureVal[6] = {100, 250, 300, 500, 900, 10000};
int16_t quietVal[12] = {
    90, -10, -10, -10, //promo
    2, 3, 3, 0, 1, -20, //quiet
    7, 5 //castle
};


class Move { 

    public:

        uint8_t fromSq, toSq;
        int8_t capture, piece;
        uint64_t fromVal, toVal;
        int64_t deltaVal;
        int16_t priority;


        Move(int8_t p, uint8_t f, uint8_t t, int8_t c) {
            fromSq = f, toSq = t, piece = p, capture = c;
            fromVal = 1ULL << f, toVal = 1ULL << t;
            deltaVal = toVal - fromVal;
            priority = (
                c == -1? quietVal[p+4] : captureVal[c] - captureVal[p]
            );
        }
        Move(bool isCheckmate) {
            fromSq = 0, toSq = 0;
            fromVal = 0, toVal = 0, deltaVal = 0;
            capture = 0; 
            priority = 0;
            piece = isCheckmate? 69 : 70;
        }

};