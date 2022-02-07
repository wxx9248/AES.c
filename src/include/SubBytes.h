#ifndef AES_SUBBYTES_H
#define AES_SUBBYTES_H

#include <stddef.h>
#include <errno.h>
#include <inttypes.h>

int subBytes(uint8_t *matrix, size_t sizeMatrix, bool inverse);

#endif //AES_SUBBYTES_H
