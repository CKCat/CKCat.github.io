#include<stdio.h>

long long sum1(long long a, long long b, long long c, long long d){
    return a + b + c + d;
}

double sum2(long long a, double b){
    return a + b;
}

int main(int argc, char const *argv[])
{
    long long sum = 0;
    sum = sum1(0x1234567898765432, 123L, 456L, 789L);
    printf("sum1 = %lld\n", sum);

    double dsum = sum2(12L, 3.4);
    printf("sum2 = %lf\n", dsum);

    return 0;
}
