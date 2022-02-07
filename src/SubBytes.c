#include "include/SubBytes.h"

#include "include/SBox.h"

errno_t subBytes(uint8_t *matrix, size_t sizeMatrix, bool inverse)
{
    if (!matrix)
        return EFAULT;

    for (size_t i = 0; i < sizeMatrix; ++i)
        matrix[i] = inverse ? S_BOX_INV[matrix[i]] : S_BOX[matrix[i]];

    return 0;
}
