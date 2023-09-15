#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

static jmp_buf env_alrm;

static void sig_alrm(int signo){
	longjmp(env_alrm, 1);
}

int main(){
	int n;
	char line[1024];
	if(signal(SIGALRM, sig_alrm) == SIG_ERR){
		perror("signal(SIGALRM) error");
		return 1;
	}
	if(setjmp(env_alrm) != 0){
		perror("read timeout");
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
