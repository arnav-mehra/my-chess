#pragma once

#include "../util/types.hpp"
#include "../move/MoveList.hpp"
#include "impl/temp.hpp"
#include "Context.hpp"
#include <string>
#include <cstdio>
#include <array>

class Board {
    std::array<U64, NUM_BITBOARDS> bitboards; // [Piece]
    BoardArr board;

public:
    Board() {}

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

    U64 get_occ() {
        return this->get_bitboard(Piece::BLACK_ALL)
             | this->get_bitboard(Piece::WHITE_ALL);
    }

    U64 get_unocc() {
        return ~this->get_occ();
    }

    // test.hpp

    void assert_board_consistency();
    bool operator==(Board &b);

    // interface.hpp

    Context from_fen(std::string fen_str, bool& turn);
    void print();
    template<class> Flag derive_flag(Square from, Square to);

    // gen_moves.hpp

    template<class Color, GenType> void gen_moves(MoveList&, Context&);
    template<class Color, GenType> void gen_order_moves(
        MoveList&,
        Context&,
        Move = Move(),
        U16 depth = 0
    );

    // do_move.hpp && undo_move.hpp

    template<class Color> Context do_move(Move&, Context&);
    template<class Color> void undo_move(Move&);

    // other.hpp

    template<class> U64 get_checks();

private:

    // gen_moves.hpp

    template<class, GenType>   void gen_pawn_moves(MoveList&, Context&, U64 filter);
    template<class, Piece> U64  gen_piece_attacks(Square from_sq);
    template<class, Piece> void _gen_piece_moves(MoveList&, U64 filter, Square from_sq);
    template<class, Piece> void gen_piece_moves(MoveList&, U64 filter);
    template<class Color, class Castle> U64 castling_checks();
    template<class Color, class Castle> void gen_castle(MoveList&, Context&);

    // do_move.hpp

    template<class Color> void remove_piece(Context&, Square, Piece, Piece col_all);
    template<class Color> void remove_piece(Square, Piece, Piece col_all);

    template<class Color> void add_piece(Context&, Square, Piece, Piece col_all);
    template<class Color> void add_piece(Square, Piece, Piece col_all);

    template<class Color> void move_piece(Context&, Piece pc, Square from, Square to);
    template<class Color> void move_piece(Piece pc, Square from, Square to);

    template<class Color>  void do_regular   (Context& ctx, Piece pc, Piece capt, Square from, Square to);
    template<class Color>  void do_en_passant(Context&, Square from, Square to);
    template<class Color>  void do_promo     (Context& ctx, Flag flag, Piece capt, Square from, Square to);
    template<class Color, class Castle> void do_castle(Context&);

    // undo_move.hpp

    template<class Color> void undo_regular   (Piece pc, Piece capt, Square from, Square to);
    template<class Color> void undo_en_passant(Square from, Square to);
    template<class Color> void undo_promo     (Flag, Piece capt, Square from, Square to);
    template<class Color, class Castle> void undo_castle();

    // other.hpp

    template<class Color> U64 get_opp_attacks();
    template<class Color> U64 get_check_blocks(U64 checks);
    template<class Color> U64 get_pins();
};