#include <stdio.h>
#include <string.h>
#include "pyra.h"

int main()
{
    char* str = malloc(33);
    strcpy(str, "123456789abcdefgABCDEFGHIJKLMNOP");
    size_t len = 32;
    // A234EFGHIJbLMeOP6cC85DKad7fN1g9B
    //char* key = "abababababababababababababababab";
    char* key = "abababababababababababababababab";
    encrypt((uint8_t*)str, &len, key);
    printf("%s\n", str);
    decrypt((uint8_t*)str, &len, key);
    printf("%s\n", str);
    return 0;
}