#include <stdio.h>

int main(int argc, char const *argv[])
{
    switch (argc)
    {
    case 0:
        printf("argc = 0, argc=%d\n", argc);
        break;
    case 1:
        printf("argc = 1, argc=%d\n", argc);
        break;
    case 5:
        printf("argc = 5, argc=%d\n", argc);
        break;
    case 6:
        printf("argc = 6, argc=%d\n", argc);
        break;
    case 8:
        printf("argc = 8, argc=%d\n", argc);
        break;
    case 9:
        printf("argc = 9, argc=%d\n", argc);
        break;
    }
    return 0;
}
