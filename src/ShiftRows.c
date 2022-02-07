#include "include/ShiftRows.h"

static int modulo(int32_t n, int32_t m)
{
    return n - m * (int) floor((double) n / m);
}

// External function
errno_t shiftRows(uint8_t *matrix, size_t sizeMatrix, bool inverse)
{
    if (matrix == NULL)
        return EFAULT;    // Bad address
    if (sizeMatrix < 16)
        return EINVAL;    // Bad length

    uint8_t t[16];
    int8_t sign = inverse ? -1 : 1;

    for (int32_t i = 0; i < 4; ++i)
        for (int32_t j = 0; j < 4; ++j)
            t[i * 4 + j] = matrix[i * 4 + modulo(j + sign * i, 4)];

    return memcpy_s(matrix, sizeMatrix, t, sizeMatrix);
}
