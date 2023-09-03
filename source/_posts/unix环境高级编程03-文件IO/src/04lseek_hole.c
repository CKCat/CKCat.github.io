#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

char buf1[] = "abcdefghij";
char buf2[] = "ABCDEFGHIJ";

int main(int argc, char const *argv[]){
    int fd;
    if((fd = creat("file.hole", 0644)) < 0){
        perror("creat error.");
        exit(1);
    }
    if(write(fd, buf1, 10) != 10) { // offset = 10
        perror("buf1 write error.");
        exit(1);
    }

    if(lseek(fd, 16384, SEEK_SET) == -1){ // offset = 16384 = 0x4000
        perror("lseek error.");
        exit(1);
    } 
    if(write(fd, buf2, 10) != 10){ // offset = 16394 
        perror("buf2 write error."); 
        exit(1);
    }
    return 0;
}