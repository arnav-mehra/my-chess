#pragma once

#include "../board/impl/index.hpp"
#include "../util/conversion.hpp"
#include <cstdio>
#include <iostream>
#include <string>
#include <stack>

#define MAX_FEN_BOARD_LENGTH 90
#define MAX_FEN_CASTLING_LENGTH 4
#define MAX_FEN_EN_PASSANT_LENGTH 2

void Board::from_fen(std::string fen_str) {
    char fen_board[MAX_FEN_BOARD_LENGTH + 1];
    char castling[MAX_FEN_CASTLING_LENGTH + 1];
    char en_passant[MAX_FEN_EN_PASSANT_LENGTH + 1];
    char turn;
    int halfmove, fullmove;

    std::sscanf(
        fen_str.c_str(),
        "%s %c %s %s %d %d",
        fen_board, &turn,
        castling, en_passant,
        &halfmove, &fullmove
    );
    // std::cout << "IN: " << fen_str << '\n'
    //           << "FB: " << fen_board << '\n'
    //           << "TN: " << turn << '\n'
    //           << "CL: " << castling << '\n'
    //           << "EN: " << en_passant << '\n'
    //           << "HM: " << halfmove << '\n'
    //           << "FM: " << fullmove << '\n';

    // Init board & bitboards
    for (int i = 0; i < 64; i++) board[i] = Piece::GARBAGE;
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
            this->board[i++] = pc;
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
    bitboards[(int)Piece::ALL] = (
          bitboards[(int)Piece::WHITE_ALL]
        | bitboards[(int)Piece::BLACK_ALL]
    );    

    // Set other feilds
    if (en_passant[0] == '-') {
        this->en_passant = 0;
    } else {
        this->en_passant = string_to_square_num(en_passant[0], en_passant[1]);
    }
    // mimic castling rights via rook from sq forgery.
    std::string c = std::string(castling);
    auto not_found = std::string::npos;
    this->from_hist[White::OO::ROOK_PRE]  += c.find("K") == not_found;
    this->from_hist[White::OOO::ROOK_PRE] += c.find("Q") == not_found;
    this->from_hist[Black::OO::ROOK_PRE]  += c.find("k") == not_found;
    this->from_hist[Black::OOO::ROOK_PRE] += c.find("q") == not_found;
}

void Board::print() {
    for (int i = 0; i < 64; i++) {
        std::cout << piece_to_char(this->board[i]) << ' ';
        if (i % 8 == 7) std::cout << "\033[1;32m" << (8 - i / 8) << "\033[0m" << '\n';
    }
    std::cout << "\033[1;32m" << "a b c d e f g h" << "\033[0m" << '\n';
}

template<class Color>
Flag Board::derive_flag(Square from, Square to) {
    Piece piece = board[from];

    bool is_capture = board[to] != Piece::GARBAGE;
    bool is_promo = (piece == (Piece)Color::PAWN)
                    && ((1ULL << to) & Color::FINAL_RANK);
    if (is_capture && is_promo) return Flag::QUEEN_PROMO_CAPTURE;
    if (is_capture) return Flag::CAPTURE;
    if (is_promo) return Flag::QUEEN_PROMO;

    bool is_en_passant = (piece == (Piece)Color::PAWN)
                         && (to == this->en_passant);
    if (is_en_passant) return Flag::EN_PASSANT;

    bool is_short_castle = (piece == (Piece)Color::KING)
                           && (to - from == 2);
    if (is_short_castle) return Flag::SHORT_CASTLE;

    bool is_long_castle = (piece == (Piece)Color::KING)
                          && (from - to == 2);
    if (is_long_castle) return Flag::LONG_CASTLE;

    bool is_double_pawn = (piece == (Piece)Color::PAWN)
                          && ((from - to == 16) || (to - from == 16));
    if (is_double_pawn) return Flag::PAWN_DOUBLE;

    return Flag::QUIET;
}

void CLI(std::string fen_str) {
    Board b; b.from_fen(fen_str);
    // b.assert_board_consistency();

    std::stack<Move> move_hist;
    std::stack<Board> board_hist; board_hist.push(b);
    bool turn = true;

    while (true) {
        std::cout << '\n';
        b.print();

        MoveList ml = MoveList();
        if (turn) b.gen_moves<White, Gen::PSEUDOS>(ml);
             else b.gen_moves<Black, Gen::PSEUDOS>(ml);
        ml.print();

        std::cout << "\nPlay " << (turn ? "White" : "Black") << " Move: ";
        std::string s; std::cin >> s;

        if (s.size() >= 1 && s[0] == 'u') {
            std::cout << "Undoing move...\n";

            Move move = move_hist.top();
            move_hist.pop();
            board_hist.pop();
            if (turn) b.undo_move<Black>(move);
                 else b.undo_move<White>(move);
            turn = !turn;
            
            assert("UNDO FAILED: PRIOR BOARD INEQUIVALENCY", b == board_hist.top());
            success("UNDO SUCCESSFUL!");
            continue;
        }

        if (s.size() != 4) {
            std::cout << "Invalid move.\n";
            continue;
        }

        Square from = string_to_square_num(s[0], s[1]);
        Square to = string_to_square_num(s[2], s[3]);

        std::cout << PIECE_NAMES[(int)b.get_board(from)] << ' ';        
        Flag flag = turn ? b.derive_flag<White>(from, to)
                         : b.derive_flag<Black>(from, to);
        Move move = Move(
            flag,
            b.get_board(from), b.get_board(to),
            from, to
        );
        move.print();

        if (turn) b.do_move<White>(move);
             else b.do_move<Black>(move); 
        turn = !turn;     
        move_hist.push(move);
        board_hist.push(b);
        // b.assert_board_consistency();
    }
}