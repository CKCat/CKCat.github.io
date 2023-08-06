#include <cstdio>

struct st_null{
};

struct st_size{
    int a;
    char b;
    double d;
};

#pragma pack(1)
struct st_pack{
    int a;
    char b;
    double d;
};

struct st_static{
    static int a;
    double d;
};

int main(int argc, char const *argv[])
{
    printf("sizeof(st_null) = %d\n", sizeof(st_null)); // 1
    printf("sizeof(st_size) = %d\n", sizeof(st_size)); // 16
    printf("sizeof(st_pack) = %d\n", sizeof(st_pack)); // 13
    printf("sizeof(st_static) = %d\n", sizeof(st_static)); // 8
    return 0;
}
