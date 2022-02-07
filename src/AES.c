#include "include/AES.h"

#include "include/SubBytes.h"
#include "include/ShiftRows.h"
#include "include/MixColumns.h"

errno_t encrypt(const uint8_t *plain,
                size_t sizePlain,
                uint8_t *cipher,
                size_t sizeCipher,
                const uint8_t *roundKeys,
                size_t sizeRoundKeys,
                AESKeyLength keyLength)
{
    if (!(plain && cipher && roundKeys))
        return EFAULT;
    if (sizePlain < 16 || sizeCipher < 16)
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
            break;
    }
    if (sizeRoundKeys < (n + 7) * 16)
        return EINVAL;

    size_t round = 0;

    errno_t e = memcpy_s(cipher, sizeCipher, plain, sizePlain);
    if (e)
        return e;

    // Transpose the state matrix: we assumed that the plain text is loaded by row.
    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < i; ++j)
            if (cipher[i * 4 + j] != cipher[j * 4 + i])
                cipher[i * 4 + j] ^= cipher[j * 4 + i] ^= cipher[i * 4 + j] ^= cipher[j * 4 + i];

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
            if (cipher[i * 4 + j] != cipher[j * 4 + i])
                cipher[i * 4 + j] ^= cipher[j * 4 + i] ^= cipher[i * 4 + j] ^= cipher[j * 4 + i];

    return 0;
}

errno_t decrypt(const uint8_t *cipher,
                size_t sizeCipher,
                uint8_t *plain,
                size_t sizePlain,
                const uint8_t *roundKeys,
                size_t sizeRoundKeys,
                AESKeyLength keyLength)
{
    if (!(plain && cipher && roundKeys))
        return EFAULT;
    if (sizePlain < 16 || sizeCipher < 16)
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
            break;
    }
    if (sizeRoundKeys < (n + 7) * 16)
        return EINVAL;

    size_t round = n + 7 - 1;

    errno_t e = memcpy_s(plain, sizePlain, cipher, sizeCipher);
    if (e)
        return e;

    // Transpose the state matrix: we assumed that the cipher text is loaded by row.
    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < i; ++j)
            if (plain[i * 4 + j] != plain[j * 4 + i])
                plain[i * 4 + j] ^= plain[j * 4 + i] ^= plain[i * 4 + j] ^= plain[j * 4 + i];

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
            if (plain[i * 4 + j] != plain[j * 4 + i])
                plain[i * 4 + j] ^= plain[j * 4 + i] ^= plain[i * 4 + j] ^= plain[j * 4 + i];

    return 0;
}
