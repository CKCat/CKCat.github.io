#include<stdio.h>

int main(int argc, char const *argv[])
{
    int sum = 0;
    int i = 0;
    while (i <= argc){
        sum +=i;
        i++;
        printf("i=%d", i);
    } 
    printf("sum=%d\n", sum);
    return 0;
}
