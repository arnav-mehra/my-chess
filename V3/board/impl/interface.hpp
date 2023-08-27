#pragma once

#include "../Board.hpp"
#include "../../util/conversion.hpp"
#include "../../util/assertion.hpp"
#include "../../search/impl/index.hpp"
#include "../../search/DrawTable.hpp"
#include "context.hpp"

#include <cstdio>
#include <iostream>
#include <string>
#include <stack>

#define MAX_FEN_BOARD_LENGTH 90
#define MAX_FEN_CASTLING_LENGTH 4
#define MAX_FEN_EN_PASSANT_LENGTH 2

Context Board::from_fen(std::string fen_str, bool& turn) {
    TranspositionTable::clear_cells();
    KillerTable::clear_cells();

    if (fen_str.compare("startpos") == 0) {
        fen_str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    }

    // Dissect String

    char fen_board[MAX_FEN_BOARD_LENGTH + 1];
    char castling[MAX_FEN_CASTLING_LENGTH + 1];
    char en_passant[MAX_FEN_EN_PASSANT_LENGTH + 1];
    char turn_char;
    int halfmove, fullmove;

    std::sscanf(
        fen_str.c_str(),
        "%s %c %s %s %d %d",
        fen_board, &turn_char,
        castling, en_passant,
        &halfmove, &fullmove
    );

    turn = turn_char == 'w';

    // std::cout << "IN: " << fen_str << '\n'
    //           << "FB: " << fen_board << '\n'
    //           << "TN: " << turn << '\n'
    //           << "CL: " << castling << '\n'
    //           << "EN: " << en_passant << '\n'
    //           << "HM: " << halfmove << '\n'
    //           << "FM: " << fullmove << '\n';

    // Init board & bitboards

    for (int i = 0; i < 64; i++) this->set_board(i, Piece::NA);
    for (U64 &x : bitboards) x = 0ULL;

    // Set board & bitboards

    int i = 0;
    for (char ch : fen_board) {
        if (ch == '\0') break;
        else if ('1' <= ch && ch <= '9') {
            i += ch - '0';
        } else if (ch != '/') {
            Piece pc = char_to_piece(ch); 
            this->bitboards[(int) pc] |= 1ULL << i;
            this->set_board(i++, pc);
        }
    }
    
    bitboards[(int)Piece::WHITE_ALL] = (
          bitboards[(int)Piece::WHITE_PAWN]
        | bitboards[(int)Piece::WHITE_KNIGHT]
        | bitboards[(int)Piece::WHITE_BISHOP]
        | bitboards[(int)Piece::WHITE_ROOK]
        | bitboards[(int)Piece::WHITE_QUEEN]
        | bitboards[(int)Piece::WHITE_KING]
    );
    bitboards[(int)Piece::BLACK_ALL] = (
          bitboards[(int)Piece::BLACK_PAWN]
        | bitboards[(int)Piece::BLACK_KNIGHT]
        | bitboards[(int)Piece::BLACK_BISHOP]
        | bitboards[(int)Piece::BLACK_ROOK]
        | bitboards[(int)Piece::BLACK_QUEEN]
        | bitboards[(int)Piece::BLACK_KING]
    );

    // Init context for castling rights + zobrist hashing.

    std::string c = std::string(castling);
    auto not_found = std::string::npos;
    bool castling_rights[4] = {
        c.find("K") != not_found,
        c.find("Q") != not_found,
        c.find("k") != not_found,
        c.find("q") != not_found
    };

    Context ctx = Context(this, turn == 'w', castling_rights);

    // Set en passant

    if (en_passant[0] == '-') {
        ctx.en_passant = 0;
    } else {
        ctx.en_passant = string_to_square_num(en_passant[0], en_passant[1]);
    }

    // clear/reset draw table with new context

    DrawTable::clear(ctx);

    return ctx;
}

void Board::print() {
    for (int i = 0; i < 64; i++) {
        std::cout << piece_to_char(this->get_board(i)) << ' ';
        if (i % 8 == 7) std::cout << "\033[1;32m" << (8 - i / 8) << "\033[0m" << '\n';
    }
    std::cout << "\033[1;32m" << "a b c d e f g h" << "\033[0m" << '\n';
}

template<class Color>
Flag Board::derive_flag(Square from, Square to) {
    Piece piece = this->get_board(from);

    bool is_capture = this->get_board(to) != Piece::NA;
    bool is_promo = (piece == (Piece)Color::PAWN)
                    && ((1ULL << to) & Color::FINAL_RANK);
    if (is_capture && is_promo) return Flag::QUEEN_PROMO;
    if (is_capture) return Flag::REGULAR;
    if (is_promo) return Flag::QUEEN_PROMO;

    // bool is_en_passant = (piece == (Piece)Color::PAWN)
    //                      && (to == this->en_passant);
    // if (is_en_passant) return Flag::EN_PASSANT;

    bool is_short_castle = (piece == (Piece)Color::KING)
                           && (to - from == 2);
    if (is_short_castle) return Flag::CASTLE;

    bool is_long_castle = (piece == (Piece)Color::KING)
                          && (from - to == 2);
    if (is_long_castle) return Flag::CASTLE;

    return Flag::REGULAR;
}

void play_moves(
    std::string moves,
    Board& b,
    Context& ctx,
    bool& turn
) {
    while (true) {
        size_t i = moves.find(' ');
        std::string s = moves.substr(
            0, i == std::string::npos ? moves.size() : i
        );

        MoveList ml = MoveList();
        if (turn) b.gen_order_moves<White, Gen::PSEUDOS>(ml, ctx);
             else b.gen_order_moves<Black, Gen::PSEUDOS>(ml, ctx);

        U8 from = string_to_square_num(s[0], s[1]);
        U8 to   = string_to_square_num(s[2], s[3]);
        
        Move move;
        for (int i = 0; i < ml.size(); i++) {
            if (ml[i].get_from() == from && ml[i].get_to() == to) {
                if (s.size() == 5) { // if promo
                    if (ml[i].get_flag() == char_to_promo_flag(s[4])) {
                        move = ml[i];
                        break;
                    }
                } else { // if not promo
                    move = ml[i];
                    break;
                }
            }
        }
        if (move.get_raw() == 0) {
            std::cout << "invalid move: " << s << " - " << (int)from << ' ' << (int)to << '\n';
            ml.print();
            exit(1);
        }

        if (turn) ctx = b.do_move<White>(move, ctx);
             else ctx = b.do_move<Black>(move, ctx);
        turn = !turn;
        b.assert_board_consistency();

        if (i == std::string::npos) break;
        moves = moves.substr(i + 1);
    }
}

void play_best_move(
    Board& b,
    Context& ctx,
    bool& turn,
    Move& best_move
) {
    std::cout << "bestmove " << best_move.to_string() << "\n";
    if (turn) ctx = b.do_move<White>(best_move, ctx);
         else ctx = b.do_move<Black>(best_move, ctx);
    turn = !turn;
}

void CLI() {
    Board b;
    Context ctx;
    bool turn = true;
    Move best_move;

    while (true) {
        std::string s; std::cin >> s;

        if (s.compare("uci") == 0) {
            std::cout << "id name " << "MyChess" << "\n";
            std::cout << "id author " << "Arnav" << "\n";
            std::cout << "uciok\n";
            continue;
        }
        if (s.compare("debug") == 0) {
            std::string ln; std::getline(std::cin, ln); // eat args
            continue; // NOT HANDLED/IGNOREABLE
        }
        if (s.compare("isready") == 0) {
            std::cout << "readyok\n";
            continue;
        }
        if (s.compare("setoption") == 0) {
            std::string ln; std::getline(std::cin, ln); // eat args
            continue; // NOT HANDLED
        }
        if (s.compare("register") == 0) {
            std::string ln; std::getline(std::cin, ln); // eat args
            continue; // NOT HANDLED
        }
        if (s.compare("ucinewgame") == 0) {
            ctx = b.from_fen("startpos", turn);
            continue; // NOT HANDLED/IGNOREABLE
        }
        if (s.compare("position") == 0) {
            std::string pos; std::cin >> pos;
            std::string ln; std::getline(std::cin, ln);
            size_t i = ln.find("moves");

            if (pos.compare("startpos") == 0) {
                ctx = b.from_fen(pos, turn);
            }
            else if (pos.compare("fen") == 0) {
                std::string fen = ln;
                if (i != std::string::npos) {
                    fen = ln.substr(0, i - 1);
                }
                ctx = b.from_fen(fen, turn);
            }
            else {
                std::cout << "invalid position arg";
                exit(1);
            }

            if (i != std::string::npos) {
                std::string moves = ln.substr(i + 6);
                play_moves(moves, b, ctx, turn);
            }
            continue;
        }
        if (s.compare("go") == 0) {
            std::string type; std::cin >> type;
            std::string ln; std::getline(std::cin, ln); // eat args
            
            auto [ move, score ] = turn ? Search::search<White>(b, ctx, MAX_DEPTH)
                                        : Search::search<Black>(b, ctx, MAX_DEPTH);
            best_move = move;

            if (type.compare("infinite") == 0) {
                continue; // wait for stop cmd
            }
            else {
                play_best_move(b, ctx, turn, best_move);
            }
        }
        if (s.compare("stop") == 0) {
            play_best_move(b, ctx, turn, best_move);
            continue;
        }
        if (s.compare("ponderhit") == 0) {
            continue; // NOT HANDLED
        }
        if (s.compare("quit") == 0) {
            exit(0);
        }
        if (s.compare("print") == 0) {
            std::string type; std::cin >> type;
            if (type.compare("board") == 0) {
                b.print();
            }
            if (type.compare("turn") == 0) {
                std::cout << turn << '\n';
            }
            if (type.compare("ctx") == 0) {
                ctx.print();
            }
            if (type.compare("movelist") == 0) {
                MoveList ml;
                if (turn) b.gen_order_moves<White, Gen::PSEUDOS>(ml, ctx);
                     else b.gen_order_moves<Black, Gen::PSEUDOS>(ml, ctx);
                
                ml.print();
            }
            if (type.compare("drawtable") == 0) {
                DrawTable::print();
            }
            continue;
        }
    }
}