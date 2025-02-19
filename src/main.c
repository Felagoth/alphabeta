#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "types.h"
#include "alphabeta.h"
#include "chess_logic.h"

void print_board(BoardState *board_s)
{
    Piece(*board)[8] = board_s->board;
    for (int i = 7; i >= 0; i--)
    {
        printf("%d ", i + 1);
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
    printf("  a b c d e f g h\n");
}

void print_move(Move move)
{
    char start_col = 'a' + move.init_co.y;
    char start_row = '1' + (move.init_co.x);
    char end_col = 'a' + move.dest_co.y;
    char end_row = '1' + (move.dest_co.x);
    printf("%c%c -> %c%c\n", start_col, start_row, end_col, end_row);
}

int main()
{
    PositionList *board_history = empty_list();
    BoardState *board_s = init_board();
    board_history = save_position(board_s, board_history);
    Move move = empty_move();
    char color = 'w';
    print_board(board_s);

    for (int i = 0; i < 300; i++)
    {
        /*
        // ask for ENTER key
        printf("Press [Enter] key to continue.\n");
        fflush(stdin); // option ONE to clean stdin
        getchar();     // wait for ENTER
        */
        if (color == 'w')
        {
            move = iterative_deepening(board_history, color, 4);
        }
        else
        {
            move = iterative_deepening(board_history, color, 4);
        }
        board_s = move_piece(board_s, move);
        board_history = save_position(board_s, board_history);

        color = color == 'w' ? 'b' : 'w';
        print_board(board_s);
        print_move(move);
        if (is_check(board_s, color))
        {
            printf("check\n");
        }
        if (is_mate(board_s, color))
        {
            if (is_check(board_s, color))
            {
                printf("checkmate\n");
            }
            else
            {
                printf("stalemate\n");
            }
            break;
        }
    }
    free(board_s);

    return 0;
}