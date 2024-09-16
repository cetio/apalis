#include "stdint.h"
#include "immintrin.h"

struct PYRA
{
    uint64_t seed;
    __m256i keys[4];
    __m128i phi;
    __m256i tau;
};

void pyra2_sb128(struct PYRA* s);

void pyra2_sb256(struct PYRA* s);

void pyra2_invsb128(struct PYRA* s);

void pyra2_invsb256(struct PYRA* s);

void pyra2_init(struct PYRA* s, uint64_t seed, char* key);

int pyra2_encrypt(struct PYRA* s, uint8_t* data, size_t* len);