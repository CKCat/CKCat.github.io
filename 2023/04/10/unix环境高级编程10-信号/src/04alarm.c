#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>

static jmp_buf env_alrm;

static void sig_alrm2(int signo){
	longjmp(env_alrm, 1);
}

unsigned int sleep2(unsigned int seconds){
	if(signal(SIGALRM, sig_alrm2) == SIG_ERR)
		return seconds;
	if(setjmp(env_alrm) == 0){
		alarm(seconds);
		pause();
	}
	return alarm(0);
}

static void sig_int(int signo){
	int i, j;
	volatile int k;
	printf("\nsig_int starting\n");
	// 下列执行时间大于 sleep 2 的参数值
	for(i = 0; i < 30000000000; i++)
		for(j = 0; j < 1000000; j++)
			k += i +j;
	printf("sig_int finished\n");
}

int main(){
	unsigned int unslept;
	if(signal(SIGINT, sig_int) == SIG_ERR){
		perror("signal(SIGINT) error");
		return 1;
	}
	unslept = sleep2(2);
	printf("sleep2 returned: %u\n", unslept);
	return 0;
}
