#include "../src/types.h"

typedef struct simple_move_list
{
    Move moves[MAX_MOVES];
    int count;
} MoveList;

typedef struct
{
    int depth; // depth of the search in plies
    int nodes; // number of nodes searched
    int searchMoves;
    MoveList searchable;

    long start;
    int alloc;
    int max;

    int hitrate;

    int timeset;
    int mate;
    int movesToGo;
    int stopped;
    int quit;
    int multiPV;
    int infinite;
} SearchParams;