#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
	pid_t pid;
	if((pid = fork()) < 0){
		perror("fork error");
		return 1;
	}else if(pid == 0){
		if(execl("/home/ckcat/testinterp", "testinterp",
				"myarg1", "MY_ARG2", NULL) < 0){
			perror("execl error");
			return 1;
		}
	}
	if(waitpid(pid, NULL, 0) < 0){
		perror("waitpid error");
		return 1;
	}
	return 0;
}
