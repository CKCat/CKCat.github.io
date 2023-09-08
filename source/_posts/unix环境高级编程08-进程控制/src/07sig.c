#include "07sig.h"

static volatile sig_atomic_t sigflag;
static sigset_t newmask, oldmask, zeromask;

static void sig_usr(int signo){
	sigflag = 1;
}

void TELL_WAIT(void){
	if(signal(SIGUSR1, sig_usr) == SIG_ERR){
		perror("signa(SIGUSR1) error");
		exit(1);
	}
	if(signal(SIGUSR2, sig_usr) == SIG_ERR){
		perror("signal(SIGUSR2) error");
		exit(1);
	}
	sigemptyset(&zeromask); //初始化一个空的信号集
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGUSR1); // 将 SIGUSR1 信号添加到信号集中。
	sigaddset(&newmask, SIGUSR2);
	// 将 newmask 添加到当前进程的信号屏蔽集， oldmask 保存原来的信号屏蔽集
	if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0){
		perror("SIG_BLOCK error");
		exit(1);
	}
}

void TELL_PARENT(pid_t pid){
	kill(pid, SIGUSR2);
}

void WAIT_PARENT(){
	while(sigflag == 0){
		sigsuspend(&zeromask); // 挂起进程
	}
		
	sigflag = 0;
	// 恢复原来的信号屏蔽集
	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0){
		perror("SIG_SETMASK error");
		exit(1);
	}
}

void TELL_CHILD(pid_t pid){
	kill(pid, SIGUSR1);
}

void WAIT_CHILD(){
	while(sigflag == 0)
		sigsuspend(&zeromask);
	sigflag = 0;

	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0){
		perror("SIG_SETMASK error");
		exit(1);
	}
}
