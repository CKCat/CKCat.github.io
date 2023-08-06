#include <stdio.h>
int main(int argc, char const *argv[])
{

    printf("argc * 7 = %d\n", argc * 7);
    printf("argc * 8 = %d\n", argc * 8);
    printf("argc * -3 = %d\n", argc * -3);
    printf("argc * -4 = %d\n", argc * -4);

    unsigned int n = 5;
    printf("n * argc = %u\n", n * argc);
    unsigned long long ll = 0x1234567898765432;
    printf("ll * argc = 0x%llx\n", ll * argc);
    
    printf("argc * 3.14 = %lf\n", argc * 3.14);
    printf("argc * 1.2 = %lf\n", argc * -1.2);
    
    return 0;
}
