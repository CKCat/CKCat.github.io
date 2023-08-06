#include<stdio.h>

long long sum(int a, int b, long long c, long long d, int e, int g, int h, char i, short j, int k){
    return a + b + c + d + e + g + h + i + j + k;
}

double fsum(float a, double b, int c, short d){
    return a + b + c + d;
}

int main(int argc, char const *argv[])
{
    long long sum_ = sum(1, 2, 3, 4, 5, 6, 7, 'a', 9, 10);
    printf("sum = %lld\n", sum_);
    double fsum_ = fsum(1.1, 2.2, 100, 20);
    printf("fsum = %lf\n", fsum_);
    return 0;
}
