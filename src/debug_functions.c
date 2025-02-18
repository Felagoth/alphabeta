#include <stdio.h>
#include "types.h"
#include "chess_logic.h"

void print_bitboard(Bitboard b)
{
    for (int i = 63; i >= 0; i--)
    {
        printf("%d", (int)(b >> i) & 1);
        if (i % 8 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}

void print_move_list(MoveList *move_list)
{
    printf("size: %d\n", move_list->size);
    for (int i = 0; i < move_list->size; i++)
    {
        Move move = move_list->moves[i];
        printf("init: %d %d, dest: %d %d\n", move.init_co.x, move.init_co.y, move.dest_co.x, move.dest_co.y);
    }
}

void print_board_debug(BoardState *board_s)
{
    Piece(*board)[8] = board_s->board;
    for (int i = 7; i >= 0; i--)
    {
        printf("%d ", i);
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j].color == 'w')
            {
                printf("%c ", board[i][j].name);
            }
            else if (board[i][j].color == 'b')
            {
                printf("%c ", board[i][j].name + 32);
            }
            else
            {
                printf("  ");
            }
        }
        printf("\n");
    }
    printf("  0 1 2 3 4 5 6 7\n");
}

Bitboard get_targetbb_move_list(MoveList *move_list)
{
    Bitboard targetbb = 0ULL;
    for (int i = 0; i < move_list->size; i++)
    {
        Move move = move_list->moves[i];
        targetbb |= 1ULL << coords_to_square(move.dest_co);
        // printf("init: %d %d, dest: %d %d\n", move.init_co.x, move.init_co.y, move.dest_co.x, move.dest_co.y);
    }
    return targetbb;
}