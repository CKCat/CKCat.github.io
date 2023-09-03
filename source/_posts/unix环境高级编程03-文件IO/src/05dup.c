#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

void my_dup(){
    int fd, new_fd;
    char buf[128];
    const char *data = "hello";
    fd = open("test.txt", O_CREAT | O_RDWR, 0644);
    if (fd == -1) {
        perror("打开文件失败.");
        exit(1);
    }
    // 复制文件描述符
    new_fd = dup(fd);
    if (new_fd == -1) {
        perror("复制文件描述符失败.");
        exit(1);
    }
    // 使用原子操作读写文件
    pwrite(new_fd, data, strlen(data), SEEK_SET);
    pread(fd, buf, 128, SEEK_SET);
    printf("fd = %d, new_fd = %d, buf = %s\n", fd, new_fd, buf);

    close(fd);
    close(new_fd);
    // 删除文件
    unlink("test.txt");
}

void my_dup2(){
    int fd, new_fd;
    char buf[128];
    const char *data = "Hello, world!";
    // 打开文件
    fd = open("example.txt", O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("无法打开文件");
        exit(EXIT_FAILURE);
    }
    // 复制文件描述符到指定的文件描述符
    new_fd = dup2(fd, 100); // 将文件描述符复制到文件描述符100
    // 在新的文件描述符上写入数据
    pwrite(new_fd, data, strlen(data), SEEK_SET);
    pread(fd, buf, 128, SEEK_SET);
    printf("fd = %d, new_fd = %d, buf = %s\n", fd, new_fd, buf);
    // 关闭文件描述符
    close(fd);
    close(new_fd);
    // 删除文件
    unlink("example.txt");
}

int main(int argc, char const *argv[])
{
    my_dup();
    my_dup2();
    return 0;
}
