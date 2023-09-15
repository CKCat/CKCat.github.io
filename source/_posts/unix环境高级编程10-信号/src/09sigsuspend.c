#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

static void sig_int(int);
void pr_mask(const char *);

int main(){
	sigset_t newmask, oldmask, waitmask;
	pr_mask("program start: ");

	if (signal(SIGINT, sig_int) == SIG_ERR){
		perror("signal(SIGINT) error");
		return 1;
	}
	sigemptyset(&waitmask);
	sigaddset(&waitmask, SIGUSR1);
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGINT);
	// 将 SIGINT 添加到进程信号屏蔽集中
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0){
		perror("signal(SIGINT) error");
		return 1;
	}
	// 打印当前的信号屏蔽字
	pr_mask("in critical region: ");
	// 挂起进程，并将 SIGUSR1 添加到进程的信号屏蔽集中
	if (sigsuspend(&waitmask) != -1){
		perror("sigsuspend error");
		return 1;
	}
    // 当从 sig_int 返回时，则 `sigsuspend` 返回，并且将信号屏蔽字设置为调用它之前的值。
	pr_mask("after return from sigsuspend: ");
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0){
		perror("SIG_SETMASK error");
		return 1;
	}
	pr_mask("program exit: ");
	return 0;
}

static void sig_int(int signo){
	pr_mask("\nin sig_int: ");
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
