#ifndef DEBUG_FUNCTIONS_H
#define DEBUG_FUNCTIONS_H

#include <stdio.h>
#include "types.h"

void print_bitboard(Bitboard b);
void print_move_list(MoveList *move_list);
void print_board_debug(BoardState *board_s);
Bitboard get_targetbb_move_list(MoveList *move_list);

#endif