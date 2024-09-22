gcc -march=native -o apalis_tests $(find -name "*.c" ! -name "module.c") -lssh -lcrypto
./apalis_tests
rm apalis_tests