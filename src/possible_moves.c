#include <stdlib.h>
#include <stdint.h>

#include "types.h"
#include "chess_logic.h"

// use bitboards to generate possible moves

typedef uint64_t Bitboard;

#define RANK_1 0xFF
#define RANK_2 0xFF00
#define RANK_3 0xFF0000
#define RANK_4 0xFF000000
#define RANK_5 0xFF00000000
#define RANK_6 0xFF0000000000
#define RANK_7 0xFF000000000000
#define RANK_8 0xFF00000000000000
#define FILE_A 0x0101010101010101
#define FILE_B 0x0202020202020202
#define FILE_C 0x0404040404040404
#define FILE_D 0x0808080808080808
#define FILE_E 0x1010101010101010
#define FILE_F 0x2020202020202020
#define FILE_G 0x4040404040404040
#define FILE_H 0x8080808080808080

void print_bitboard(Bitboard b)
{
    for (int i = 63; i >= 0; i--)
    {
        printf("%d", (b >> i) & 1);
        if (i % 8 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}

Bitboard init_white()
{
    return 0xFFFF;
}

Bitboard init_black()
{
    return 0xFFFF000000000000;
}

Bitboard get_empty(Bitboard white, Bitboard black)
{
    return ~(white | black);
}

Bitboard init_white_pawns()
{
    return 0xFF00;
}

Bitboard init_black_pawns()
{
    return 0xFF000000000000;
}

Bitboard get_white_pawn_moves(Bitboard pawns, Bitboard empty, Bitboard enemy, char color, int en_passant)
{
    Bitboard moves_1_square = (pawns << 8) & empty;
    Bitboard moves_2_squares = ((moves_1_square & RANK_3) << 8) & empty;
    Bitboard attacks = ((pawns & ~FILE_A) << 7) & enemy;
    attacks |= ((pawns & ~FILE_H) << 9) & enemy;
    if (en_passant != -1)
    {
        Bitboard en_passant_square = 1ULL << en_passant;
        attacks |= (pawns & ~FILE_A) << 7 & en_passant_square;
        attacks |= (pawns & ~FILE_H) << 9 & en_passant_square;
    }
    return (moves_1_square | moves_2_squares | attacks);
}

Bitboard get_black_pawn_moves(Bitboard pawns, Bitboard empty, Bitboard enemy, char color, int en_passant)
{
    Bitboard moves_1_square = (pawns >> 8) & empty;
    Bitboard moves_2_squares = ((moves_1_square & RANK_6) >> 8) & empty;
    Bitboard attacks = ((pawns & ~FILE_A) >> 9) & enemy;
    attacks |= ((pawns & ~FILE_H) >> 7) & enemy;
    if (en_passant != -1)
    {
        Bitboard en_passant_column = FILE_A << en_passant;
        attacks |= (pawns & ~FILE_A) >> 9 & en_passant_column;
        attacks |= (pawns & ~FILE_H) >> 7 & en_passant_column;
    }
    return (moves_1_square | moves_2_squares | attacks);
}

Bitboard init_white_knights()
{
    return 0x42;
}

Bitboard init_black_knights()
{
    return 0x4200000000000000;
}

Bitboard get_knight_moves(Bitboard knights, Bitboard ally)
{
    Bitboard knight_moves = 0;
    Bitboard not_a_file = ~FILE_A;
    Bitboard not_ab_file = ~FILE_A & ~FILE_B;
    Bitboard not_h_file = ~FILE_H;
    Bitboard not_gh_file = ~FILE_H & ~FILE_G;
    Bitboard knight_moves_north = (knights << 17) & not_a_file;
    knight_moves_north |= (knights << 15) & not_h_file;
    knight_moves_north |= (knights << 10) & not_ab_file;
    knight_moves_north |= (knights << 6) & not_gh_file;
    Bitboard knight_moves_south = (knights >> 17) & not_h_file;
    knight_moves_south |= (knights >> 15) & not_a_file;
    knight_moves_south |= (knights >> 10) & not_gh_file;
    knight_moves_south |= (knights >> 6) & not_ab_file;
    knight_moves = knight_moves_north | knight_moves_south;
    return knight_moves & ~knights & ~ally;
}

int main()
{
    Bitboard white = init_white();
    Bitboard black = init_black();
    Bitboard empty_squares = get_empty(white, black);
    Bitboard white_pawns = init_white_pawns();
    Bitboard black_pawns = init_black_pawns();
    Bitboard white_knights = init_white_knights();
    Bitboard black_knights = init_black_knights();
    Bitboard white_pawn_moves = get_white_pawn_moves(white_pawns, empty_squares, black, 'w', -1);
    Bitboard black_pawn_moves = get_black_pawn_moves(black_pawns, empty_squares, white, 'b', -1);
    Bitboard white_knight_moves = get_knight_moves(white_knights, white);
    Bitboard black_knight_moves = get_knight_moves(black_knights, black);
    printf("white pawns\n");
    print_bitboard(white_pawns);
    printf("black pawns\n");
    print_bitboard(black_pawns);
    printf("white knights\n");
    print_bitboard(white_knights);
    printf("black knights\n");
    print_bitboard(black_knights);
    printf("white pawn moves\n");
    print_bitboard(white_pawn_moves);
    printf("black pawn moves\n");
    print_bitboard(black_pawn_moves);
    printf("white knight moves\n");
    print_bitboard(white_knight_moves);
    printf("black knight moves\n");
    print_bitboard(black_knight_moves);
    return 0;
}