#include <stdio.h>

int main(int argc, char const *argv[])
{
    unsigned int n = 7;
    printf("argc / 7 = %u\n", argc / n);
    printf("argc %% 7 = %d\n", argc % n);
    return 0;
}