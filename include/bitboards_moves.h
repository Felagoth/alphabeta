#ifndef BITBOARDS_MOVES_H

#include "types.h"

Bitboard init_white();
Bitboard init_black();
Bitboard init_white_pawns();
Bitboard init_black_pawns();
Bitboard init_white_knights();
Bitboard init_black_knights();
Bitboard init_white_bishops();
Bitboard init_black_bishops();
Bitboard init_white_rooks();
Bitboard init_black_rooks();
Bitboard init_white_queens();
Bitboard init_black_queens();
Bitboard init_white_kings();
Bitboard init_black_kings();

MoveList *possible_moves_bb(BoardState *board_s);

#endif