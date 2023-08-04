#pragma once
#include "../Board.hpp"
#include <iostream>
#include <functional>
#include <chrono>
#include <unordered_map>
#include <fstream>

void success(std::string name) {
    std::cout << "\033[1;32m" << name << "\033[0m" << "\n";
}

void failure(std::string name) {
    std::cout << "\033[1;31m" << name << "\033[0m" << "\n";
}

void assert(std::string name, bool condition) {
    if (!condition) {
        failure("ASSERT FAILED: " + name + ".");
        std::abort();
    }
}

void assert(std::function<void(void)> fn, bool condition) {
    if (!condition) {
        failure("ASSERT FAILED.");
        fn();
        std::abort();
    }
}

void Board::assert_board_consistency() {
    // BITBOARD self-consistency.

    // accidental aggregate miscalculation.
    assert(
        "WHITE_AGGREGATE",
        bitboards[(int)Piece::WHITE_ALL] == (
              bitboards[(int)Piece::WHITE_PAWN]
            | bitboards[(int)Piece::WHITE_KNIGHT]
            | bitboards[(int)Piece::WHITE_BISHOP]
            | bitboards[(int)Piece::WHITE_ROOK]
            | bitboards[(int)Piece::WHITE_QUEEN]
            | bitboards[(int)Piece::WHITE_KING]
        )
    );
    assert(
        "BLACK_AGGREGATE",
        bitboards[(int)Piece::BLACK_ALL] == (
              bitboards[(int)Piece::BLACK_PAWN]
            | bitboards[(int)Piece::BLACK_KNIGHT]
            | bitboards[(int)Piece::BLACK_BISHOP]
            | bitboards[(int)Piece::BLACK_ROOK]
            | bitboards[(int)Piece::BLACK_QUEEN]
            | bitboards[(int)Piece::BLACK_KING]
        )
    );
    assert(
        "FULL_AGGREGATE",
        bitboards[(int)Piece::ALL] == (
              bitboards[(int)Piece::BLACK_ALL]
            | bitboards[(int)Piece::WHITE_ALL]
        )
    );

    // accidental piece non-removal or addition.
    // if piece overlap, additional +1 to pop_count.
    assert(
        "WHITE_EXCLUSIVITY",
        pop_count(bitboards[(int)Piece::WHITE_ALL]) == ( 
              pop_count(bitboards[(int)Piece::WHITE_PAWN])
            + pop_count(bitboards[(int)Piece::WHITE_KNIGHT])
            + pop_count(bitboards[(int)Piece::WHITE_BISHOP])
            + pop_count(bitboards[(int)Piece::WHITE_ROOK])
            + pop_count(bitboards[(int)Piece::WHITE_QUEEN])
            + pop_count(bitboards[(int)Piece::WHITE_KING])
        )
    );
    assert(
        "BLACK_EXCLUSIVITY",
        pop_count(bitboards[(int)Piece::BLACK_ALL]) == ( 
              pop_count(bitboards[(int)Piece::BLACK_PAWN])
            + pop_count(bitboards[(int)Piece::BLACK_KNIGHT])
            + pop_count(bitboards[(int)Piece::BLACK_BISHOP])
            + pop_count(bitboards[(int)Piece::BLACK_ROOK])
            + pop_count(bitboards[(int)Piece::BLACK_QUEEN])
            + pop_count(bitboards[(int)Piece::BLACK_KING])
        )
    );
    assert(
        "COLOR_EXCLUSIVITY",
        0ULL == (
              bitboards[(int)Piece::BLACK_ALL]
            & bitboards[(int)Piece::WHITE_ALL]
        )
    );

    // BOARD vs. BITBOARD consistency.

    // presence
    for (int pc = (int)Piece::WHITE_PAWN; pc <= (int)Piece::BLACK_KING; pc++) {
        U64 cpy = bitboards[pc];
        while (cpy) {
            int sq = pop_lsb(cpy);
            std::string name = PIECE_NAMES[pc] + " PRESENCE AT " + std::to_string(sq);
            assert(name, (int)board[sq] == pc);
        }
    }
    // void
    U64 void_bits = ~bitboards[(int)Piece::ALL];
    while (void_bits) {
        int sq = pop_lsb(void_bits);
        std::string name = "VOID AT " + std::to_string(sq);
        assert(name, board[sq] == Piece::GARBAGE);
    }

    // OTHER

    // accidental piece creation.
    assert(
        "WHITE OVERFLOW",
        pop_count(bitboards[(int)Piece::WHITE_ALL]) <= 16
    );
    assert(
        "BLACK OVERFLOW",
        pop_count(bitboards[(int)Piece::BLACK_ALL]) <= 16
    );
    assert(
        "WHITE PAWN OVERFLOW",
        pop_count(bitboards[(int)Piece::WHITE_PAWN]) <= 8
    );
    assert(
        "BLACK PAWN OVERFLOW",
        pop_count(bitboards[(int)Piece::BLACK_PAWN]) <= 8
    );

    // en passant square correctness.
    // assert(
    //     "EN PASSANT SQUARE OVERLAP",
    //     this->en_passant == 0
    //     || 0ULL == (
    //           (1ULL << this->en_passant)
    //         & bitboards[(int)Piece::ALL]
    //     )
    // );

    // success("ASSERTIONS PASSED!");
}

bool Board::operator==(Board &b) {
    for (int i = 0; i < NUM_SQUARES; i++) {
        if (b.get_board(i) != this->get_board(i)) {
            std::cout << "NOT EQUAL: square " << i << "\n";
            return false;
        }
    }
    for (int i = 0; i < NUM_BITBOARDS; i++) {
        if (b.get_bitboard(i) != this->get_bitboard(i)) {
            std::cout << "NOT EQUAL: bitboard " << PIECE_NAMES[i] << "\n";
            return false;
        }
    }
    for (int i = 0; i < NUM_BITBOARDS; i++) {
        if (b.get_from_cnt(i) != b.get_from_cnt(i)) {
            std::cout << "NOT EQUAL: from_hist " << i << "\n";
            return false;
        }
    }
    return true;
}

struct PerftStats {
    MoveList ml[10];

    int flag_hist[14] = {};
    int move_hist[64 * 64 * 8] = {};
    int checks = 0;
    int checkmates = 0;
    int init_depth;

    void add_move(Move m, int res) {
        int sq = (m.get_raw() >> 4) & 0b111111111111;
        if (m.get_flag() >= Flag::KNIGHT_PROMO) {
            int promo_pc = (((int)m.get_flag() - 6) % 4) + 1;
            sq |= promo_pc << 12;
        }
        this->move_hist[sq] += res;
    }
};

template<class Color>
U64 _perft(Board &b, int depth, PerftStats &ps) {
    constexpr bool turn = std::is_same<Color, White>::value;

    if (depth == 0) {
        // ps.checks += b.is_check<Color>();
        // MoveList ml; b.gen_moves<Color>(ml);
        // int legals = 0;
        // for (int i = 0; i < ml.size(); i++) {
        //     b.do_move<Color>(ml[i]);
        //     legals += !b.is_check<Color>();
        //     b.undo_move<Color>(ml[i]);
        // }
        // ps.checkmates += legals == 0;

        return 1;
    }

    MoveList& ml = ps.ml[depth];
    ml.clear(); b.gen_moves<Color, Gen::PSEUDOS>(ml);

    U64 cnt = 0;
    for (int i = 0; i < ml.size(); i++) {
        b.do_move<Color>(ml[i]);
        if (b.get_checks<Color>() == 0ULL) { // filter out illegal moves
            // ps.flag_hist[(int)ml[i].get_flag()]++;
            int res = turn ? _perft<Black>(b, depth - 1, ps)
                           : _perft<White>(b, depth - 1, ps);
            if (depth == ps.init_depth) ps.add_move(ml[i], res);
            cnt += res;
        }
        b.undo_move<Color>(ml[i]);
    }

    ps.checkmates += cnt == 0; // no legal moves == checkmate.
    return cnt;
}

void read_solution_file(
    std::string fname,
    std::unordered_map<std::string, int> &solution_hist,
    bool CMP_TO_SOLUTION
) {
    if (!CMP_TO_SOLUTION) return;

    std::ifstream infile(fname);
    std::string move; int cnt;
    while (infile >> move >> cnt) {
        solution_hist.insert({move, cnt});
    }
}

void print_hist_output(
    PerftStats &ps,
    std::unordered_map<std::string, int> &solution_hist,
    bool CMP_TO_SOLUTION
) {
    std::unordered_map<std::string, int> my_hist;

    for (int i = 0; i < 64 * 64 * 8; i++) {
        int cnt = ps.move_hist[i];
        if (cnt == 0) continue;

        std::string str = square_num_to_string((i >> 6) & 0b111111)
                        + square_num_to_string(i & 0b111111);
        char piece_char_map[5] = {' ', 'n', 'b', 'r', 'q'};
        if ((i >> 12) != 0) str += piece_char_map[i >> 12];
        str += ":";

        my_hist[str] = cnt;

        if (CMP_TO_SOLUTION) {
            if (solution_hist.find(str) == solution_hist.end()) {
                std::cout << str << " " << cnt << " vs 0\n";
            } else if (solution_hist[str] != cnt) {
                std::cout << str << " " << cnt << " vs " << solution_hist[str] << '\n'; 
            }
        }
    }

    if (CMP_TO_SOLUTION) {
        for (auto& [move, cnt] : solution_hist) {
            if (my_hist.find(move) == my_hist.end()) {
                std::cout << move << " 0 vs " << cnt << "\n";
            }
        }
    }
}

template<class Color>
void perft(
    Board &b,
    int depth,
    std::string solution_file = ""
) {
    bool CMP_TO_SOLUTION = solution_file.size() != 0;
    std::unordered_map<std::string, int> solution_hist;
    read_solution_file(solution_file, solution_hist, CMP_TO_SOLUTION);

    PerftStats ps; ps.init_depth = depth;

    auto start = std::chrono::system_clock::now();
    U64 res = _perft<Color>(b, depth, ps);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> t_sec = end - start;

    // std::cout << "PERFT " << depth << ": " << res << "\n\n";

    std::cout << t_sec.count() << " s\n";
    std::cout << (res / t_sec.count()) << " n/s\n\n";

    // std::cout << "CHECKS: " << ps.checks << '\n'; 
    // std::cout << "CHECKMATE: " << ps.checkmates << "\n\n";
    // for (int i = 0; i < 14; i++) {
    //     std::cout << FLAG_NAMES[i] << ": " << ps.flag_hist[i] << '\n'; 
    // } std::cout << '\n';

    print_hist_output(ps, solution_hist, CMP_TO_SOLUTION);

    std::cout << "\nNodes searched: " << res << '\n';    
}