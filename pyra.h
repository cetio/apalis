#include "stdint.h"
#include "immintrin.h"

struct State
{
    uint64_t seed;
    __m256i keys[4];
    __m128i phi;
    __m256i tau;
};

void sb128(struct State* state);

void sb256(struct State* state);

void invsb128(struct State* state);

void invsb256(struct State* state);

void init(struct State* state, uint64_t seed, char* key);

int encrypt(struct State* state, uint8_t* data, size_t* len);