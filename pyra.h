#include "stdint.h"
#include "immintrin.h"

/// @brief Derives a subkey vector from a given key and derivation seed. 
/// @brief This is is intended to be a 4 step process using alternate seeds.
/// @param seed Derivation seed.
/// @param key Encryption key, this must be 256 bits.
/// @return Subkey vector.
__m256i* derive(uint64_t seed, char* key);

/// @brief Encrypts data of length using the given plaintext key.
/// @param data Data to be encrypted.
/// @param length Length of data in bytes, this will become aligned to block size.
/// @param key Encryption key, this must be 256 bits.
/// @return Operation status code.
int encrypt(uint8_t* data, size_t* length, char* key);