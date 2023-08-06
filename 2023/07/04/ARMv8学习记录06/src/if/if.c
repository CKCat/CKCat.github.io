#include<stdio.h>

int main(int argc, char const *argv[])
{
    if(argc > 1){
        printf("argc > 1, argc=%d\n", argc);
    }else if(argc == 1){
        printf("argc == 1, argc=%d\n", argc);
    }else{
        printf("argc < 1, argc=%d\n", argc);
    }
    return 0;
}
