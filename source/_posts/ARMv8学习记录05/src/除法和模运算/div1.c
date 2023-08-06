#include <stdio.h>

int main(int argc, char const *argv[])
{
    unsigned int n = 4;
    unsigned int j = 8;
    printf("argc / 4 = %u\n", argc / n);
    printf("argc %% 8 = %d\n", argc % j);
    return 0;
}