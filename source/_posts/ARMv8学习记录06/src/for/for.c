#include<stdio.h>

int main(int argc, char const *argv[])
{
    int sum = 0;
    for (int i = 0; i <= argc; i++)
    {
        sum += i;
        printf("i=%d", i);
    }
    printf("sum=%d\n", sum);
    
    return 0;
}
