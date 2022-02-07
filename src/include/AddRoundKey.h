#ifndef AES_ADDROUNDKEY_H
#define AES_ADDROUNDKEY_H

#include <errno.h>
#include <string.h>

typedef enum
{
    AES_KL_128,
    AES_KL_192,
    AES_KL_256
} AESKeyLength;

int expandRoundKey(AESKeyLength keyLength, uint8_t *roundKeys, size_t sizeRoundKeys);
int addRoundKey(uint8_t *matrix, size_t sizeMatrix, const uint8_t *roundKeys, size_t sizeRoundKeys, size_t round);

#endif //AES_ADDROUNDKEY_H
