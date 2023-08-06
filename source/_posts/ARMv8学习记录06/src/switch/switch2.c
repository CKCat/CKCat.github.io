#include <stdio.h>

int main(int argc, char *argv[])
{

    switch (argc)
    {
    case 0:
        printf("argc = 0, argc=%d\n", argc);
        break;
    case 30:
        printf("argc = 30, argc=%d\n", argc);
        break;
    case 50:
        printf("argc = 50, argc=%d\n", argc);
        break;
    case 80:
        printf("argc = 80, argc=%d\n", argc);
        break;
    case 100:
        printf("argc = 100, argc=%d\n", argc);
        break;
    case 200:
        printf("argc = 200, argc=%d\n", argc);
        break;
    }
    return 0;
}