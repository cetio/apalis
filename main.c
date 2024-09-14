#include <stdio.h>
#include <string.h>
#include "pyra.h"

int main()
{
    char* str = "Hello World!\n";
    size_t len = strlen(str);
    char* key = "abababababababababababababababab";
    encrypt(str, &len, key);
    printf(str);
    return 0;
}