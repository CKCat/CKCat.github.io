#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void sig_handler(int signo, siginfo_t *info, void *context){
	printf("Received signal %d\n", signo);
	if(info->si_code == SI_QUEUE){
		printf("Value sent with signal: %d\n", info->si_value.sival_int);
	}
}

int main(){
	struct sigaction sa;
	sa.sa_sigaction = sig_handler;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sa, NULL);

	union sigval value;
	value.sival_int = 42;
	if(sigqueue(getpid(), SIGUSR1, value) == -1){
		perror("sigqueue");
		return 1;
	}
	printf("Signal sent to process %d\n", getpid());
	sleep(2);
	return 0;
}
