#include <stdio.h>

int main(int argc, char const *argv[])
{
    unsigned int n = 3;
    unsigned int j = 6;
    printf("argc / 3 = %u\n", argc / n);
    printf("argc %% 6 = %d\n", argc % j);
    return 0;
}