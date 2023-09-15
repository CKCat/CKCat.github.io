#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static void sig_alrm(int signo){
	// 仅仅只是返回中断读操作
}

int main(){
	struct sigaction act, oact;
	int n;
	char line[1024];
	act.sa_handler = sig_alrm;
	act.sa_flags = SA_INTERRUPT; // 由此信号中断的系统调用不自动重启动
	if(sigaction(SIGALRM, &act, &oact) < 0){
		perror("signal(SIGALRM) error");
		return 1;
	}
	alarm(5);
	if((n = read(STDIN_FILENO, line, 1024)) < 0){
		printf("read timeout.\n");
		return 1;
	}
	alarm(0);
	write(STDOUT_FILENO, line, n);
	return 0;
}
