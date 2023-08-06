#include<stdio.h>

typedef struct st_data1{
   int a;
   char b;
   int c;
   int d;
}st_data1;

typedef struct st_data2{
   int a;
   char b;
   int c;
   int d;
   int e;
   int f;
}st_data2;

st_data1 sum1(st_data1 data){
    int sum =  data.a + data.b + data.c + data.d;
    printf("sum = %d\n", sum);
    data.a = sum;
    return data;
}

st_data2 sum2(st_data2 data){
    int sum = data.a + data.b + data.c + data.d + data.e + data.f;
    printf("sum = %d\n", sum);
    data.a = sum;
    return data;
}

int main(int argc, char const *argv[])
{
    st_data1 d1 = {1, 2, 3, 4};
    st_data2 d2 = {12, 23, 34, 45, 56, 78};
    st_data1 st_r1 = sum1(d1);
    printf("a =%d, b=%d, c=%d, d=%d\n", st_r1.a, st_r1.b, st_r1.c, st_r1.d);

    st_data2 st_r2 = sum2(d2);
    printf("a =%d, b=%d, c=%d, d=%d, e=%d, f=%d\n", st_r2.a, st_r2.b, st_r2.c, st_r2.d, st_r2.e, st_r2.f);

    return 0;
}
