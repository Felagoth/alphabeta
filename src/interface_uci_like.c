#include <stdio.h>
#include "alphabeta.h"
#include "types.h"
#include "chess_logic.h"
#include "debug_functions.h"
#include <string.h>

void print_answer(Move best_move)
{
    if (best_move.init_co.x == -1)
    {
        printf("bestmove (none)\n");
        fflush(stdout);
    }
    else if (best_move.promotion != ' ')
    {
        printf("bestmove %c%c%c%c%c\n", best_move.init_co.y + 'a', best_move.init_co.x + '1', best_move.dest_co.y + 'a', best_move.dest_co.x + '1', best_move.promotion);
        fflush(stdout);
    }
    else
    {
        printf("bestmove %c%c%c%c\n", best_move.init_co.y + 'a', best_move.init_co.x + '1', best_move.dest_co.y + 'a', best_move.dest_co.x + '1');
        fflush(stdout);
    }
}

PositionList *parse_moves(char *token, PositionList *board_history)
{
    char last_char;
    Move move;
    BoardState *new_board_s = malloc(sizeof(BoardState));
    do
    {
        *new_board_s = *board_history->board_s;
        token = strtok(NULL, " ");
        last_char = token[strlen(token) - 1];
        move.init_co.x = token[1] - '1';
        move.init_co.y = token[0] - 'a';
        move.dest_co.x = token[3] - '1';
        move.dest_co.y = token[2] - 'a';
        if (token[4] != '\0' && token[4] != '\n')
        {
            move.promotion = token[4];
        }
        else
        {
            move.promotion = ' ';
        }
        new_board_s = move_piece(new_board_s, move);
        board_history = save_position(new_board_s, board_history);
    } while (last_char != '\n');
    free(new_board_s);
    return board_history;
}

PositionList *parse_position(char *token)
{
    PositionList *board_history = empty_list();
    BoardState *new_board_s;
    token = strtok(NULL, " ");
    if (strcmp(token, "startpos") == 0 || strcmp(token, "startpos\n") == 0)
    {
        new_board_s = init_board();
    }
    else if (strcmp(token, "fen") == 0)
    {
        token = strtok(NULL, "\"");
        new_board_s = FEN_to_board(token);
    }
    else
    {
        fprintf(stderr, "Error: unknown position command\n");
        exit(EXIT_FAILURE);
    }
    board_history = save_position(new_board_s, board_history);
    free(new_board_s);
    if (token[strlen(token) - 1] == '\n')
    {
        return board_history;
    }
    token = strtok(NULL, " ");
    if (strcmp(token, "moves") == 0)
    {
        board_history = parse_moves(token, board_history);
    }
    return board_history;
}

double parse_time_ms(char *token)
{
    if (token[strlen(token) - 1] == '\n')
    {
        token[strlen(token) - 1] = '\0';
    }
    if (strcmp(token, "infinite") == 0)
    {
        return -1;
    }
    return atof(token) / 1000;
}

int parse_depth(char *token)
{
    if (token[strlen(token) - 1] == '\n')
    {
        token[strlen(token) - 1] = '\0';
    }
    return atoi(token);
}

void parse_go(char *token, PositionList *board_history)
{
    int depth = 50;
    double wtime = 0, btime = 0;
    double winc = 0, binc = 0;
    while (token != NULL)
    {
        token = strtok(NULL, " ");
        if (token == NULL)
        {
            break;
        }
        if (strcmp(token, "depth") == 0)
        {
            token = strtok(NULL, " ");
            depth = parse_depth(token);
        }
        else if (strcmp(token, "wtime") == 0)
        {
            token = strtok(NULL, " ");
            wtime = parse_time_ms(token);
        }
        else if (strcmp(token, "btime") == 0)
        {
            token = strtok(NULL, " ");
            btime = parse_time_ms(token);
        }
        else if (strcmp(token, "winc") == 0)
        {
            token = strtok(NULL, " ");
            winc = parse_time_ms(token);
        }
        else if (strcmp(token, "binc") == 0)
        {
            token = strtok(NULL, " ");
            binc = parse_time_ms(token);
        }
        else
        {
            fprintf(stderr, "Error: unknown go command\n");
        }
    }
    (void)winc, (void)binc;
    double time = board_history->board_s->player == WHITE ? wtime : btime;
    if (time < 0.01)
        time = 0.005;
    else if (time == -1)
        time = 3600000;
    else
        time = time - 0.005;
    char color = board_history->board_s->player == WHITE ? 'w' : 'b';
    Move best_move = iterative_deepening(board_history, color, depth, time);
    print_answer(best_move);
    print_board_debug(move_piece(board_history->board_s, best_move));
}

void handle_uci_command(char *command, PositionList *board_history)
{
    if (strlen(command) == 0)
    {
        fprintf(stderr, "Error: empty command\n");
        return;
    }
    char *token = strtok(command, " ");
    if (strcmp(token, "uci\n") == 0)
    {
        printf("id name deltabeta\n");
        fflush(stdout);
        printf("id author Achille Correge\n");
        fflush(stdout);
        printf("uciok\n");
        fflush(stdout);
    }
    else if (strcmp(token, "isready\n") == 0)
    {
        printf("readyok\n");
        fflush(stdout);
    }
    else if (strncmp(token, "position", 8) == 0)
    {
        free_position_list(board_history->tail);
        if (board_history->board_s != NULL)
            free(board_history->board_s);
        PositionList *new_board_history = parse_position(token);
        *board_history = *new_board_history;
        free(new_board_history);
        print_board_debug(board_history->board_s);
    }
    else if (strncmp(token, "go", 2) == 0)
    {
        print_board_debug(board_history->board_s);
        parse_go(token, board_history);
    }
    else if (strcmp(token, "quit\n") == 0)
    {
        // do nothing
    }
    else if (strcmp(token, "ucinewgame\n") == 0)
    {
        // do nothing
    }
    else
    {
        fprintf(stderr, "Error: unknown command\n");
    }
}