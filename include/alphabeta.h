#ifndef ALPHABETA_H
#define ALPHABETA_H

#include "chess_logic.h"
#include "types.h"
#include "eval.h"

MoveList *possible_moves(BoardState *board_s, char color);
Move iterative_deepening(PositionList *board_history, char color, int max_depth);

#endif