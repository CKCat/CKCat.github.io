#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

volatile sig_atomic_t quitflag; /* set nonzero by signal handler */

void pr_mask(const char *str);
static void sig_int(int signo){ /* one signal handler for SIGINT and SIGQUIT */
    if (signo == SIGINT){
        printf("\ninterrupt\n");
        if (signal(SIGINT, sig_int) == SIG_ERR){
            perror("signal(SIGINT) error");
            return;
        }
    }
    else if (signo == SIGQUIT) // 仅当捕捉到退出信号时，才唤醒主例程。
        quitflag = 1;
}

int main()
{
    sigset_t newmask, oldmask, zeromask;

    // 捕捉中断信号和退出信号
    if (signal(SIGINT, sig_int) == SIG_ERR){
        perror("signal(SIGINT) error");
        return 1;
    }
    if (signal(SIGQUIT, sig_int) == SIG_ERR){
        perror("signal(SIGQUIT) error");
        return 1;
    }

    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);

    // 将 SIGQUIT 添加到进程信号屏蔽集中
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0){
        perror("SIG_BLOCK error");
        return 1;
    }

    while (quitflag == 0){
        pr_mask("sigsuspend before: ");
        sigsuspend(&zeromask); // 挂起进程，在调用 sigsuspend 之前，
        pr_mask("sigsuspend after: ");
    }
    // 捕获到 SIGQUIT 信号后
    quitflag = 0;
    // 恢复旧的信号屏蔽集
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0){
        perror("SIG_SETMASK error");
        return 1;
    }
    return 0;
}

void pr_mask(const char *str){
	sigset_t sigset;
	int errno_save;
	errno_save = errno;
	if (sigprocmask(0, NULL, &sigset) < 0)
	{
		perror("sigprocmask error");
		return;
	}
	else
	{
		printf("%s", str);
		if (sigismember(&sigset, SIGINT))
			printf(" SIGINT");
		if (sigismember(&sigset, SIGQUIT))
			printf(" SIGQUIT");
		if (sigismember(&sigset, SIGUSR1))
			printf(" SIGUSR1");
		if (sigismember(&sigset, SIGALRM))
			printf(" SIGALRM");
		printf("\n");
	}
	errno = errno_save;
}
