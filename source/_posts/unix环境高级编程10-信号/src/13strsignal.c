#include <stdio.h>
#include <signal.h>
#include <string.h>

void sig_handler(int signo, siginfo_t *info, void *context){
	printf("Received signal %d\n", signo);
	psiginfo(info, "signal info: ");
}
int main() {
    struct sigaction sa;
    sa.sa_sigaction = sig_handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, NULL);
	
	const char *str = strsignal(SIGUSR2);
	printf("SIGUSR2: %s\n", str);

    // 触发 SIGUSR1 信号
    raise(SIGUSR1);

    return 0;
}

