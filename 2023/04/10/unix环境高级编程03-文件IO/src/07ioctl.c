#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        perror("无法获取窗口大小");
        exit(EXIT_FAILURE);
    }
    printf("终端窗口大小：行 %d, 列 %d\n", w.ws_row, w.ws_col);
    return 0;
}
