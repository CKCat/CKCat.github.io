# include <stdio.h>

int main(int argc, char const *argv[])
{
    unsigned short u2 = 32768;
    unsigned int u4 = 123;
    unsigned long ul4 = 456;
    unsigned long long ul8 =0x8765432198765432;

    short s2 = 32768;
    int i4 = -1;
    long l4 = 0x87654321;
    long long l8 =0x1234567812345678;    
    printf("u2 = %hu\n", u2);
    printf("u4 = %u\n", u4);
    printf("ul4 = %lu\n", ul4);
    printf("ul8 = %llu\n", ul8);
    printf("s2 = %hd\n", s2);
    printf("i4 = %d\n", i4);
    printf("l4 = %ld\n", l4);
    printf("l8 = %lld\n", l8); 

    return 0;
}