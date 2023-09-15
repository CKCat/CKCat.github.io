#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int sig_int_flag = 0;

void sig_segv(int signo){
    signal(SIGSEGV, sig_segv); // 注册信号处理程序
    sig_int_flag = 1;
    printf("received signal %d, sig_int_flag = %d\n", signo, sig_int_flag);
}

int main(int argc, char const *argv[]){
    signal(SIGSEGV, sig_segv); // 注册信号处理程序防止重置为默认值
    while (sig_int_flag == 0){
        printf("start raise(SIGSEGV).\n");
        raise(SIGSEGV);
        printf("finish raise(SIGSEGV).\n");
        pause(); // pause 函数还没有执行的时候，产生了 SIGSEGV 信号，然后再执行 pause 导致进程被挂起。
        printf("from sig_segv return.\n");
    }
    return 0;
}