#include<stdio.h>

int main(int argc, char const *argv[])
{
    int sum = 0;
    int i = 0;
    do{
        sum +=i;
        i++;
        printf("i=%d", i);
    } while (i <= argc);
    printf("sum=%d\n", sum);
    return 0;
}
