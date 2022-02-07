#include "include/SubBytes.h"
#include "include/AddRoundKey.h"

#define DWORD_ROW_INDEX_TO_OFFSET(rowIndex) ((rowIndex) * 4)

static const uint8_t rc[] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

static uint8_t *subWord(uint8_t *dword)
{
    subBytes(dword, 4, false);
    return dword;
}

static uint8_t *rotWord(uint8_t *dword)
{
    uint8_t t = dword[0];

    dword[0] = dword[1];
    dword[1] = dword[2];
    dword[2] = dword[3];
    dword[3] = t;

    return dword;
}

// External functions
errno_t expandRoundKey(AESKeyLength keyLength, uint8_t *roundKeys, size_t sizeRoundKeys)
{
    if (roundKeys == NULL)
        return EFAULT;
    else
    {
        // n is the length of key in DWORD (a.k.a. uint32_t)
        size_t n = 0;

        switch (keyLength)
        {
            case AES_KL_128:
                n = 128 / 32;
                break;
            case AES_KL_192:
                n = 192 / 32;
                break;
            case AES_KL_256:
                n = 256 / 32;
                break;
            default:
                break;
        }

        /*
         * There are (n + 7) rounds. Each round consume a round key of 16 * 16 bytes.
         * Therefore, the total size of round key is (n + 7) * 4 * 4 bytes.
         */
        if (sizeRoundKeys < (n + 7) * 16)
            return EINVAL;

        /*
         * We assume that the key is loaded by row, rather than column,
         * since indices of rows are easier to calculate.
         *
         * We assume that a seed key is loaded in `roundKeys`,
         * so we are skipping the situation where rowIndex = 0 to n - 1.
         *
         * We are doing expansion in byte, rather than in DWORD,
         * so the algorithm has to be modified slightly.
         */
        for (size_t rowIndex = n; rowIndex < (n + 7) * 4; ++rowIndex)
        {
            if (!(rowIndex % n))
            {
                uint8_t previousRow[4];

                errno_t e = memcpy_s(previousRow, 4,
                                     roundKeys + DWORD_ROW_INDEX_TO_OFFSET(rowIndex - 1), 4);
                if (e)
                    return e;

                subWord(rotWord(previousRow));

                for (size_t i = 0; i < 4; ++i)
                {
                    roundKeys[DWORD_ROW_INDEX_TO_OFFSET(rowIndex) + i] =
                        roundKeys[DWORD_ROW_INDEX_TO_OFFSET(rowIndex - n) + i] ^
                        previousRow[i] ^
                        (i ? 0x00 : rc[rowIndex / n]);
                    // If i == 0, XOR wit7h rc[i / n]; otherwise XOR with 0x00 (unchanged)
                }
            }
            else if (n > 6 && rowIndex % n == 4)
            {
                uint8_t previousRow[4];

                errno_t e = memcpy_s(previousRow, 4,
                                     roundKeys + DWORD_ROW_INDEX_TO_OFFSET(rowIndex - 1), 4);
                if (e)
                    return e;

                subWord(previousRow);

                for (size_t i = 0; i < 4; ++i)
                {
                    roundKeys[DWORD_ROW_INDEX_TO_OFFSET(rowIndex) + i] =
                        roundKeys[DWORD_ROW_INDEX_TO_OFFSET(rowIndex - n) + i] ^ previousRow[i];
                }
            }
            else
            {
                for (size_t i = 0; i < 4; ++i)
                {
                    roundKeys[DWORD_ROW_INDEX_TO_OFFSET(rowIndex) + i] =
                        roundKeys[DWORD_ROW_INDEX_TO_OFFSET(rowIndex - n) + i] ^
                        roundKeys[DWORD_ROW_INDEX_TO_OFFSET(rowIndex - 1) + i];
                }
            }
        }

        return 0;
    }
}

// Main procedure
errno_t addRoundKey(
    uint8_t *matrix, size_t sizeMatrix,
    const uint8_t *roundKeys, size_t sizeRoundKeys,
    size_t round)
{
    if (matrix == NULL || roundKeys == NULL)
        return EFAULT;
    if (sizeMatrix < 16)
        return EINVAL;
    if (sizeRoundKeys == 0 || sizeRoundKeys < (round + 1) * 16)
        return EINVAL;

    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < 4; ++j)
            matrix[i * 4 + j] ^= roundKeys[round * 16 + j * 4 + i];

    return 0;
}
