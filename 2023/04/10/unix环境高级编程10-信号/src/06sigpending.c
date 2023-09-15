#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static void sig_quit(int signo){
	printf("caught SIGQUIT\n");
	// 将 SIGQUIT 改为系统默认动作
	if(signal(SIGQUIT, SIG_DFL) == SIG_ERR){
		perror("can't reset SIGQUIT");
		return;
	}
}

int main(){
	sigset_t newmask, oldmask, pendmask;
	if(signal(SIGQUIT, sig_quit) == SIG_ERR){
		perror("can't catch SIGQUIT");
		return 1;
	}
	// 将 SIGQUIT 添加到进程信号屏蔽集中
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGQUIT);
	if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0){
		perror("SIG_BLOCK error");
		return 1;
	}
	sleep(5); 
	if(sigpending(&pendmask) < 0){
		perror("sigpending error");
		return 1;
	}
	if(sigismember(&pendmask, SIGQUIT))
		printf("\nSIGQUIT pending\n");
	// 恢复之前保存的信号屏蔽集，如果在休眠期间产生了 SIGQUIT 信号，
	// 那么此刻 SIGQUIT 是未决的，但是不再阻塞，在 sigprocmask 返回之前，
	// 它将被递送给进程。
	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0){
		perror("sigprocmask SIG_SETMASK error");
		return 1;
	}
	printf("SIGQUIT unblocked\n");
	sleep(5); // 这里产生 SIGQUIT 信号将按默认方式处理，即终止进程
	return 0;
}
