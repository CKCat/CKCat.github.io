# include <stdio.h>

int main(int argc, char const *argv[])
{
    char a = 'a';
    printf("a = %c\n", a);
    char str[] = "hello";
    char utf8[] = "你好";
    printf("str = %s\n", str);
    printf("utf8 = %s\n", utf8);
    return 0;
}