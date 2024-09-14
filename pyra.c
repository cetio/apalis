#include "immintrin.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

__m256i derive(uint64_t seed, char* key)
{
    char* ret = aligned_alloc(32, 32);
    memset(ret, 32, 0);
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            ret[i] ^= ret[j] += key[j] ^ seed;
            ret[(j + i) % 32] += ret[i] ^= key[i] ^ seed;
        }
    }
    return *(__m256i*)ret;
}

__m128i* mix16(int64_t seed)
{
    __m128i* ret = aligned_alloc(16, 32);
    uint8_t* enc = (uint8_t*)ret;
    uint8_t* dec = ((uint8_t*)ret) + 16;
    memset(dec, 255, 16);

    for (uint8_t j = 0; j < 16; j++)
    {
        seed ^= seed << 13;
        seed ^= seed >> 7;
        seed ^= seed << 17;

        uint8_t k = (uint8_t)(seed % 16);

        // Avoid duplicates as to avoid corruption when shuffling.
        // Reseeding may be faster than incrementing using `++k %= SIZE` because it has more uniform
        // distribution, but also may be slower because of that.
        while (dec[k] != 255)
        {
            seed++;
            seed ^= seed << 13;
            seed ^= seed >> 7;
            seed ^= seed << 17;
            k = (uint8_t)(seed % 16);
        }

        enc[j] = k;
        dec[k] = j;
    }

    return ret;
}

__m256i* mix32(uint64_t seed)
{
    __m128i* ret = aligned_alloc(32, 64);
    __m128i* fmm16 = mix16(seed ^ 0x54ca34d3);
    __m128i* smm16 = mix16(seed ^ 0x01ab27d3);

    ret[0] = fmm16[0];
    ret[1] = smm16[0];
    ret[2] = fmm16[1];
    ret[3] = smm16[1];

    return (__m256i*)ret;
}

__m128i split16(uint64_t seed)
{
    __m128i ret = *(__m128i*)aligned_alloc(16, 16);

    ret[0] = seed % 2 == 0 ? -1 : 0;
    seed ^= seed << 13;
    seed ^= seed >> 7;
    seed ^= seed << 17;
    ret[1] = seed % 2 == 0 ? -1 : 0;

    return ret;
}

int encrypt(uint8_t* data, size_t* length, char* key)
{
    if (data == NULL || length == NULL)
        return 0;

    const __m256i kp0 = derive(0x0c0b6479, key);
    const __m256i kp1 = derive(0x8ea853bc, key);
    const __m256i kp2 = derive(0x79b953f7, key);
    const __m256i kp3 = derive(0xfe778533, key);

    const uint64_t SLOTS[16] = {
        kp0[0], kp0[1], kp0[2], kp0[3],
        kp1[0], kp1[1], kp1[2], kp1[3],
        kp2[0], kp2[1], kp2[2], kp2[3],
        kp3[0], kp3[1], kp3[2], kp3[3]
    };

    uint64_t seed = SLOTS[0];
    seed ^= SLOTS[seed % 16];

    __m128i mm16 = mix16(seed)[0];
    __m256i mm32 = mix32(seed)[0];
    __m128i sm16 = split16(seed);

    
    return 1;
}