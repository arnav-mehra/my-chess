#include <iostream>
unsigned long long n = 29ULL;
// king: 1441151880758558720ULL
// rook: 648518346341351424ULL

int main() {
    // for (int i = 0; i < 64; i++) {
    //     int a = n & 1;
    //     std::cout << a << ' ';
    //     if (i % 8 == 7) std::cout << '\n';
    //     n >>= 1;
    // }
    for (int i = 0; i < 10; i ++) {
        std::cout << (rand() ^ rand()) % 3 << ' ';
    }
}