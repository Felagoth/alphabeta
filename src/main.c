#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "types.h"
#include "alphabeta.h"
#include "chess_logic.h"
#include "interface_uci_like.h"
#include "debug_functions.h"

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

void test_self_engine(double time_white, double time_black)
{
    PositionList *board_history = empty_list();
    BoardState *board_s = init_board();
    board_history = save_position(board_s, board_history);
    Move move = empty_move();
    char color = 'w';
    print_board(board_s);

    for (int i = 0; i < 1000; i++)
    {
        /*
        // ask for ENTER key
        printf("Press [Enter] key to continue.\n");
        fflush(stdin); // option ONE to clean stdin
        getchar();     // wait for ENTER
        */
        if (color == 'w')
        {
            move = iterative_deepening(board_history, color, 20, time_white);
        }
        else
        {
            move = iterative_deepening(board_history, color, 20, time_black);
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
        if (threefold_repetition(board_s, board_history, 0))
        {
            printf("draw by threefold repetition\n");
            break;
        }
        if (board_s->fifty_move_rule >= 100)
        {
            printf("draw by fifty move rule\n");
            break;
        }
    }
    free(board_s);
}

void test_uci_solo()
{
    char buffer[512] = {0};
    PositionList *board_history = malloc(sizeof(PositionList));
    if (board_history == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    board_history->tail = NULL;
    board_history->board_s = NULL;

    const char *commands[] = {
        "position startpos moves e2e4\n",
        "go wtime 1000 btime 1000\n",
        "position startpos moves e2e4 e7e5\n",
        "go wtime 1000 btime 1000\n",
        "position startpos moves e2e4 e7e5 g1f3\n",
        "go wtime 1000 btime 1000\n",
        "position fen \"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\" moves e2e4 e7e5 g1f3 b8c6\n",
        "go wtime 1000 btime 1000\n",
        "position fen \"rnbqkbnr/p3pppp/2p5/3p4/8/2N5/PPPPPPPP/R1BQKBNR w KQkq - 0 1\" moves e2e4 e7e5 g1f3\n",
        "go wtime 1000 btime 1000\n",
        "quit\n"};

    int i = 0;
    while (strcmp(buffer, "quit\n") != 0)
    {
        strcpy(buffer, commands[i]);
        fprintf(stderr, "Debug: Received message from main program:\n %s\n\n", buffer);
        handle_uci_command(buffer, board_history);
        i++;
    }
    free_position_list(board_history);
}

void answer_uci()
{
    char buffer[512] = {0};
    PositionList *board_history = malloc(sizeof(PositionList));
    if (board_history == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    board_history->tail = NULL;
    board_history->board_s = NULL;

    while (strcmp(buffer, "quit\n") != 0)
    {
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            fprintf(stderr, "Debug: Received message from main program:\n %s\n\n", buffer);
            handle_uci_command(buffer, board_history);
        }
        else
        {
            perror("fgets");
            exit(EXIT_FAILURE);
        }
    }
    free_position_list(board_history);
}

int main()
{
    // test_self_engine(5.0, 0.5);
    test_uci_solo();
    // answer_uci();
    return 0;
}