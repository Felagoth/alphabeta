#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "types.h"
#include "chess_logic.h"

// create magic!
// https://analog-hors.github.io/site/magic-bitboards/

typedef uint64_t Bitboard;

int rook_index_bits[64];
Bitboard rook_magic_numbers[64];
Bitboard rook_entry_masks[64];
Bitboard rook_table[64][4096];

int bishop_index_bits[64];
Bitboard bishop_magic_numbers[64];
Bitboard bishop_entry_masks[64];
Bitboard bishop_tables[64][512];

int min(int a, int b)
{
    if (a < b)
        return a;
    else
        return b;
}

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

void generate_rook_index_bits()
{
    int index;
    for (int i = 0; i < 64; i++)
    {
        index = 10;
        if (i < 8 || i > 55)
        {
            index++;
        }
        if (i % 8 == 0 || i % 8 == 7)
        {
            index++;
        }
        rook_index_bits[i] = index;
    }
}

void generate_bishop_index_bits()
{
    int dist_edge;
    int x, y;
    for (int i = 0; i < 64; i++)
    {
        x = i % 8;
        y = i / 8;
        if ((x == 0 || x == 7) && (y == 0 || y == 7))
        {
            bishop_index_bits[i] = 6;
        }
        else if (x == 0 || x == 7 || y == 0 || y == 7)
        {
            bishop_index_bits[i] = 5;
        }
        else
        {
            dist_edge = min(min(7 - x, x), min(7 - y, y));
            bishop_index_bits[i] = 3 + dist_edge * 2;
        }
    }
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

Bitboard generate_bishop_entry_mask(int square)
{
    Bitboard mask = 0;
    int x = square % 8;
    int y = square / 8;
    // int dist_ne = (min(7 - y, x));
    for (int i = 1; i < min(x, y); i++)
    {
        mask |= 1ULL << ((y - i) * 8 + x - i);
    }
    for (int i = 1; i < min(x, 7 - y); i++)
    {
        mask |= 1ULL << ((y + i) * 8 + x - i);
    }
    for (int i = 1; i < min(7 - x, 7 - y); i++)
    {
        mask |= 1ULL << ((y + i) * 8 + x + i);
    }
    for (int i = 1; i < min(7 - x, y); i++)
    {
        mask |= 1ULL << ((y - i) * 8 + x + i);
    }
    return mask;
}

void generate_all_entry_masks()
{
    for (int i = 0; i < 64; i++)
    {
        rook_entry_masks[i] = generate_rook_entry_mask(i);
        bishop_entry_masks[i] = generate_bishop_entry_mask(i);
    }
}

Bitboard find_rook_moves(Bitboard blockers, int square)
{
    Bitboard moves = 0;
    int x = square % 8;
    int y = square / 8;
    for (int i = x + 1; i < 8; i++)
    {
        moves |= 1ULL << (y * 8 + i);
        if (blockers & (1ULL << (y * 8 + i)))
            break;
    }
    for (int i = x - 1; i >= 0; i--)
    {
        moves |= 1ULL << (y * 8 + i);
        if (blockers & (1ULL << (y * 8 + i)))
            break;
    }
    for (int i = y + 1; i < 8; i++)
    {
        moves |= 1ULL << (i * 8 + x);
        if (blockers & (1ULL << (i * 8 + x)))
            break;
    }
    for (int i = y - 1; i >= 0; i--)
    {
        moves |= 1ULL << (i * 8 + x);
        if (blockers & (1ULL << (i * 8 + x)))
            break;
    }
    return moves;
}

Bitboard find_bishop_moves(Bitboard blockers, int square)
{
    Bitboard moves = 0;
    int x = square % 8;
    int y = square / 8;
    // dist_ne = (min(7 - y, x));
    for (int i = 1; i <= min(x, y); i++)
    {
        moves |= 1ULL << ((y - i) * 8 + x - i);
        if (blockers & 1ULL << ((y - i) * 8 + x - i))
            break;
    }
    for (int i = 1; i <= min(x, 7 - y); i++)
    {
        moves |= 1ULL << ((y + i) * 8 + x - i);
        if (blockers & 1ULL << ((y + i) * 8 + x - i))
            break;
    }
    for (int i = 1; i <= min(7 - x, 7 - y); i++)
    {
        moves |= 1ULL << ((y + i) * 8 + x + i);
        if (blockers & 1ULL << ((y + i) * 8 + x + i))
            break;
    }
    for (int i = 1; i <= min(7 - x, y); i++)
    {
        moves |= 1ULL << ((y - i) * 8 + x + i);
        if (blockers & 1ULL << ((y - i) * 8 + x + i))
            break;
    }
    return moves;
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

bool try_make_table(Bitboard magic_number, int index_bits, Bitboard mask, int square, Bitboard *table)
{
    for (int i = 0; i < (1 << index_bits); i++)
    {
        table[i] = 0;
    }
    for (int i = 0; i < (1 << index_bits); i++)
    {
        Bitboard blockers = blockers_for_index(i, mask);
        Bitboard moves;
        if (index_bits > 9)
            moves = find_rook_moves(blockers, square);
        else
            moves = find_bishop_moves(blockers, square);

        int table_entry = (blockers * magic_number) >> (64 - index_bits);
        if (table_entry >= (1 << index_bits))
        {
            printf("table entry; %d\n", table_entry);
            printf("wtf\n");
            return NULL;
        }
        if (table[table_entry] == 0)
        {
            if (i == (1 << index_bits) - 1)
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
            // printf("collision benigne\n");
        }
    }
    return true;
}

typedef struct
{
    Bitboard magic_number;
    Bitboard *table;
} Magik;

Magik find_magik(int square, Bitboard blockers, int index_bits)
{
    Bitboard entry_mask;
    if (index_bits > 9)
        entry_mask = rook_entry_masks[square];
    else
        entry_mask = bishop_entry_masks[square];
    int i = 1;
    Bitboard *table = malloc(sizeof(Bitboard) * (1 << index_bits));
    if (table == NULL)
    {
        printf("could not allocate memory for table\n");
        Magik magik = {0, NULL};
        return magik;
    }
    while (true)
    {
        for (int i = 0; i < (1 << index_bits); i++)
        {
            table[i] = 0;
        }
        Bitboard magic_number = random_bitboard() & random_bitboard() & random_bitboard();
        // printf("trying magic number \n");
        // print_bitboard(magic_number);

        if (try_make_table(magic_number, index_bits, entry_mask, square, table))
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
    int index_bits;
    for (int i = 0; i < 64; i++)
    {
        printf("generating rook magik for square %d\n", i);
        index_bits = rook_index_bits[i];
        magik = find_magik(i, rook_entry_masks[i], index_bits);
        rook_magic_numbers[i] = magik.magic_number;
        memcpy(rook_table[i], magik.table, sizeof(Bitboard) * (1 << index_bits));
        free(magik.table);
    }
}

void generate_all_bishop_magik()
{
    Magik magik;
    int index_bits;
    for (int i = 0; i < 64; i++)
    {
        printf("generating bishop magik for square %d\n", i);
        index_bits = bishop_index_bits[i];
        magik = find_magik(i, bishop_entry_masks[i], index_bits);
        bishop_magic_numbers[i] = magik.magic_number;
        memcpy(bishop_tables[i], magik.table, sizeof(Bitboard) * (1 << index_bits));
        free(magik.table);
    }
}

Bitboard get_rook_moves(Bitboard blockers, int square)
{
    blockers &= rook_entry_masks[square];
    Bitboard index = (blockers * rook_magic_numbers[square]) >> (64 - rook_index_bits[square]);
    return rook_table[square][index];
}

Bitboard get_bishop_moves(Bitboard blockers, int square)
{
    blockers &= bishop_entry_masks[square];
    Bitboard index = (blockers * bishop_magic_numbers[square]) >> (64 - bishop_index_bits[square]);
    return bishop_tables[square][index];
}

void test_rook_functions()
{
    // test intermediate functions for one square
    printf("tests for rook functions");
    int square = 13;
    Bitboard mask = rook_entry_masks[square];
    print_bitboard(mask);

    for (int i = 0; i < (1 << rook_index_bits[square]); i++)
    {
        printf("index: %d\n", i);
        printf("blockers\n");
        Bitboard blockers = blockers_for_index(i, mask);
        print_bitboard(blockers);
        printf("moves\n");
        Bitboard moves = find_rook_moves(blockers, square);
        print_bitboard(moves);
        printf("blockers * magic number:%lu\n", blockers * 0xffff000ff00);
    }
}

void test_bishop_functions()
{
    // test intermediate functions for one square
    printf("tests for bishop functions\n");
    int square = 21;
    Bitboard mask = bishop_entry_masks[square];
    print_bitboard(mask);
    printf("index bits for square 13: %d\n", bishop_index_bits[square]);

    for (int i = 0; i < (1 << bishop_index_bits[square]); i++)
    {
        printf("index: %d\n", i);
        printf("blockers\n");
        Bitboard blockers = blockers_for_index(i, mask);
        print_bitboard(blockers);
        printf("moves\n");
        Bitboard moves = find_bishop_moves(blockers, square);
        print_bitboard(moves);
        printf("blockers * magic number:%lu\n", blockers * 0xffff000ff00);
    }
}

void test_magic_gen_first_rook_square()
{
    // test the magic number generation for the first square
    printf("index 0: %d\n", rook_index_bits[0]);
    Magik magik = find_magik(0, rook_entry_masks[0], rook_index_bits[0]);
    printf("magic number\n");
    print_bitboard(magik.magic_number);
    memcpy(rook_table[0], magik.table, sizeof(Bitboard) * (1 << rook_index_bits[0]));
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
    printf("index: %lu\n", blockers * rook_magic_numbers[0] >> (64 - rook_index_bits[0]));
    Bitboard moves = get_rook_moves(blockers, 0);
    print_bitboard(moves);
}

void test_rook_magik()
{
    // test the rook table
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
}

void test_bishop_magik()
{
    // test the bishop table
    Bitboard blockers = 2;
    Bitboard moves = get_bishop_moves(blockers, 0);
    print_bitboard(moves);
    blockers = 0x0000000000001000;
    moves = get_bishop_moves(blockers, 13);
    print_bitboard(moves);
    blockers = 0xFFFFFFFFFFFFFFFF;
    print_bitboard(blockers);
    moves = get_bishop_moves(blockers, 13);
    print_bitboard(moves);
    blockers = 0x0000000400000000;
    print_bitboard(blockers);
    moves = get_bishop_moves(blockers, 13);
    print_bitboard(moves);
    blockers = 0x0000000008001000;
    print_bitboard(blockers);
    moves = get_bishop_moves(blockers, 13);
    print_bitboard(moves);
}

void save_int_array_to_file(const int *array, size_t size, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    fprintf(file, "{");
    for (size_t i = 0; i < size; ++i)
    {
        fprintf(file, "%d", array[i]);
        if (i < size - 1)
        {
            fprintf(file, ", ");
        }
    }
    fprintf(file, "}");

    fclose(file);
}

void save_bb_array_to_file(const Bitboard *array, size_t size, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    fprintf(file, "{");
    for (size_t i = 0; i < size; ++i)
    {
        fprintf(file, "%luULL", array[i]);
        if (i < size - 1)
        {
            fprintf(file, ", ");
        }
    }
    fprintf(file, "}");

    fclose(file);
}

void save_mag_table_to_file(const Bitboard *array, size_t size, size_t second_dim_max_size, const int *index_bits, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    fprintf(file, "{");
    for (size_t i = 0; i < size; ++i)
    {
        int size2 = 1 << index_bits[i];
        fprintf(file, "{");
        for (size_t j = 0; j < size2; ++j)
        {
            fprintf(file, "%luULL", *((array + i * second_dim_max_size) + j));
            if (j < size2 - 1)
            {
                fprintf(file, ", ");
            }
        }
        if (i < size - 1)
        {
            fprintf(file, "},\n");
        }
    }
    fprintf(file, "}}");
    fclose(file);
}

int main()
{
    generate_all_entry_masks();
    generate_rook_index_bits();
    generate_bishop_index_bits();

    // test_rook_functions();
    // test_bishop_functions();
    // test_magic_gen_first_rook_square();

    // generate_all_rook_magik();
    // test_rook_magik();

    // save_int_array_to_file(rook_index_bits, 64, "builds/magik/rook_index_bits.txt");
    // save_bb_array_to_file(rook_magic_numbers, 64, "builds/magik/rook_magic_numbers.txt");
    // save_bb_array_to_file(rook_entry_masks, 64, "builds/magik/rook_entry_masks.txt");
    // save_mag_table_to_file((const Bitboard *)rook_table, 64, 4096, rook_index_bits, "builds/magik/rook_table.txt");

    generate_all_bishop_magik();
    test_bishop_magik();

    save_int_array_to_file(bishop_index_bits, 64, "builds/magik/bishop_index_bits.txt");
    save_bb_array_to_file(bishop_magic_numbers, 64, "builds/magik/bishop_magic_numbers.txt");
    save_bb_array_to_file(bishop_entry_masks, 64, "builds/magik/bishop_entry_masks.txt");
    save_mag_table_to_file((const Bitboard *)bishop_tables, 64, 512, bishop_index_bits, "builds/magik/bishop_table.txt");
    return 0;
}