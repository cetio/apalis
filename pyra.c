#include "immintrin.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

__m256i derive(uint64_t seed, char* key)
{
    __m256i ret = _mm256_setzero_si256();
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

__m128i mix8a(uint64_t seed)
{
    uint16_t mask = 0;

    for (int i = 0; i < 16; i++)
    {
        mask |= (seed & 1) << i;
        seed ^= seed << 13;
        seed ^= seed >> 7;
        seed ^= seed << 17;
    }

    return _mm_movm_epi8(mask);
}

__m128i mix16b(uint64_t seed)
{
    // We only want one lane, so use 16-bit values to get a total of 16 values.
    // This has minor complications because we now have zeroes in the vectors.
    __m128i lo = _mm_set_epi16(0, 1, 2, 3, 4, 5, 6, 7);
    __m128i hi = _mm_set_epi16(8, 9, 10, 11, 12, 13, 14, 15);

    // Generate a blend mask to get pseudo-random shuffle mask.
    __m128i pm16 = mix8a(seed);
    __m128i tmp = _mm_blendv_epi8(hi, lo, pm16);
    lo = _mm_blendv_epi8(lo, hi, pm16);
    hi = tmp;
    // Starting with 16-bit values means we have zeroes every even index, by shifting
    // to the left by 1 byte we make it so its easy to interleave hi and lo and remove the zeroes.
    hi = _mm_bslli_si128(hi, 1);

    // Interleave hi and lo.
    return _mm_or_si128(hi, lo);
}

__m256i mix32b(uint64_t seed)
{
    return _mm256_set_m128i(mix16b(seed ^ 0x54ca34d3), mix16b(seed ^ 0x01ab27d3));
}

__m128i invMix16b(uint64_t seed)
{
    __m128i enc = mix16b(seed);
    __m128i dec = _mm_setzero_si128();
    
    // I don't know of a better way to do this, this is fine though.
    for (int i = 0; i < 16; i++)
        ((uint8_t*)&dec)[((uint8_t*)&enc)[i]] = i;

    return dec;
}

__m256i invMix32b(uint64_t seed)
{
    return _mm256_set_m128i(invMix16b(seed ^ 0x54ca34d3), invMix16b(seed ^ 0x01ab27d3));
}

int encrypt(uint8_t* data, size_t* len, char* key)
{
    if (data == NULL || len == NULL)
        return 0;

    const __m256i kp[4] = {
        derive(0x0c0b6479, key),
        derive(0x8ea853bc, key),
        derive(0x79b953f7, key),
        derive(0xfe778533, key)
    };

    const uint64_t SLOTS[16] = {
        kp[0][0], kp[0][1], kp[0][2], kp[0][3],
        kp[1][0], kp[1][1], kp[1][2], kp[1][3],
        kp[2][0], kp[2][1], kp[2][2], kp[2][3],
        kp[3][0], kp[3][1], kp[3][2], kp[3][3]
    };

    uint64_t seed = SLOTS[0];

    __m128i mm16 = mix16b(seed);
    __m256i mm32 = mix32b(seed);

    for (int i = (*len / 32); i < (*len / 16); i++)
    {
        int j = i - (*len / 32);

        __m128i hi = _mm_load_si128((__m128i*)data + i);
        __m128i lo = _mm_load_si128((__m128i*)data + j);
        __m128i pm16 = mix8a(seed);

        for (int i = 0; i < seed % 8; i++)
        {
            lo = _mm_shuffle_epi8(lo, mm16);
            hi = _mm_shuffle_epi8(hi, mm16);
        }

        __m128i tmp = _mm_blendv_epi8(hi, lo, pm16);
        _mm_store_si128((__m128i*)data + j, _mm_blendv_epi8(lo, hi, pm16));
        _mm_store_si128((__m128i*)data + i, tmp);

        seed ^= SLOTS[seed % 16];
    }

    for (int i = 0; i < (*len / 32); i++)
    {
        __m256i v = _mm256_load_si256((__m256i*)data + i);

        v = _mm256_sub_epi64(v, kp[2]);
        v = _mm256_xor_si256(v, kp[1]);

        for (int i = 0; i < seed % 8; i++)
            v = _mm256_shuffle_epi8(v, mm32);
        seed ^= SLOTS[seed % 16];

        v = _mm256_add_epi64(v, _mm256_set1_epi64x(seed));
        v = _mm256_add_epi64(v, kp[0]);
        v = _mm256_xor_si256(v, kp[3]);

        for (int i = 0; i < seed % 8; i++)
            v = _mm256_shuffle_epi8(v, mm32);
        seed ^= SLOTS[seed % 16];

        _mm256_store_si256((__m256i*)data + i, v);
    }

    return 1;
}

int decrypt(uint8_t* data, size_t* len, char* key)
{
    if (data == NULL || len == NULL)
        return 0;

    const __m256i kp[4] = {
        derive(0x0c0b6479, key),
        derive(0x8ea853bc, key),
        derive(0x79b953f7, key),
        derive(0xfe778533, key)
    };

    const uint64_t SLOTS[16] = {
        kp[0][0], kp[0][1], kp[0][2], kp[0][3],
        kp[1][0], kp[1][1], kp[1][2], kp[1][3],
        kp[2][0], kp[2][1], kp[2][2], kp[2][3],
        kp[3][0], kp[3][1], kp[3][2], kp[3][3]
    };

    uint64_t seed = SLOTS[0];

    __m128i mm16 = invMix16b(seed);
    __m256i mm32 = invMix32b(seed);

    for (int i = (*len / 32); i < (*len / 16); i++)
        seed ^= SLOTS[seed % 16];

    for (int i = 0; i < (*len / 32); i++)
    {
        __m256i v = _mm256_load_si256((__m256i*)data + i);

        for (int i = 0; i < seed % 8; i++)
            v = _mm256_shuffle_epi8(v, mm32);

        v = _mm256_xor_si256(v, kp[3]);
        v = _mm256_sub_epi64(v, kp[0]);
        v = _mm256_sub_epi64(v, _mm256_set1_epi64x(seed));

        for (int i = 0; i < seed % 8; i++)
            v = _mm256_shuffle_epi8(v, mm32);

        v = _mm256_xor_si256(v, kp[1]);
        v = _mm256_add_epi64(v, kp[2]);

        _mm256_store_si256((__m256i*)data + i, v);
    }

    seed = SLOTS[0];

    for (int i = (*len / 32); i < (*len / 16); i++)
    {
        int j = i - (*len / 32);

        __m128i hi = _mm_load_si128((__m128i*)data + i);
        __m128i lo = _mm_load_si128((__m128i*)data + j);
        __m128i pm16 = mix8a(seed);

        __m128i tmp = _mm_blendv_epi8(hi, lo, pm16);
        lo = _mm_blendv_epi8(lo, hi, pm16);
        hi = tmp;

        for (int i = 0; i < seed % 8; i++)
        {
            lo = _mm_shuffle_epi8(lo, mm16);
            hi = _mm_shuffle_epi8(hi, mm16);
        }

        _mm_store_si128((__m128i*)data + j, lo);
        _mm_store_si128((__m128i*)data + i, hi);

        seed ^= SLOTS[seed % 16];
    }

    return 1;
}