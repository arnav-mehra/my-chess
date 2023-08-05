#pragma once

#include "../board/impl/index.hpp"
#include "../move/MoveList.hpp"
#include "../util/conversion.hpp"
#include <unordered_map>
#include <fstream>
#include <string>

namespace Perft {
    struct Stats {
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
        Stats &stats,
        std::unordered_map<std::string, int> &solution_hist,
        bool CMP_TO_SOLUTION
    ) {
        std::unordered_map<std::string, int> my_hist;

        for (int i = 0; i < 64 * 64 * 8; i++) {
            int cnt = stats.move_hist[i];
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
    U64 _run(Board &b, int depth, Stats &stats) {
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

        MoveList& ml = stats.ml[depth];
        ml.clear(); b.gen_moves<Color, Gen::PSEUDOS>(ml);

        U64 cnt = 0;
        for (int i = 0; i < ml.size(); i++) {
            b.do_move<Color>(ml[i]);
            if (b.get_checks<Color>() == 0ULL) { // filter out illegal moves
                // stats.flag_hist[(int)ml[i].get_flag()]++;
                int res = turn ? _run<Black>(b, depth - 1, stats)
                               : _run<White>(b, depth - 1, stats);
                if (depth == stats.init_depth) stats.add_move(ml[i], res);
                cnt += res;
            }
            b.undo_move<Color>(ml[i]);
        }

        stats.checkmates += cnt == 0; // no legal moves == checkmate.
        return cnt;
    }

    template<class Color>
    void run(Board &b, int depth, std::string solution_file = "") {
        bool CMP_TO_SOLUTION = solution_file.size() != 0;
        std::unordered_map<std::string, int> solution_hist;
        read_solution_file(solution_file, solution_hist, CMP_TO_SOLUTION);

        Stats stats; stats.init_depth = depth;

        auto start = std::chrono::system_clock::now();
        U64 res = _run<Color>(b, depth, stats);
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> t_sec = end - start;

        // std::cout << "PERFT " << depth << ": " << res << "\n\n";

        std::cout << t_sec.count() << " s\n";
        std::cout << (res / t_sec.count()) << " n/s\n\n";

        // std::cout << "CHECKS: " << stats.checks << '\n'; 
        // std::cout << "CHECKMATE: " << stats.checkmates << "\n\n";
        // for (int i = 0; i < 14; i++) {
        //     std::cout << FLAG_NAMES[i] << ": " << stats.flag_hist[i] << '\n'; 
        // } std::cout << '\n';

        print_hist_output(stats, solution_hist, CMP_TO_SOLUTION);

        std::cout << "\nNodes searched: " << res << '\n';    
    }
};