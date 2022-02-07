#ifndef AES_AES_H
#define AES_AES_H

#include <inttypes.h>
#include <errno.h>

#include "AddRoundKey.h"

errno_t
encrypt(const uint8_t *plain,
        size_t sizePlain,
        uint8_t *cipher,
        size_t sizeCipher,
        const uint8_t *roundKeys,
        size_t sizeRoundKeys,
        AESKeyLength keyLength);

errno_t
decrypt(const uint8_t *cipher,
        size_t sizeCipher,
        uint8_t *plain,
        size_t sizePlain,
        const uint8_t *roundKeys,
        size_t sizeRoundKeys,
        AESKeyLength keyLength);

#endif //AES_AES_H
