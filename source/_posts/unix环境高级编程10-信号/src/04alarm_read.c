#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static void sig_alrm(int signo){
	// 仅仅只是返回中断读操作
}

int main(){
	int n;
	char line[1024];
	if(signal(SIGALRM, sig_alrm) == SIG_ERR){
		perror("signal(SIGALRM) error");
		return 1;
	}
	alarm(5);
	if((n = read(STDIN_FILENO, line, 1024)) < 0){
		perror("read error");
		return 1;
	}
	alarm(0);
	write(STDOUT_FILENO, line, n);
	return 0;
}
