#include "md5.h"

int main(int argc, char const *argv[])
{
    MD5_CTX ctx;

    MD5Init(&ctx);
    const char * str = "hello world";
    MD5Update(&ctx, str, strlen(str));
    MD5Final(&ctx);
    char *s = ctx.digest;
    printf("MD5sum: ");
    for (int i = 0; i < 16; i++)
      printf("%02x", *s++);
    printf("\n");
    return 0;
}
