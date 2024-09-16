#include <stdio.h>
#include <string.h>
#include "pyra.h"
#include <time.h>
#include "openssl/aes.h"

void aes256_encrypt(uint8_t* page, size_t len, const char* _key) 
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
}

int main()
{
    uint8_t* page = aligned_alloc(32, 1024 * 1024 * 4);
    size_t len = 1024 * 1024 * 4;
    char* key = "abababababababababababababababab";
// KEY 1: 624760935, 624760935, -746907545, -465710233, 
// KEY 2: -786163550, 1070799075, -1941974205, -791926161, 
// KEY 3: 1922898138, -449850410, -952489316, 1100854952, 
// KEY 4: -853028017, 1252797171, 2023208660, 110419214,
    struct PYRA* state = aligned_alloc(32, sizeof(struct PYRA));
    pyra2_init(state, 0, key);

    clock_t start, end;
    start = clock();

    //aes256_encrypt(page, len, key);
    pyra2_encrypt(state, page, &len);

    end = clock();
    printf("%fs", (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}