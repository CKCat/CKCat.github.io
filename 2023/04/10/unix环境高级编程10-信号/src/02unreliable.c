#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sig_segv(int signo){
    printf("received signal %d\n", signo);
    int *ptr = NULL;
    *ptr = 42;  // 在信号处理程序调用 signal 的时间窗口发生另一个信号中断
    signal(SIGSEGV, sig_segv); // 注册信号处理程序
}

int main(int argc, char const *argv[]){
    signal(SIGSEGV, sig_segv); // 注册信号处理程序防止重置为默认值
    raise(SIGSEGV);
    while (1)
        pause();
    return 0;
}