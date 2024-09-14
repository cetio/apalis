#include "stdint.h"
#include "immintrin.h"

/// @brief Derives a subkey vector from a given key and derivation seed. 
/// @brief This is is intended to be a 4 step process using alternate seeds.
/// @param seed Derivation seed.
/// @param key Encryption key, this must be 256 bits.
/// @return Subkey vector.
__m256i derive(uint64_t seed, char* key);

/// @brief First phase mix table generation.
/// @param seed Derivation seed.
/// @return Symmetric first phase blending mix table.
__m128i mix8a(uint64_t seed);

/// @brief Second phase __m128i mix table generation.
/// @param seed Derivation seed.
/// @return Asymmetric second phase shuffling mix table.
__m128i mix16b(uint64_t seed);

/// @brief Third phase __m256i mix table generation.
/// @param seed Derivation seed.
/// @return Asymmetric third phase shuffling mix table.
__m256i mix32b(uint64_t seed);

/// @brief Second phase __m128i inverse mix table generation.
/// @param seed Derivation seed.
/// @return Asymmetric second phase shuffling inverse mix table.
__m128i invMix16b(uint64_t seed);

/// @brief Third phase __m256i inverse mix table generation.
/// @param seed Derivation seed.
/// @return Asymmetric third phase shuffling inverse mix table.
__m256i invMix32b(uint64_t seed);

/// @brief Encrypts data of length using the given plaintext key.
/// @param data Data to be encrypted.
/// @param length Length of data in bytes, this will become aligned to block size.
/// @param key Encryption key, this must be 256 bits.
/// @return Operation status code.
int encrypt(uint8_t* data, size_t* length, char* key);

/// @brief Decrypts data of length using the given plaintext key.
/// @param data Data to be decrypted.
/// @param length Length of data in bytes, this will become aligned to block size.
/// @param key Decryption key, this must be 256 bits.
/// @return Operation status code.
int decrypt(uint8_t* data, size_t* length, char* key);