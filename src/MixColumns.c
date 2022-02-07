#include "include/MixColumns.h"

static uint8_t countBinaryDigit(uint64_t n)
{
    uint8_t r = 0;
    while (n)
    {
        ++r;
        n >>= 1;
    }
    return r;
}

static uint8_t rijndaelAdd(uint8_t a, uint8_t b)
{
    return a ^ b;
}

static uint8_t rijndaelMultiply(uint8_t a, uint8_t b)
{
    uint16_t m           = 0x011b;    // 0b0000 0001 0001 1011
    uint8_t  mDigitCount = 9;

    // Calculate the product of a and b (XOR as addition)
    uint16_t aExpand = a, bExpand = b;
    uint16_t product = 0;
    while (bExpand)
    {
        product ^= (bExpand & 0x1) * aExpand;
        bExpand >>= 1;
        aExpand <<= 1;
    }

    uint8_t productDigitCount = countBinaryDigit(product);

    // Check if reduction is needed
    if (productDigitCount >= mDigitCount)
    {
        uint8_t shiftCount = productDigitCount - mDigitCount;
        m <<= shiftCount;

        // Simulate long division
        do
        {
            // If the algorithm does not reach to the end AND the MSB of `product` is 1
            if (productDigitCount - shiftCount >= mDigitCount && (product & (0x1 << (shiftCount + mDigitCount - 1))))
                product ^= m;
            m >>= 1;
        }
        while (shiftCount-- > 0);
        // We need to loop one more time when the digit count of `product` and `m` are the same.
    }

    return (uint8_t) product;
}

static errno_t matrixMultiply(
    const uint8_t *leftMatrix,
    const uint8_t *rightMatrix,
    uint8_t *resultMatrix,
    uint8_t (*add)(uint8_t a, uint8_t b),
    uint8_t (*multiply)(uint8_t a, uint8_t b))
{
    if (leftMatrix == NULL || rightMatrix == NULL || resultMatrix == NULL || add == NULL || multiply == NULL)
        return EFAULT;    // Bad address
    else
    {
        for (size_t i = 0; i < 4; ++i)
            for (size_t j = 0; j < 4; ++j)
            {
                size_t t = i * 4 + j;
                resultMatrix[t] = 0;
                // Dot product for two vectors
                for (size_t k       = 0; k < 4; ++k)
                    resultMatrix[t] = add(resultMatrix[t], multiply(leftMatrix[i * 4 + k], rightMatrix[k * 4 + j]));
            }
        return 0;
    }
}

// External function
errno_t mixColumns(uint8_t *matrix, size_t sizeMatrix, bool inverse)
{
    static const uint8_t M[16]     = {
        0x02, 0x03, 0x01, 0x01,
        0x01, 0x02, 0x03, 0x01,
        0x01, 0x01, 0x02, 0x03,
        0x03, 0x01, 0x01, 0x02
    };
    static const uint8_t M_INV[16] = {
        0x0e, 0x0b, 0x0d, 0x09,
        0x09, 0x0e, 0x0b, 0x0d,
        0x0d, 0x09, 0x0e, 0x0b,
        0x0b, 0x0d, 0x09, 0x0e
    };

    if (matrix == NULL || sizeMatrix != 16)
        return EFAULT;    // Bad address
    if (sizeMatrix < 16)
        return EINVAL;    // Bad length

    errno_t e;
    uint8_t resultMatrix[16];

    if (inverse)
        e = matrixMultiply(M_INV, matrix, resultMatrix, rijndaelAdd, rijndaelMultiply);
    else
        e = matrixMultiply(M, matrix, resultMatrix, rijndaelAdd, rijndaelMultiply);

    if (e)
        return e;
    else
        return memcpy_s(matrix, 16, resultMatrix, 16);
}
