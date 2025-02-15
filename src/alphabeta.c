#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "types.h"
#include "chess_logic.h"
#include "eval.h"

const int MAX_SCORE = 100000;

int alpha_beta_score(PositionList *board_history, char color, int is_max)
{
    if ((is_max == 1 && color == 'w') || (is_max == 0 && color == 'b'))
    {
        return eval(board_history);
    }
    else
    {
        return -eval(board_history);
    }
}

MoveList *possible_moves(BoardState *board_s, char color)
{
    MoveList *move_list = malloc(sizeof(MoveList));
    if (move_list == NULL)
    {
        return NULL;
    }
    move_list->count = 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Piece piece = board_s->board[i][j];
            if (piece.color == color && !is_empty(piece))
            {
                for (int k = 0; k < 8; k++)
                {
                    for (int l = 0; l < 8; l++)
                    {
                        Coords init_co = {i, j};
                        Coords dest_co = {k, l};
                        if (can_move_heuristic(board_s, piece, init_co, dest_co, true))
                        {
                            Move move = {init_co, dest_co, NULL};
                            if (piece.name == 'P' && (dest_co.x == 0 || dest_co.x == 7))
                            {
                                for (int m = 0; m < 4; m++)
                                {
                                    char promotion = "QNBR"[m];
                                    move.promotion = &promotion;
                                    move_list->moves[move_list->count] = move;
                                    move_list->count++;
                                }
                            }
                            else
                            {
                                move.promotion = NULL;
                                move_list->moves[move_list->count] = move;
                                move_list->count++;
                            }
                        }
                    }
                }
            }
        }
    }
    return move_list;
}

typedef struct
{
    Move move;
    int score;
} MoveScore;

// do an alpha beta search

// alpha is the best score that the maximizing player can guarantee
// beta is the best score that the minimizing player can guarantee
// depth is the depth of the search
// max_depth is the maximum depth of the search
// board_s is the current board state
// color is the color of the player to move
// tested_move is the move to make
// is_max is true if the current player is the maximizing player
// is_min is true if the current player is the minimizing player
// return the score of the best move

MoveScore alphabeta(int alpha, int beta, int depth, int max_depth, PositionList *board_history, char color, Move tested_move, int is_max, int is_min)
{
    MoveScore result;
    result.move = tested_move;
    if (depth == max_depth)
    {
        result.score = alpha_beta_score(board_history, color, is_max);
        if (result.score != 0)
        {
            // printf("depth: %d, move: %c%c -> %c%c, score: %d\n", depth, 'a' + tested_move.init_co.y, '1' + tested_move.init_co.x, 'a' + tested_move.dest_co.y, '1' + tested_move.dest_co.x, result.score);
        }
        return result;
    }
    MoveList *move_list = possible_moves(board_history->board_s, color);
    if (move_list == NULL)
    {
        if (is_check(board_history->board_s, color))
        {
            result.score = -MAX_SCORE;
            return result;
        }
        result.score = 0;
        return result;
    }
    BoardState *new_board_s = malloc(sizeof(BoardState));
    PositionList *new_board_history = malloc(sizeof(PositionList));
    new_board_history->tail = board_history;
    if (new_board_s == NULL || new_board_history == NULL)
    {
        result.score = 0;
        result.move = empty_move();
        return result;
    }
    char next_color = color == 'w' ? 'b' : 'w';
    if (is_max)
    {
        result.score = -MAX_SCORE;
        for (int i = 0; i < move_list->count; i++)
        {
            Move new_move = move_list->moves[i];
            *new_board_s = *board_history->board_s;
            new_board_s = move_piece(new_board_s, new_move);
            new_board_history->board_s = new_board_s;
            int new_score;
            int *eval_game_ended_res = eval_game_ended(new_board_history);
            if (eval_game_ended_res != NULL)
            {
                new_score = *eval_game_ended_res;
                free(eval_game_ended_res);
            }
            else
            {
                MoveScore new_move_score = alphabeta(alpha, beta, depth + 1, max_depth, new_board_history, next_color, new_move, 0, 1);
                new_score = new_move_score.score;
            }
            if (new_score > result.score)
            {
                result.move = new_move;
                result.score = new_score;
            }
            if (result.score > alpha)
            {
                alpha = result.score;
            }
            if (alpha >= beta)
            {
                break;
            }
        }
    }
    else
    {
        result.score = MAX_SCORE;
        for (int i = 0; i < move_list->count; i++)
        {
            Move new_move = move_list->moves[i];
            *new_board_s = *board_history->board_s;
            new_board_s = move_piece(new_board_s, new_move);
            new_board_history->board_s = new_board_s;
            int new_score;
            int *eval_game_ended_res = eval_game_ended(new_board_history);
            if (eval_game_ended_res != NULL)
            {
                new_score = *eval_game_ended_res;
                free(eval_game_ended_res);
            }
            else
            {
                MoveScore new_move_score = alphabeta(alpha, beta, depth + 1, max_depth, new_board_history, next_color, new_move, 1, 0);
                new_score = new_move_score.score;
            }
            if (new_score < result.score)
            {
                result.score = new_score;
                result.move = new_move;
            }
            if (result.score < beta)
            {
                beta = result.score;
            }
            if (alpha >= beta)
            {
                break;
            }
        }
    }
    if (move_list == NULL)
    {
        printf("move_list is NULL\n");
    }
    else
    {
        free(move_list);
    }
    if (new_board_s == NULL)
    {
        printf("new_board_s is NULL\n");
    }
    else
    {
        free(new_board_s);
    }
    if (new_board_history == NULL)
    {
        printf("new_board_history is NULL\n");
    }
    else
    {
        free(new_board_history);
    }
    return result;
}

// do an alpha beta iterative deepening search
// board_s is the current board state
// color is the color of the player to move
// max_depth is the maximum depth of the search
// return the best move found

Move iterative_deepening(PositionList *board_history, char color, int max_depth)
{
    Move move = empty_move();
    MoveScore new_move_score;
    for (int i = 1; i <= max_depth; i++)
    {
        new_move_score = alphabeta(-MAX_SCORE, MAX_SCORE, 0, i, board_history, color, empty_move(), 1, 0);
        int score = new_move_score.score;
        move = new_move_score.move;
        printf("depth: %d, move: %c%c -> %c%c, score: %d\n", i, 'a' + move.init_co.y, '1' + move.init_co.x, 'a' + move.dest_co.y, '1' + move.dest_co.x, score);
    }
    return move;
}