#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main(){
	pid_t pid;
	if((pid = fork()) < 0){
		perror("fork error");
		return 1;
	}else if(pid !=0){
		sleep(2);
		exit(2);
	}

	if((pid = fork()) < 0){
		perror("fork error");
		return 1;
	}else if(pid != 0){
		sleep(4);
		abort();
	}

	if((pid = fork()) < 0){
		perror("fork error");
		return 1;
	}else if(pid != 0){
		execl("/bin/dd", "dd", "if=/etc/passwd", "of=/dev/null", NULL);
		exit(7);
	}

	if((pid = fork()) < 0){
		perror("fork error");
		return 1;
	}else if(pid != 0){
		sleep(8);
		exit(0);
	}
	sleep(6);
	kill(getpid(), SIGKILL);
	exit(6);
}
