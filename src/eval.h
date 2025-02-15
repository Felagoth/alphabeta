#ifndef EVAL_H
#define EVAL_H

#include <stdlib.h>
#include "types.h"

int *eval_game_ended(PositionList *board_history);
int eval(PositionList *board_history);

#endif