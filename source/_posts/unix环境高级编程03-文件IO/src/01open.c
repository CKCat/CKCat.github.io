#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int open_with_mode(const char *path, int oflag, mode_t mode){
    int fd = open(path, oflag, mode);
    if (fd == -1) {
        perror("打开文件失败.");
        return -1;
    }
    return fd;
}
int open_without_mode(const char *path, int oflag){
    int fd = open(path, oflag);
    if (fd == -1) {
        perror("打开文件失败.");
        return -1;
    }
    return fd;
}

void read_content(char* path){
    int fd = open_without_mode(path, O_RDONLY);
    char buf[128];
    ssize_t n = read(fd, buf, 128);
    if (n == -1) {
        perror("读取文件失败.");
        return;
    }
    buf[n] = 0;
    printf("%s\n", buf);
    close(fd);
}

void write_content(int fd, char* content, int len){
    ssize_t n =  write(fd, content, len);
    if (n == -1) {
        perror("写入文件失败.");
        return;
    }
}

int main(int argc, char const *argv[])
{   
    // 打开 hello.txt 文件,不存在则出错
    open_without_mode("hello.txt", O_RDONLY);
    open_without_mode("hello.txt", O_RDWR);
    open_without_mode("hello.txt", O_WRONLY);

    // 创建 hello.txt 文件，
    int fd = open_with_mode("hello.txt", O_RDWR|O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    write_content(fd, "hello", 5);
    close(fd);
    read_content("hello.txt");

    // 追加打开 
    fd = open_without_mode("hello.txt", O_RDWR|O_APPEND);
    write_content(fd, "world", 5);
    close(fd);
    read_content("hello.txt");
    // openat
    fd = open_without_mode("./", O_RDONLY);
    int fd2 = openat(fd, "hello.txt", O_RDONLY);
    printf("fd2 = %d\n", fd2);
    
    // O_TRUNC
    open_without_mode("hello.txt", O_TRUNC);
    read_content("hello.txt");
    // 删除 hello.txt 文件
    int ret = unlink("hello.txt");
    if (ret == -1) {
        perror("删除文件失败.");
        return 1;
    }
    return 0;
}