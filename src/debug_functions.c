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

bool is_in_move_list(MoveList *move_list, Move move)
{
    for (int i = 0; i < move_list->size; i++)
    {
        if (move_list->moves[i].init_co.x == move.init_co.x &&
            move_list->moves[i].init_co.y == move.init_co.y &&
            move_list->moves[i].dest_co.x == move.dest_co.x &&
            move_list->moves[i].dest_co.y == move.dest_co.y &&
            move_list->moves[i].promotion == move.promotion)
        {
            return true;
        }
    }
    return false;
}

bool are_same_move_set(MoveList *move_list, MoveList *move_list_bb)
{
    if (move_list->size != move_list_bb->size)
    {
        return false;
    }
    for (int i = 0; i < move_list->size; i++)
    {
        if (!is_in_move_list(move_list_bb, move_list->moves[i]))
        {
            return false;
        }
    }
    return true;
}

void print_differences(MoveList *move_list, MoveList *move_list_bb)
{
    if (move_list->size > move_list_bb->size)
    {
        printf("move_list has more moves\n");
        for (int i = 0; i < move_list->size; i++)
        {
            if (!is_in_move_list(move_list_bb, move_list->moves[i]))
            {
                Move move = move_list->moves[i];
                printf("init: %d %d, dest: %d %d\n", move.init_co.x, move.init_co.y, move.dest_co.x, move.dest_co.y);
            }
        }
    }
    else
    {
        printf("move_list_bb has more or same number of moves\n");
        for (int i = 0; i < move_list_bb->size; i++)
        {
            if (!is_in_move_list(move_list, move_list_bb->moves[i]))
            {
                Move move = move_list_bb->moves[i];
                printf("init: %d %d, dest: %d %d\n", move.init_co.x, move.init_co.y, move.dest_co.x, move.dest_co.y);
            }
        }
    }
}

void verify_and_print_differences(MoveList *move_list, MoveList *move_list_bb, PositionList *board_history, char color)
{
    if (!are_same_move_set(move_list, move_list_bb))
    {
        printf("\n\nplayer: %c\n", color);
        printf("move_list size: %d\n", move_list->size);
        print_bitboard(get_targetbb_move_list(move_list));
        // print_move_list(move_list);
        printf("move_list1 size: %d\n", move_list_bb->size);
        print_bitboard(get_targetbb_move_list(move_list_bb));
        // print_move_list(move_list1);
        print_differences(move_list, move_list_bb);
        // print_bitboard(get_targetbb_move_list(move_list) ^ get_targetbb_move_list(move_list1));
        print_board_debug(board_history->board_s);
    }
}