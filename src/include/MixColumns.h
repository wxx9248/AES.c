#ifndef AES_MIXCOLUMNS_H
#define AES_MIXCOLUMNS_H

#include <inttypes.h>
#include <errno.h>
#include <string.h>

int mixColumns(uint8_t *matrix, size_t sizeMatrix, bool inverse);

#endif //AES_MIXCOLUMNS_H
