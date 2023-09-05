#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]){
    if(lseek(STDIN_FILENO, 0, SEEK_CUR) == -1)
        printf("cannot, seek\n");
    else
        printf("seek ok.\n");
    exit(0);
}