gcc -march=native -o apalis_tests $(find -name "*.c" ! -name "apalis.c") -lssh -lcrypto
./apalis_tests
rm apalis_tests