#pragma once

class Move { 
public:
    U32 bitField;

    inline Move();
    inline Move(MoveType::Piece p, MoveCapture::Piece c, U32 f, U32 t);
    inline Move(MoveType::Piece p, U32 f, U32 t);
    inline Move(bool isCheckmate);

    inline MoveType::Piece getPiece();
    inline MoveCapture::Piece getCapture();
    inline U32 getFromSq();
    inline U32 getToSq();
    inline U32 getPriority();

    inline U64 getFromVal();
    inline U64 getToVal();
};