#include "chess_logic.h"
#include "types.h"
#include "eval.h"

Move iterative_deepening(PositionList *board_history, char color, int max_depth);