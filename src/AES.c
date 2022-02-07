#include "include/AES.h"

#include "include/SubBytes.h"
#include "include/ShiftRows.h"
#include "include/MixColumns.h"

#define AES_BLOCK_SIZE 16

int encrypt(const uint8_t *plain,
                size_t sizePlain,
                uint8_t *cipher,
                size_t sizeCipher,
                const uint8_t *roundKeys,
                size_t sizeRoundKeys,
                AESKeyLength keyLength)
{
    if (!(plain && cipher && roundKeys))
        return EFAULT;
    if (sizePlain < AES_BLOCK_SIZE || sizeCipher < AES_BLOCK_SIZE)
        return EINVAL;

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
            return EINVAL;
    }
    if (sizeRoundKeys < (n + 7) * AES_BLOCK_SIZE)
        return EINVAL;

    size_t round = 0;

    memcpy(cipher, plain, AES_BLOCK_SIZE);
    int e;

    // Transpose the state matrix: we assumed that the plain text is loaded by row.
    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < i; ++j)
        {
            uint8_t tmp = cipher[i * 4 + j];
            cipher[i * 4 + j] = cipher[j * 4 + i];
            cipher[j * 4 + i] = tmp;
        }

    e = addRoundKey(cipher, sizeCipher, roundKeys, sizeRoundKeys, round++);
    if (e)
        return e;

    while (round < n + 7)
    {
        e = subBytes(cipher, sizeCipher, false);
        if (e)
            return e;

        e = shiftRows(cipher, sizeCipher, false);
        if (e)
            return e;

        if (round != n + 7 - 1)
        {
            e = mixColumns(cipher, sizeCipher, false);
            if (e)
                return e;
        }

        e = addRoundKey(cipher, sizeCipher, roundKeys, sizeRoundKeys, round++);
        if (e)
            return e;
    }

    // Transpose back the state matrix: the cipher is also need to be consistent with the plain text in terms of format.
    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < i; ++j)
        {
            uint8_t tmp = cipher[i * 4 + j];
            cipher[i * 4 + j] = cipher[j * 4 + i];
            cipher[j * 4 + i] = tmp;
        }

    return 0;
}

int decrypt(const uint8_t *cipher,
                size_t sizeCipher,
                uint8_t *plain,
                size_t sizePlain,
                const uint8_t *roundKeys,
                size_t sizeRoundKeys,
                AESKeyLength keyLength)
{
    if (!(plain && cipher && roundKeys))
        return EFAULT;
    if (sizePlain < AES_BLOCK_SIZE || sizeCipher < AES_BLOCK_SIZE)
        return EINVAL;

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
            return EINVAL;
    }
    if (sizeRoundKeys < (n + 7) * AES_BLOCK_SIZE)
        return EINVAL;

    size_t round = n + 7 - 1;

    memcpy(plain, cipher, AES_BLOCK_SIZE);
    int e;

    // Transpose the state matrix: we assumed that the cipher text is loaded by row.
    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < i; ++j)
        {
            uint8_t tmp = plain[i * 4 + j];
            plain[i * 4 + j] = plain[j * 4 + i];
            plain[j * 4 + i] = tmp;
        }

    e = addRoundKey(plain, sizePlain, roundKeys, sizeRoundKeys, round);
    if (e)
        return e;

    while (round > 0)
    {
        e = shiftRows(plain, sizePlain, true);
        if (e)
            return e;

        e = subBytes(plain, sizePlain, true);
        if (e)
            return e;

        e = addRoundKey(plain, sizePlain, roundKeys, sizeRoundKeys, --round);
        if (e)
            return e;

        if (round)
        {
            e = mixColumns(plain, sizePlain, true);
            if (e)
                return e;
        }
    }

    // Transpose back the state matrix: the plain text is also need to be consistent with the cipher in terms of format.
    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < i; ++j)
        {
            uint8_t tmp = plain[i * 4 + j];
            plain[i * 4 + j] = plain[j * 4 + i];
            plain[j * 4 + i] = tmp;
        }

    return 0;
}
