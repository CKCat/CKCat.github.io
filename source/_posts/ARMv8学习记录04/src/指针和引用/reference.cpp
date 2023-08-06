#include <stdio.h>

void foo(int &ref){
    ref++;
    printf("ref = 0x%x\n", ref);
    printf("&ref = %p\n", &ref);
}

int main(int argc, char const *argv[])
{
    int n = 0x12345678;
    int &ref = n;
    printf("&n = %p\n", &n);
    foo(ref);
    printf("n = 0x%x\n", n);
    return 0;
}
