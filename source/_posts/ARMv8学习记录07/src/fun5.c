#include<stdio.h>

typedef struct st_data
{
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
    int g;
    int h;
    int i;
    int j;
    int k;
}st_data;

st_data sum(st_data data){
    int sum = data.a + data.b + data.c + data.d + data.e + data.f + data.g + data.h + data.i + data.j + data.k;
    printf("sum = %d\n", sum);
    data.a = sum;
    return data;
}

int main(int argc, char const *argv[])
{
    st_data data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    st_data ret = sum(data);
    printf("st_data.a = %d\n", ret.a);
    return 0;
}
