#include "immintrin.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

__m256i derive(uint64_t seed, char* key)
{
    __m256i ret = _mm256_set1_epi64x(0L);
    char* ptr = (char*)&ret;

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            ptr[i] ^= ptr[j] += key[j] ^ seed;
            ptr[(j + i) % 32] += ptr[i] ^= key[i] ^ seed;
        }
    }

    return ret;
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

    free(fmm16);
    free(smm16);

    return (__m256i*)ret;
}

__m128i split16(uint64_t seed)
{
    uint16_t mask;

    for (int i = 0; i < 16; i++)
    {
        mask |= (seed % 2) << i;
        seed ^= seed << 13;
        seed ^= seed >> 7;
        seed ^= seed << 17;
    }

    return _mm_movm_epi8(mask);
}

__m128i mix16a(uint64_t seed)
{
    __m128i lo = _mm_set_epi16(0, 1, 2, 3, 4, 5, 6, 7);
    __m128i hi = _mm_set_epi16(8, 9, 10, 11, 12, 13, 14, 15);

    __m128i sm16 = split16(seed);
    __m128i tmp = _mm_blendv_epi8(hi, lo, sm16);
    lo = _mm_blendv_epi8(lo, hi, sm16);
    hi = tmp;
    lo = _mm_cvtsepi16_epi8(lo);
    hi = _mm_cvtsepi16_epi8(hi);
    lo = _mm_shuffle_epi32(lo, 0b00011011);

    return _mm_or_si128(hi, lo);
}

__m128i invMix16a(uint64_t seed)
{
    __m128i enc = mix16a(seed);
    __m128i dec = _mm_set1_epi64x(0L);
    
    for (int i = 0; i < 16; i++)
        ((uint8_t*)&dec)[((uint8_t*)&enc)[i]] = i;

    return dec;
}

int encrypt(uint8_t* data, size_t* len, char* key)
{
    if (data == NULL || len == NULL)
        return 0;

    __m256i kp[4] = {
        derive(0x0c0b6479, key)[0],
        derive(0x8ea853bc, key)[0],
        derive(0x79b953f7, key)[0],
        derive(0xfe778533, key)[0]
    };

    const uint64_t SLOTS[16] = {
        kp[0][0], kp[0][1], kp[0][2], kp[0][3],
        kp[1][0], kp[1][1], kp[1][2], kp[1][3],
        kp[2][0], kp[2][1], kp[2][2], kp[2][3],
        kp[3][0], kp[3][1], kp[3][2], kp[3][3]
    };

    uint64_t seed = SLOTS[0];

    __m128i* mm16 = mix16(seed);
    __m256i* mm32 = mix32(seed);

    for (int i = (*len / 32); i < (*len / 16); i++)
    {
        int j = i - (*len / 32);

        __m128i m = _mm_load_si128((__m128i*)data + i);
        __m128i c = _mm_load_si128((__m128i*)data + j);
        __m128i sm16 = split16(seed);

        __m128i tmp = _mm_blendv_epi8(m, c, sm16);
        _mm_store_si128((__m128i*)data + j, _mm_blendv_epi8(c, m, sm16));

        for (int i = 0; i < seed % 8; i++)
            tmp = _mm_shuffle_epi8(tmp, *mm16);
        _mm_store_si128((__m128i*)data + i, tmp);

        seed ^= SLOTS[seed % 16];
    }

    /*for (int i = 0; i < (*len / 32); i++)
    {
        __m256i v = _mm256_load_si256((__m256i*)data + i);
        v = _mm256_sub_epi64(v, kp[2]);
        v = _mm256_xor_si256(v, kp[1]);

        for (int i = 0; i < seed % 8; i++)
            v = _mm256_shuffle_epi8(v, *mm32);
        seed ^= SLOTS[seed % 16];

        v = _mm256_add_epi64(v, _mm256_set1_epi64x(seed));
        v = _mm256_add_epi64(v, kp[0]);
        v = _mm256_xor_si256(v, kp[3]);

        for (int i = 0; i < seed % 8; i++)
            v = _mm256_shuffle_epi8(v, *mm32);
        seed ^= SLOTS[seed % 16];

        _mm256_store_si256((__m256i*)data + i, v);
    }*/

    free(mm16);
    free(mm32);

    return 1;
}