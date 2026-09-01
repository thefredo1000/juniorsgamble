#include "hash_table.h"
#include "tables.h"
#include "hash.h"

static short binaries_by_id[52] = {
    0x1,
    0x1,
    0x1,
    0x1,
    0x2,
    0x2,
    0x2,
    0x2,
    0x4,
    0x4,
    0x4,
    0x4,
    0x8,
    0x8,
    0x8,
    0x8,
    0x10,
    0x10,
    0x10,
    0x10,
    0x20,
    0x20,
    0x20,
    0x20,
    0x40,
    0x40,
    0x40,
    0x40,
    0x80,
    0x80,
    0x80,
    0x80,
    0x100,
    0x100,
    0x100,
    0x100,
    0x200,
    0x200,
    0x200,
    0x200,
    0x400,
    0x400,
    0x400,
    0x400,
    0x800,
    0x800,
    0x800,
    0x800,
    0x1000,
    0x1000,
    0x1000,
    0x1000,
};

static short suitbit_by_id[52] = {
    0x1,
    0x8,
    0x40,
    0x200,
    0x1,
    0x8,
    0x40,
    0x200,
    0x1,
    0x8,
    0x40,
    0x200,
    0x1,
    0x8,
    0x40,
    0x200,
    0x1,
    0x8,
    0x40,
    0x200,
    0x1,
    0x8,
    0x40,
    0x200,
    0x1,
    0x8,
    0x40,
    0x200,
    0x1,
    0x8,
    0x40,
    0x200,
    0x1,
    0x8,
    0x40,
    0x200,
    0x1,
    0x8,
    0x40,
    0x200,
    0x1,
    0x8,
    0x40,
    0x200,
    0x1,
    0x8,
    0x40,
    0x200,
    0x1,
    0x8,
    0x40,
    0x200,
};
/*
 * Card id, ranged from 0 to 51.
 * The two least significant bits represent the suit, ranged from 0-3.
 * The rest of it represent the rank, ranged from 0-12.
 * 13 * 4 gives 52 ids.
 * Code from : https://github.com/HenryRLee/PokerHandEvaluator/blob/master/cpp/src/evaluator7.c
 */
int evaluate_7cards(int a, int b, int c, int d, int e, int f, int g)
{
    int suit_hash = 0;

    suit_hash += suitbit_by_id[a];
    suit_hash += suitbit_by_id[b];
    suit_hash += suitbit_by_id[c];
    suit_hash += suitbit_by_id[d];
    suit_hash += suitbit_by_id[e];
    suit_hash += suitbit_by_id[f];
    suit_hash += suitbit_by_id[g];

    if (suits[suit_hash])
    {
        int suit_binary[4] = {0};
        suit_binary[a & 0x3] |= binaries_by_id[a];
        suit_binary[b & 0x3] |= binaries_by_id[b];
        suit_binary[c & 0x3] |= binaries_by_id[c];
        suit_binary[d & 0x3] |= binaries_by_id[d];
        suit_binary[e & 0x3] |= binaries_by_id[e];
        suit_binary[f & 0x3] |= binaries_by_id[f];
        suit_binary[g & 0x3] |= binaries_by_id[g];

        return flush[suit_binary[suits[suit_hash] - 1]];
    }

    unsigned char quinary[13] = {0};

    quinary[(a >> 2)]++;
    quinary[(b >> 2)]++;
    quinary[(c >> 2)]++;
    quinary[(d >> 2)]++;
    quinary[(e >> 2)]++;
    quinary[(f >> 2)]++;
    quinary[(g >> 2)]++;

    const int hash = hash_quinary(quinary, 7);

    return noflush7[hash];
}