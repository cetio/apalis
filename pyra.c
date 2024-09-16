// Pyra-2 is a block cipher intended to be an incredibly fast, SIMD-leveraging
// encryption algorithm for situations where you want near-realtime
// access to data on the fly. It is not perfect, and I have no professional
// background in encryption, but it is fairly secure from what I can tell,
// and yields high entropy results with a variety of test inputs.
//
// Initially Pyra-1 was a feistel cipher, but this is no longer the case,
// it is inspired by the pharoahs card shuffle and mixes and blends data
// repeatedly to yield a result that has no discernable pattern even if you were to
// generally have an idea of what the key is or what operations are being performed.
//
// This would likely benefit from some kind of side-channeling prevention, like
// including some random operations as a wrapper of some sort or during intermission of loops,
// since there is a lot of data that could be harvested during both encryption
// and decryption phases of Pyra-2.

#include "immintrin.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

struct State
{
    uint64_t seed;
    __m256i keys[2];
    __m128i phi;
    __m256i tau;
};

void sb128(struct State* state)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        uint8_t j = i;
        uint8_t k = 15 - i;
        
        uint8_t mask = -(state->seed & 1);
        uint8_t r = (j & mask) | (k & ~mask);
        ((uint8_t*)&state->phi)[j] = r;
        ((uint8_t*)&state->phi)[k] = r ^ 15;

        state->seed ^= state->seed << 13;
        state->seed ^= state->seed >> 7;
        state->seed ^= state->seed << 17;
    }
}

void sb256(struct State* state)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        uint8_t j = i;
        uint8_t k = 15 - i;
        
        uint8_t mask = -(state->seed & 1);
        uint8_t r = (j & mask) | (k & ~mask);
        ((uint8_t*)&state->tau)[j] = r;
        ((uint8_t*)&state->tau)[k] = r ^ 15;

        state->seed ^= state->seed << 13;
        state->seed ^= state->seed >> 7;
        state->seed ^= state->seed << 17;
    }

    for (uint8_t i = 15; i < 32; i++)
    {
        uint8_t j = i;
        uint8_t k = 31 - i;
        
        uint8_t mask = -(state->seed & 1);
        uint8_t r = (j & mask) | (k & ~mask);
        ((uint8_t*)&state->tau)[j] = r;
        ((uint8_t*)&state->tau)[k] = r ^ 31;

        state->seed ^= state->seed << 13;
        state->seed ^= state->seed >> 7;
        state->seed ^= state->seed << 17;
    }
}

void invsb128(struct State* state)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        uint8_t j = i;
        uint8_t k = 15 - i;
        
        uint8_t mask = -(state->seed & 1);
        uint8_t r = (k & mask) | (j & ~mask);
        ((uint8_t*)&state->phi)[j] = r;
        ((uint8_t*)&state->phi)[k] = r ^ 15;

        state->seed ^= state->seed << 13;
        state->seed ^= state->seed >> 7;
        state->seed ^= state->seed << 17;
    }
}

void invsb256(struct State* state)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        uint8_t j = i;
        uint8_t k = 15 - i;
        
        uint8_t mask = -(state->seed & 1);
        uint8_t r = (k & mask) | (j & ~mask);
        ((uint8_t*)&state->tau)[j] = r;
        ((uint8_t*)&state->tau)[k] = r ^ 15;

        state->seed ^= state->seed << 13;
        state->seed ^= state->seed >> 7;
        state->seed ^= state->seed << 17;
    }

    for (uint8_t i = 15; i < 32; i++)
    {
        uint8_t j = i;
        uint8_t k = 31 - i;
        
        uint8_t mask = -(state->seed & 1);
        uint8_t r = (k & mask) | (j & ~mask);
        ((uint8_t*)&state->tau)[j] = r;
        ((uint8_t*)&state->tau)[k] = r ^ 31;

        state->seed ^= state->seed << 13;
        state->seed ^= state->seed >> 7;
        state->seed ^= state->seed << 17;
    }
}

void init(struct State* state, uint64_t seed, char* key)
{
    state->seed = seed | 1;
    state->keys[0] = _mm256_loadu_si256((__m256i*)key);
    state->keys[1] = state->keys[0];

    sb256(state);

    for (int i = 0; i < 4; i++)
    {
        state->keys[0] = _mm256_xor_si256(state->keys[0], _mm256_slli_epi64(state->keys[0], 13));
        state->keys[0] = _mm256_xor_si256(state->keys[0], _mm256_srli_epi64(state->keys[0], 7));
        state->keys[0] = _mm256_xor_si256(state->keys[0], _mm256_slli_epi64(state->keys[0], 17));

        state->keys[0] = _mm256_permute2x128_si256(state->keys[0], state->keys[0], 1);
        state->keys[0] = _mm256_shuffle_epi8(state->keys[0], state->tau);
    }

    for (int i = 0; i < 4; i++)
    {
        state->keys[1] = _mm256_xor_si256(state->keys[0], _mm256_slli_epi64(state->keys[1], 13));
        state->keys[1] = _mm256_xor_si256(state->keys[0], _mm256_srli_epi64(state->keys[1], 7));
        state->keys[1] = _mm256_xor_si256(state->keys[0], _mm256_slli_epi64(state->keys[1], 17));

        state->keys[1] = _mm256_permute2x128_si256(state->keys[1], state->keys[1], 1);
        state->keys[1] = _mm256_shuffle_epi8(state->keys[1], state->tau);
    }

    state->keys[0] = _mm256_xor_si256(state->keys[0], state->keys[1]);
    //state->keys[1] = _mm256_permute2x128_si256(state->keys[0], state->keys[1], 3);
}

int encrypt(struct State* state, uint8_t* data, size_t* len)
{
    if (data == NULL || len == NULL)
        return 0;

    for (int i = 0; i < (*len / 32); i++)
    {
        __m256i v = _mm256_loadu_si256((__m256i*)data + i);

        v = _mm256_sub_epi64(v, state->keys[2]);
        v = _mm256_xor_si256(v, state->keys[1]);

        for (int i = 0; i < state->seed % 8; i++)
            v = _mm256_shuffle_epi8(v, state->tau);

        v = _mm256_add_epi64(v, _mm256_set1_epi64x(state->seed));
        v = _mm256_add_epi64(v, state->keys[0]);
        v = _mm256_xor_si256(v, state->keys[3]);

        for (int i = 0; i < state->seed % 8; i++)
            v = _mm256_shuffle_epi8(v, state->tau);

        _mm256_storeu_si256((__m256i*)data + i, v);
    }

    return 1;
}

/*int decrypt(uint8_t* data, size_t* len, char* key)
{
    if (data == NULL || len == NULL)
        return 0;

    const struct state[4] = {
        derive(0x0c0b6479, key),
        derive(0x8ea853bc, key),
        derive(0x79b953f7, key),
        derive(0xfe778533, key)
    };

    const uint64_t SLOTS[16] = {
        state[0][0], state[0][1], state[0][2], state[0][3],
        state[1][0], state[1][1], state[1][2], state[1][3],
        state[2][0], state[2][1], state[2][2], state[2][3],
        state[3][0], state[3][1], state[3][2], state[3][3]
    };

    uint64_t seed = SLOTS[0];

    __m128i mm16 = invsb128(seed);
    __m256i mm32 = invsb256(seed);

    for (int i = (*len / 32); i < (*len / 16); i++)
        seed ^= SLOTS[seed % 16];

    for (int i = 0; i < (*len / 32); i++)
    {
        __m256i v = _mm256_load_si256((__m256i*)data + i);

        for (int i = 0; i < seed % 8; i++)
            v = _mm256_shuffle_epi8(v, mm32);

        v = _mm256_xor_si256(v, state[3]);
        v = _mm256_sub_epi64(v, state[0]);
        v = _mm256_sub_epi64(v, _mm256_set1_epi64x(seed));

        for (int i = 0; i < seed % 8; i++)
            v = _mm256_shuffle_epi8(v, mm32);

        v = _mm256_xor_si256(v, state[1]);
        v = _mm256_add_epi64(v, state[2]);

        _mm256_store_si256((__m256i*)data + i, v);
    }

    seed = SLOTS[0];

    for (int i = (*len / 32); i < (*len / 16); i++)
    {
        int j = i - (*len / 32);

        __m128i hi = _mm_load_si128((__m128i*)data + i);
        __m128i lo = _mm_load_si128((__m128i*)data + j);
        __m128i pm16 = mix128a(seed);

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
}*/