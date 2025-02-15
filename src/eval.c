#include "eval.h"
#include "types.h"
#include "chess_logic.h"
#include <stdlib.h>

extern int MAX_SCORE;

const int KING_SQUARE_TABLE[8][8] = {
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-20, -30, -30, -40, -40, -30, -30, -20},
    {-10, -20, -20, -20, -20, -20, -20, -10},
    {20, 20, 0, 0, 0, 0, 20, 20},
    {20, 30, 10, 0, 0, 10, 30, 20}};

const int PAWN_SQUARE_TABLE[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    {5, 5, 10, 25, 25, 10, 5, 5},
    {0, 0, 0, 20, 20, 0, 0, 0},
    {5, -5, -10, 0, 0, -10, -5, 5},
    {5, 10, 10, -20, -20, 10, 10, 5},
    {0, 0, 0, 0, 0, 0, 0, 0}};

const int PAWN = 100;
const int KNIGHT = 320;
const int BISHOP = 330;
const int ROOK = 500;
const int QUEEN = 900;
const int KING = 20000;

int piece_eval(char piece_name, int x, int y)
{
    switch (piece_name)
    {
    case 'P':
        return PAWN + PAWN_SQUARE_TABLE[x][y];
    case 'N':
        return KNIGHT;
    case 'B':
        return BISHOP;
    case 'R':
        return ROOK;
    case 'Q':
        return QUEEN;
    case 'K':
        return KING + KING_SQUARE_TABLE[x][y];
    default:
        return 0;
    }
}

void print_board2(BoardState *board_s)
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

void print_board_history(PositionList *board_history)
{
    PositionList *pos_l = board_history;
    while (pos_l != NULL)
    {
        print_board2(pos_l->board_s);
        pos_l = pos_l->tail;
    }
    printf("\n\n\n\n");
}

int *eval_game_ended(PositionList *board_history)
{
    BoardState *board_s = board_history->board_s;
    int *result = malloc(sizeof(int));
    if (result == NULL)
    {
        return NULL;
    }

    // check for mates
    bool white_mate = is_mate(board_s, 'w');
    bool black_mate = is_mate(board_s, 'b');
    if (white_mate && is_check(board_s, 'w'))
    {
        *result = -MAX_SCORE;
    }
    else if (black_mate && is_check(board_s, 'b'))
    {
        *result = MAX_SCORE;
    }
    else if (white_mate || black_mate)
    {
        *result = 0;
    }
    else if (threefold_repetition(board_s, board_history, 0))
    {
        // printf("threefold repetition\n");
        *result = 0;
    }
    else if (board_s->fifty_move_rule >= 100)
    {
        *result = 0;
    }
    else
    {
        free(result);
        return NULL;
    }
    return result;
}

// evaluate the board state for the white player
// return the score of the board state
int eval(PositionList *board_history)
{
    BoardState *board_s = board_history->board_s;
    int score = 0;
    Piece(*board)[8] = board_s->board;
    // printf("eval\n");
    // evaluate the board
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Piece piece = board[i][j];
            if (piece.color == 'w')
            {
                score += piece_eval(piece.name, i, j);
            }
            else if (piece.color == 'b')
            {
                score -= piece_eval(piece.name, i, j);
            }
        }
    }
    return score;
}