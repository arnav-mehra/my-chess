#pragma once

#include "types.hpp"
#include "data.hpp"

#include <iostream>

Piece char_to_piece(char pc) {
    switch (pc) {
        case 'P': return Piece::WHITE_PAWN;
        case 'N': return Piece::WHITE_KNIGHT;
        case 'B': return Piece::WHITE_BISHOP;
        case 'R': return Piece::WHITE_ROOK;
        case 'Q': return Piece::WHITE_QUEEN;
        case 'K': return Piece::WHITE_KING;

        case 'p': return Piece::BLACK_PAWN;
        case 'n': return Piece::BLACK_KNIGHT;
        case 'b': return Piece::BLACK_BISHOP;
        case 'r': return Piece::BLACK_ROOK;
        case 'q': return Piece::BLACK_QUEEN;
        case 'k': return Piece::BLACK_KING;
    }
    return Piece::GARBAGE;
}

char piece_to_char(Piece pc) {
    switch (pc) {
        case Piece::WHITE_PAWN:   return 'P';
        case Piece::WHITE_KNIGHT: return 'N';
        case Piece::WHITE_BISHOP: return 'B';
        case Piece::WHITE_ROOK:   return 'R';
        case Piece::WHITE_QUEEN:  return 'Q';
        case Piece::WHITE_KING:   return 'K';

        case Piece::BLACK_PAWN:   return 'p';
        case Piece::BLACK_KNIGHT: return 'n';
        case Piece::BLACK_BISHOP: return 'b';
        case Piece::BLACK_ROOK:   return 'r';
        case Piece::BLACK_QUEEN:  return 'q';
        case Piece::BLACK_KING:   return 'k';
        default: return '.';
    }
    return '.';
}

Square string_to_square_num(char file_ch, char rank_ch) {
    Square file = file_ch - 'a';
    Square rank = 7 - (rank_ch - '1');
    Square sq_num = (rank << 3) + file;
    return sq_num;
}

std::string square_num_to_string(Square sq) {
    Square file = sq & 0b111;
    Square rank =  0b111 - (sq >> 3);
    std::string str;
    str += file + 'a';
    str += rank + '1';
    return str;
}

void printBB(U64 bb) {
    for (Square i = 0; i < NUM_SQUARES; i++) {
        std::cout << ((bb >> i) & 0b1) << ' ';
        if (i % 8 == 7) std::cout << '\n';
    }
} 

const std::string INITIAL_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

const std::string FLAG_NAMES[] = {
    "QUIET",
    
    "CAPTURE",
    "EN_PASSANT",

    "PAWN_DOUBLE",

    "SHORT_CASTLE",
    "LONG_CASTLE",

    "KNIGHT_PROMO",
    "BISHOP_PROMO",
    "ROOK_PROMO",
    "QUEEN_PROMO",

    "KNIGHT_PROMO_CAPTURE",
    "BISHOP_PROMO_CAPTURE",
    "ROOK_PROMO_CAPTURE",
    "QUEEN_PROMO_CAPTURE",
};

const std::string PIECE_NAMES[] = { // 4 bits
    "WHITE_PAWN",
    "WHITE_KNIGHT",
    "WHITE_BISHOP",
    "WHITE_ROOK",
    "WHITE_QUEEN",
    "WHITE_KING",

    "BLACK_PAWN",
    "BLACK_KNIGHT",
    "BLACK_BISHOP",
    "BLACK_ROOK",
    "BLACK_QUEEN",
    "BLACK_KING",

    "WHITE_ALL",
    "BLACK_ALL",
    "ALL",
    "GARBAGE",
};