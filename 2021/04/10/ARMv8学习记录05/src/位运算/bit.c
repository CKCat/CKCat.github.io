#include <stdio.h>
int main(int argc, char const *argv[])
{
    unsigned int n = argc;
    printf("argc << 3 = %d\n", argc << 3);  // 逻辑左移
    printf("argc >> 3 = %d\n", argc >> 4);  // 算术右移
    printf("n << 3 = %u\n", n << 7);     // 逻辑左移
    printf("n >> 3 = %u\n", n >> 9);     // 逻辑右移
    printf("~argc = %d\n", ~argc);          // 按位取反（求补码）
    printf("argc & 1 = %d\n", argc & 1);    // 按位与 AND
    printf("argc | 2 = %d\n", argc | 2);    // 按位或 OR
    printf("argc ^ 3 = %d\n", argc ^ 3);    // 异或 XOR

    return 0;
}
