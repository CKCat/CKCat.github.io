#include<stdio.h>

int sum1(int a, int b, int c, int d){
    int sum = a + b + c + d;
    return sum;
}

int sum2(int a, int b, int c, int d, int e, int f){
    int sum = a + b + c + d + e + f;
    return sum;
}

float sum3(int a, char b, float c){
    float sum = a + b + c;
    return sum;
}

int main(int argc, char const *argv[])
{
    int sum = 0;
    sum = sum1(1, 2, 3, argc);
    printf("sum1 = %d\n", sum);
    sum = sum2(11, 22, 32, 44, 55, 66);
    printf("sum2 = %d\n", sum);

    float fsum = sum3(12, 'a', 3.14);
    printf("sum3 = %f\n", fsum);
    return 0;
}
