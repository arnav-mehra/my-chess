#pragma once

#include "../util/types.hpp"
#include "../move/MoveList.hpp"
#include "impl/temp.hpp"
#include <string>
#include <cstdio>
#include <array>

class Board {
    std::array<U64, NUM_BITBOARDS> bitboards; // [Piece]
    BoardArr board;

    // state info
    std::array<int, NUM_SQUARES> from_hist = {}; // used for castling rights. Only incremented for quiet, castle, & capture.
    Square en_passant;

public:    
    Board() {}

    // test.hpp

    void assert_board_consistency();
    bool operator==(Board &b);

    // interface.hpp

    void from_fen(std::string fen_str);
    void print();
    template<class> Flag derive_flag(Square from, Square to);
    
    // do_move.hpp && undo_move.hpp

    template<class> void do_move(Move& m);
    template<class> void undo_move(Move& m);

    // gen_moves.hpp

    template<class Color, Gen Gn> void gen_moves(MoveList &moves);
    
    // other.hpp
    
    template<class> U64 get_checks();

    // getters + setters

    template<typename Sq>
    Piece get_board(Sq sq) {
        return board.get((int)sq);
    }

    template<typename Sq, typename Pc>
    void set_board(Sq sq, Pc pc) {
        board.set((int)sq, (Piece)pc);
    }

    template<typename Pc>
    U64& get_bitboard(Pc pc) {
        return this->bitboards[(int)pc];
    }

    template<typename Sq>
    int& get_from_cnt(Sq sq) {
        return this->from_hist[(int)sq];
    }

    int get_en_passant() {
        return this->en_passant;
    }

private:

    // gen_moves.hpp
    
    template<class, Gen> void gen_pawn_moves(MoveList&, U64 filter);
    template<class, Piece> U64 gen_piece_attacks(Square from_sq);
    template<class, Piece> void _gen_piece_moves(MoveList&, U64 filter, Square from_sq);
    template<class, Piece> void gen_piece_moves(MoveList&, U64 filter);
    template<class, class> U64 sliding_castle_checks();
    template<class, class> void gen_castle(MoveList &moves);

    template<class Color>
    void remove_piece(Square sq, Piece pc, Piece pc_col_all);

    template<class Color>
    void add_piece(Square sq, Piece pc, Piece pc_col_all);

    // do_move.hpp

    template<class> void do_quiet(Move&);
    template<class> void do_pawn_double(Move&);
    template<class> void do_en_passant(Move&); // en passant is forced.
    template<class> void do_capture(Move&);
    template<class, class> void do_castle();
    template<class> void do_promo(Move&);
    template<class> void do_promo_capture(Move&);

    // undo_move.hpp

    template<class> void undo_quiet(Move&);
    template<class> void undo_pawn_double(Move&);
    template<class> void undo_en_passant(Move&);
    template<class> void undo_capture(Move&);
    template<class, class> void undo_castle();
    template<class> void undo_promo(Move&);
    template<class> void undo_promo_capture(Move&);

    // other.hpp

    template<class> U64 get_opp_attacks();
    template<class> U64 get_check_blocks(U64 checks);
    template<class> U64 get_pins();
};