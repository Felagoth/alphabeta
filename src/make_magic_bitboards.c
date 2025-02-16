#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "types.h"
#include "chess_logic.h"

// use bitboards to generate possible moves

typedef uint64_t Bitboard;

const int rook_index_bits = 12;
const int number_of_attacks = 1 << rook_index_bits;
Bitboard rook_magic_numbers[64];
Bitboard rook_entry_masks[64];
Bitboard rook_table[64][4096];

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

Bitboard random_bitboard()
{
    Bitboard res = 0;
    for (int i = 0; i < 64; i++)
    {
        if (rand() % 2 == 0)
        {
            res |= 1ULL << i;
        }
    }
    return res;
}

Bitboard generate_rook_entry_mask(int square)
{
    Bitboard mask = 0;
    int x = square % 8;
    int y = square / 8;
    for (int i = x + 1; i < 7; i++)
    {
        mask |= 1ULL << (y * 8 + i);
    }
    for (int i = x - 1; i > 0; i--)
    {
        mask |= 1ULL << (y * 8 + i);
    }
    for (int i = y + 1; i < 7; i++)
    {
        mask |= 1ULL << (i * 8 + x);
    }
    for (int i = y - 1; i > 0; i--)
    {
        mask |= 1ULL << (i * 8 + x);
    }
    return mask;
}

void generate_all_rook_entry_masks()
{
    Bitboard all_masks = 0;
    for (int i = 0; i < 64; i++)
    {
        rook_entry_masks[i] = generate_rook_entry_mask(i);
    }
}

Bitboard blockers_for_index(int index, Bitboard mask)
{
    Bitboard blockers = 0;
    for (int i = 0; i < 64; i++)
    {
        if (mask & (1ULL << i))
        {
            if (index & 1)
            {
                blockers |= 1ULL << i;
            }
            index >>= 1;
        }
    }
    return blockers;
}

Bitboard find_moves(Bitboard blockers, int square)
{
    Bitboard moves = 0;
    int x = square % 8;
    int y = square / 8;
    for (int i = x + 1; i < 8; i++)
    {
        moves |= 1ULL << (y * 8 + i);
        if (blockers & (1ULL << (y * 8 + i)))
        {
            break;
        }
    }
    for (int i = x - 1; i >= 0; i--)
    {
        moves |= 1ULL << (y * 8 + i);
        if (blockers & (1ULL << (y * 8 + i)))
        {
            break;
        }
    }
    for (int i = y + 1; i < 8; i++)
    {
        moves |= 1ULL << (i * 8 + x);
        if (blockers & (1ULL << (i * 8 + x)))
        {
            break;
        }
    }
    for (int i = y - 1; i >= 0; i--)
    {
        moves |= 1ULL << (i * 8 + x);
        if (blockers & (1ULL << (i * 8 + x)))
        {
            break;
        }
    }
    return moves;
}

bool try_make_table(Bitboard magic_number, int index_bits, Bitboard mask, int square, Bitboard *table)
{
    for (int i = 0; i < (1 << index_bits); i++)
    {
        table[i] = 0;
    }
    for (int i = 0; i < (1 << index_bits); i++)
    {
        Bitboard blockers = blockers_for_index(i, mask);
        Bitboard moves = find_moves(blockers, square);
        int table_entry = (blockers * magic_number) >> (64 - index_bits);
        if (table_entry >= (1 << index_bits))
        {
            printf("table entry; %d\n", table_entry);
            printf("magic number too small\n");
            return NULL;
        }
        if (i == 2048 && square == 1)
        {
            printf("blockers\n");
            print_bitboard(blockers);
            printf("moves\n");
            print_bitboard(moves);
            printf("table entry: %d\n", table_entry);
            print_bitboard(table[table_entry]);
        }
        if (table[table_entry] == 0)
        {
            if (i % 1000 == 0 && i > 2000)
            {
                printf("found working entry for index %d\n", i);
            }
            table[table_entry] = moves;
        }
        else if (table[table_entry] != moves)
        {
            // printf("collision\n");
            return false;
        }
        else
        {
            // return false;
        }
    }
    return true;
}

typedef struct
{
    Bitboard magic_number;
    Bitboard *table;
} Magik;

Magik find_magik(int square, Bitboard blockers)
{
    Bitboard entry_mask = rook_entry_masks[square];
    int i = 1;
    Bitboard *table = malloc(sizeof(Bitboard) * (1 << rook_index_bits));
    if (table == NULL)
    {
        printf("could not allocate memory for table\n");
        Magik magik = {0, NULL};
        return magik;
    }
    while (true)
    {
        for (int i = 0; i < (1 << rook_index_bits); i++)
        {
            table[i] = 0;
        }
        Bitboard magic_number = random_bitboard() & random_bitboard() & random_bitboard();
        // printf("trying magic number \n");
        // print_bitboard(magic_number);

        if (try_make_table(magic_number, rook_index_bits, entry_mask, square, table))
        {
            Magik magik = {magic_number, table};
            return magik;
        }
        if (i % 100000 == 0)
        {
            printf("tried %d magic numbers\n", i);
        }
        i++;
    }
}

void generate_all_rook_magik()
{
    Magik magik;
    for (int i = 0; i < 64; i++)
    {
        printf("generating magik for square %d\n", i);
        magik = find_magik(i, rook_entry_masks[i]);
        rook_magic_numbers[i] = magik.magic_number;
        memcpy(rook_table[i], magik.table, sizeof(Bitboard) * (1 << rook_index_bits));
        free(magik.table);
    }
}

Bitboard get_rook_moves(Bitboard blockers, int square)
{
    blockers &= rook_entry_masks[square];
    Bitboard index = (blockers * rook_magic_numbers[square]) >> (64 - rook_index_bits);
    return rook_table[square][index];
}

void test_interm_functions()
{
    // test intermediate functions for one square
    Bitboard mask = rook_entry_masks[13];
    print_bitboard(mask);

    for (int i = 0; i < (1 << rook_index_bits); i++)
    {
        printf("index: %d\n", i);
        printf("blockers\n");
        Bitboard blockers = blockers_for_index(i, mask);
        print_bitboard(blockers);
        printf("moves\n");
        Bitboard moves = find_moves(blockers, 13);
        print_bitboard(moves);
        printf("blockers * magic number:%llu\n", blockers * 0xffff000ff00);
    }
}

void test_magic_gen_first_square()
{
    // test the magic number generation for the first square
    Magik magik = find_magik(0, rook_entry_masks[0]);
    printf("magic number\n");
    print_bitboard(magik.magic_number);
    memcpy(rook_table[0], magik.table, sizeof(Bitboard) * (1 << rook_index_bits));
    free(magik.table);
    rook_magic_numbers[0] = magik.magic_number;
    printf("table\n");
    // for (int i = 0; i < (1 << rook_index_bits); i++)
    // {
    //     print_bitboard(magik.table[i]);
    //     print_bitboard(rook_table[0][i]);
    // }
    printf("testing the table\n");
    Bitboard blockers = 4;
    printf("blockers\n");
    print_bitboard(blockers);
    printf("index: %llu\n", blockers * rook_magic_numbers[0] >> (64 - rook_index_bits));
    Bitboard moves = get_rook_moves(blockers, 0);
    print_bitboard(moves);
}

int main()
{
    generate_all_rook_entry_masks();

    // test_interm_functions();
    test_magic_gen_first_square();

    generate_all_rook_magik();

    // test the rook table=
    Bitboard blockers = 2;
    Bitboard moves = get_rook_moves(blockers, 0);
    print_bitboard(moves);
    blockers = 0x0000000000001000;
    moves = get_rook_moves(blockers, 13);
    print_bitboard(moves);
    blockers = 0x0000000000200000;
    print_bitboard(blockers);
    moves = get_rook_moves(blockers, 13);
    print_bitboard(moves);
    blockers = 0x0000000000201000;
    print_bitboard(blockers);
    moves = get_rook_moves(blockers, 13);
    print_bitboard(moves);
    blockers = 0x0000000002001000;
    print_bitboard(blockers);
    moves = get_rook_moves(blockers, 13);
    print_bitboard(moves);

    return 0;
}