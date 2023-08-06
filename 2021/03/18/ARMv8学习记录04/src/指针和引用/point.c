#include <stdio.h>


int main(int argc, char const *argv[])
{
    char ary [] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
    int *p1 = (int*)ary;
    char *p2 = (char*)ary;
    short *p3 = (short*)ary;
    long *p4 = (long*)ary;
    long long *p5 = (long long*)ary;
    printf("*p1 = 0x%x\n", *p1);
    printf("*p2 = 0x%x\n", *p2);
    printf("*p3 = 0x%x\n", *p3);
    printf("*p4 = 0x%lx\n", *p4);
    printf("*p5 = 0x%llx\n", *p5);
    p1 += 1;
    p2 += 1;
    p3 += 1;
    p4 += 1;
    p5 += 1;
    printf("ary = %p\n", ary);
    printf("&ary = %p\n", &ary);
    printf("p1 = %p\n", p1);
    printf("p2 = %p\n", p2);
    printf("p3 = %p\n", p3);
    printf("p4 = %p\n", p4);
    printf("p5 = %p\n", p5);

    return 0;
}
