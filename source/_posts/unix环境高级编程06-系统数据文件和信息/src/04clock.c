#include <stdio.h>
#include <time.h>

int main() {
    struct timespec resolution;
    if (clock_getres(CLOCK_REALTIME, &resolution) == 0) {
        printf("系统实时时钟的分辨率：%ld 秒 %ld 纳秒\n", resolution.tv_sec, resolution.tv_nsec);
    } else {
        perror("获取时钟分辨率时出错");
    }
    return 0;
}
