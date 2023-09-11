#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

char *env_init[] = {"USER=unknown", "PATH=/tmp", NULL};

int main(int argc, char *argv[]){
	pid_t pid;
	if((pid = fork()) < 0){
		perror("fork error");
		return 1;
	}else if(pid == 0){
		// 传入路径名和特定的环境 env_init
		if(execle("/home/ckcat/echoall", "echoall", "myarg1",
				"MY_ARG2", (char*)0, env_init) < 0){
			printf("execle error");
			return 1;
		}
	}

	if(waitpid(pid, NULL, 0) < 0){
		printf("wait error");
		return 1;
	}
	if((pid = fork()) < 0){
		perror("fork error");
		return 1;
	}else if(pid == 0){
		// 传入一个文件名，并将调用者环境变量传送给新进程
		if(execlp("echoall", "echoall", "only 1 arg", NULL) < 0){
			perror("execlp error");
			return 1;
		}
	}
	return 0;
}	
