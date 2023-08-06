#include <stdio.h>

int main(int argc, char const *argv[])
{
    int n = 123;
    // 使用 argc 防止编译器优化
    printf("n + argc = %d\n", n + argc); 

    printf("n + (argc++) = %d\n", (argc++)); 
    printf("argc = %d\n", argc); 
    printf("n + (++argc) = %d\n", (++argc)); 
    printf("argc = %d\n", argc);

    argc = argc - 5;
    printf("(argc--) = %d\n", (argc--));
    printf("argc = %d\n", argc); 
    printf("(--argc) = %d\n", (--argc));
    printf("argc = %d\n", argc);

    long long ll = 0x1234567898765432;
    printf("ll + argc = 0x%llx\n", ll + argc);
    printf("argc - ll = 0x%llx\n", argc - ll);

    unsigned int u1 = 0xFFFF;
    printf("u1 + argc = 0x%x\n", u1 + argc);
    printf("argc - u1 = 0x%x\n", argc - u1);

    float pi = 3.14;
    printf("pi + argc = %f\n", pi + argc);
    printf("argc - pi = %f\n", argc - pi);

    return 0;
}