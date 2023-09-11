#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t sid;
    pid_t my_sid;
    // 获取当前进程所属会话的会话ID
    if ((my_sid = getsid(0)) == -1) {
        perror("getsid");
        return 1;
    }
    printf("My session ID (SID): %d\n", (int)my_sid);

    // 创建一个新的会话
    if ((sid = setsid()) == -1) {
        perror("setsid");
        return 1;
    }
    printf("New session ID (SID): %d\n", (int)sid);

    // 再次获取当前进程所属会话的会话ID
    if ((my_sid = getsid(0)) == -1) {
        perror("getsid");
        return 1;
    }
    printf("My session ID (SID): %d\n", (int)my_sid);
    return 0;
}