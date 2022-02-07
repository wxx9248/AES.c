/*
 * Carrying out FIPS-197 specified tests.
 */

#include <stdio.h>

#include "include/AES.h"

#define TEST_KEY_LENGTH 32    // Option: 16, 24, or 32 bytes
#define TEST_KEY_LENGTH_ENUM AES_KL_256

static void printMatrix(uint8_t *matrix, size_t sizeMatrix);

int main(void)
{
    static uint8_t key[TEST_KEY_LENGTH];
    static uint8_t plain[16];
    static uint8_t cipher[16];
    static uint8_t roundKeys[(TEST_KEY_LENGTH / 4 + 7) * 16];

    for (size_t i = 0; i < sizeof(key); ++i)
        key[i] = (uint8_t) i;
    puts("Key matrix:");
    printMatrix(key, sizeof(key));

    for (size_t i = 0; i < sizeof(plain); ++i)
        plain[i] = (uint8_t) ((i << 4) + i);
    puts("Plain text matrix:");
    printMatrix(plain, sizeof(plain));

    errno_t e;

    memcpy_s(roundKeys, sizeof(roundKeys), key, sizeof(key));
    e = expandRoundKey(TEST_KEY_LENGTH_ENUM, roundKeys, sizeof(roundKeys));
    if (e)
    {
        fprintf(stderr, "Error code: %d", e);
        return e;
    }
    puts("Expanded round key matrix:");
    printMatrix(roundKeys, sizeof(roundKeys));


    printf("AES-%d encryption test:\n", TEST_KEY_LENGTH * 8);
    e = encrypt(plain, sizeof(plain), cipher, sizeof(cipher), roundKeys, sizeof(roundKeys), TEST_KEY_LENGTH_ENUM);
    if (e)
    {
        fprintf(stderr, "Error code: %d", e);
        return e;
    }
    puts("Cipher text matrix:");
    printMatrix(cipher, sizeof(cipher));


    printf("AES-%d decryption test:\n", TEST_KEY_LENGTH * 8);
    e = decrypt(cipher, sizeof(cipher), plain, sizeof(plain), roundKeys, sizeof(roundKeys), TEST_KEY_LENGTH_ENUM);
    if (e)
    {
        fprintf(stderr, "Error code: %d", e);
        return e;
    }
    puts("Plain text matrix:");
    printMatrix(plain, sizeof(plain));

    return 0;
}

void printMatrix(uint8_t *matrix, size_t sizeMatrix)
{
    if (!(matrix && sizeMatrix))
        return;

    for (size_t i = 0; i < sizeMatrix; ++i)
    {
        printf("0x%02hhx ", matrix[i]);
        if (i % 4 == 3)
            putchar('\n');
    }
}
