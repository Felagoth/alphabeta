#include "eval.h"
#include "types.h"
#include "chess_logic.h"
#include <stdlib.h>
#include <stdbool.h>

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

const int KNIGHT_SQUARE_TABLE[8][8] = {
    {-50, -40, -30, -30, -30, -30, -40, -50},
    {-40, -20, 0, 0, 0, 0, -20, -40},
    {-30, 0, 10, 15, 15, 10, 0, -30},
    {-30, 5, 15, 20, 20, 15, 5, -30},
    {-30, 0, 15, 20, 20, 15, 0, -30},
    {-30, 5, 10, 15, 15, 10, 5, -30},
    {-40, -20, 0, 5, 5, 0, -20, -40},
    {-50, -40, -30, -30, -30, -30, -40, -50}};

const int BISHOP_SQUARE_TABLE[8][8] = {
    {-20, -10, -10, -10, -10, -10, -10, -20},
    {-10, 0, 0, 0, 0, 0, 0, -10},
    {-10, 0, 5, 10, 10, 5, 0, -10},
    {-10, 5, 5, 10, 10, 5, 5, -10},
    {-10, 0, 10, 10, 10, 10, 0, -10},
    {-10, 10, 10, 10, 10, 10, 10, -10},
    {-10, 5, 0, 0, 0, 0, 5, -10},
    {-20, -10, -10, -10, -10, -10, -10, -20}};

const int ROOK_SQUARE_TABLE[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {5, 10, 10, 10, 10, 10, 10, 5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {0, 0, 0, 5, 5, 0, 0, 0}};

const int QUEEN_SQUARE_TABLE[8][8] = {
    {-20, -10, -10, -5, -5, -10, -10, -20},
    {-10, 0, 0, 0, 0, 0, 0, -10},
    {-10, 0, 5, 5, 5, 5, 0, -10},
    {-5, 0, 5, 5, 5, 5, 0, -5},
    {0, 0, 5, 5, 5, 5, 0, -5},
    {-10, 5, 5, 5, 5, 5, 0, -10},
    {-10, 0, 5, 0, 0, 0, 0, -10},
    {-20, -10, -10, -5, -5, -10, -10, -20}};

const int PAWN_VAL = 100;
const int KNIGHT_VAL = 320;
const int BISHOP_VAL = 330;
const int ROOK_VAL = 500;
const int QUEEN_VAL = 900;
const int KING_VAL = 20000;

int piece_eval(char piece_name, int x, int y, char color)
{
    int x_from_color = color == 'w' ? 7 - x : x;
    switch (piece_name)
    {
    case 'P':
        return PAWN_VAL + PAWN_SQUARE_TABLE[x_from_color][y];
    case 'N':
        return KNIGHT_VAL + KNIGHT_SQUARE_TABLE[x_from_color][y];
    case 'B':
        return BISHOP_VAL + BISHOP_SQUARE_TABLE[x_from_color][y];
    case 'R':
        return ROOK_VAL + ROOK_SQUARE_TABLE[x_from_color][y];
    case 'Q':
        return QUEEN_VAL + QUEEN_SQUARE_TABLE[x_from_color][y];
    case 'K':
        return KING_VAL + KING_SQUARE_TABLE[x_from_color][y];
    default:
        return 0;
    }
}

bool check_for_mates(BoardState *board_s, char color, int *result)
{
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
    else
    {
        return false;
    }
    return true;
}

bool eval_game_ended(PositionList *board_history, int *result)
{
    BoardState *board_s = board_history->board_s;

    // check for mates, not necessary in the way we call this function
    // if (check_for_mates(board_s, 'w', result) || check_for_mates(board_s, 'b', result))
    // {
    //     return true;
    // }
    // else
    if (threefold_repetition(board_s, board_history, 0))
    {
        // printf("threefold repetition\n");
        *result = -100;
    }
    else if (board_s->fifty_move_rule >= 100)
    {
        *result = -50;
    }
    else if (insufficient_material(board_s))
    {
        *result = 0;
    }
    else
    {
        return false;
    }
    return true;
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
                score += piece_eval(piece.name, i, j, 'w');
            }
            else if (piece.color == 'b')
            {
                score -= piece_eval(piece.name, i, j, 'b');
            }
        }
    }
    return score;
}