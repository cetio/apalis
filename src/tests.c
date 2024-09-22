#include <stdio.h>
#include <string.h>
#include "pyra.h"
#include <time.h>
//#include "openssl/aes.h"

/*void _aes256_encrypt(uint8_t* page, size_t len, const char* _key) 
{
    AES_KEY key;
    AES_set_encrypt_key(_key, 256, &key);
    size_t num_blocks = len / AES_BLOCK_SIZE;

    for (size_t i = 0; i + 3 < num_blocks; i += 4) 
    {
        AES_ecb_encrypt(page + (i * AES_BLOCK_SIZE), page + (i * AES_BLOCK_SIZE), &key, AES_ENCRYPT);
        AES_ecb_encrypt(page + ((i + 1) * AES_BLOCK_SIZE), page + ((i + 1) * AES_BLOCK_SIZE), &key, AES_ENCRYPT);
        AES_ecb_encrypt(page + ((i + 2) * AES_BLOCK_SIZE), page + ((i + 2) * AES_BLOCK_SIZE), &key, AES_ENCRYPT);
        AES_ecb_encrypt(page + ((i + 3) * AES_BLOCK_SIZE), page + ((i + 3) * AES_BLOCK_SIZE), &key, AES_ENCRYPT);
    }

    for (size_t i = num_blocks & ~3; i < num_blocks; i++)
        AES_ecb_encrypt(page + (i * AES_BLOCK_SIZE), page + (i * AES_BLOCK_SIZE), &key, AES_ENCRYPT);
}*/

int main()
{
    uint8_t* page = aligned_alloc(32, 1024 * 1024);
    size_t size = 1024 * 1024;
    char* key = "abababababababababababababababab";
    struct PYRA* state = aligned_alloc(32, sizeof(struct PYRA));
    pyra_init(state, 0, key);

    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < 1024; i++)
        pyra_encrypt(state, page, size);

    clock_gettime(CLOCK_MONOTONIC, &end);
    double time = ((end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0) / 1024.0;
    printf(" [%s] \e[4;37mpyra_encrypt\e[0;0m %fms", 
        time <= 1 
        ? "\e[1;32mPASS\e[0;0m" 
        : "\e[1;31mFAIL\e[0;0m", 
    time);

    return 0;
}