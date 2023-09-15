#include <stdio.h>
#include <errno.h>
#include <signal.h>

void pr_mask(const char *str){
	sigset_t sigset;
	int errno_save;
	errno_save = errno;
	if(sigprocmask(0, NULL, &sigset) < 0){
		perror("sigprocmask error");
		return;
	}else{
		printf("%s", str);
		if(sigismember(&sigset, SIGINT))
			printf(" SIGINT");
		if(sigismember(&sigset, SIGQUIT))
			printf(" SIGQUIT");
		if(sigismember(&sigset, SIGUSR1))
			printf(" SIGUSR1");
		if(sigismember(&sigset, SIGALRM))
			printf(" SIGALRM");
		printf("\n");
	}
	errno = errno_save;
}

int main(){
	sigset_t newset, oldset;
	// 初始化信号集
	sigemptyset(&newset);
	sigaddset(&newset, SIGINT);
	sigaddset(&newset, SIGALRM);
	sigaddset(&newset, SIGUSR1);

	// 将 newset 添加到当前的信号屏蔽集中
	sigprocmask(SIG_BLOCK, &newset, &oldset);

	// 打印当前进程的信号屏蔽字
	pr_mask("main mask: ");
	// 恢复原来的信号屏蔽集
	sigprocmask(SIG_SETMASK, &oldset, NULL);
	return 0;
}
