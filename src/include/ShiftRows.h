#ifndef AES_SHIFTROWS_H
#define AES_SHIFTROWS_H

#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <math.h>

int shiftRows(uint8_t *matrix, size_t sizeMatrix, bool inverse);

#endif //AES_SHIFTROWS_H
