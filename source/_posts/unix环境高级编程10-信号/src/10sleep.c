#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static void sig_alrm(int signo){
	// 什么也不做，仅仅返回唤醒 sigsuspend()
}

unsigned int sleep1(unsigned int seconds){
	struct sigaction newact, oldact;
	sigset_t newmask, oldmask, suspmask;
	unsigned int unslept;
	// 设置信号处理程序
	newact.sa_handler = sig_alrm;
	sigemptyset(&newact.sa_mask);
	newact.sa_flags = 0;
	sigaction(SIGALRM, &newact, &oldact);
	// 将 SIGALRM 添加到信号屏蔽集中
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGALRM);
	sigprocmask(SIG_BLOCK, &newmask, &oldmask);

	alarm(seconds);
	suspmask = oldmask;

	// 确保 SIGALRM 不在信号屏蔽集中
	sigdelset(&suspmask, SIGALRM);
	// 挂起进程等待信号处理程序返回，或者被其他信号中断
	sigsuspend(&suspmask);

	unslept =alarm(0);
	// 恢复原来的 SIGALRM 信号处理程序
	sigaction(SIGALRM, &oldact, NULL);
	// 恢复原来的信号屏蔽集
	sigprocmask(SIG_SETMASK, &oldmask, NULL);
	return unslept;
}

int main(){
	unsigned int unslept = sleep1(5);
	printf("%u\n", unslept);
	return 0;
}
