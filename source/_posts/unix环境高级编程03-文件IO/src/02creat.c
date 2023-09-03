#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd; 
    // 创建一个新文件，如果文件已存在，则截断它，只写模式
    fd = creat("hello.txt", 0644);
    if (fd == -1) {
        perror("无法创建文件");
        exit(EXIT_FAILURE);
    }
    printf("成功创建文件\n");
    // 关闭文件
    close(fd);

    // 使用 open 达到同样的效果
    fd = open("hello.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("无法创建文件");
        exit(EXIT_FAILURE);
    }
    printf("成功创建文件\n");
    // 关闭文件
    close(fd);
    // 删除文件
    unlink("hello.txt");
    return 0;
}
