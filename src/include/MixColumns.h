#ifndef AES_MIXCOLUMNS_H
#define AES_MIXCOLUMNS_H

#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>
#include <memory.h>

errno_t mixColumns(uint8_t *matrix, size_t sizeMatrix, bool inverse);

#endif //AES_MIXCOLUMNS_H
