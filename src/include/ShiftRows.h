#ifndef AES_SHIFTROWS_H
#define AES_SHIFTROWS_H

#include <stdbool.h>
#include <memory.h>
#include <inttypes.h>
#include <errno.h>
#include <math.h>

errno_t shiftRows(uint8_t *matrix, size_t sizeMatrix, bool inverse);

#endif //AES_SHIFTROWS_H
