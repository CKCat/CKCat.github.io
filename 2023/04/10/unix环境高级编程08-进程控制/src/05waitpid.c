#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
	pid_t pid;
	if((pid = fork()) < 0){
		perror("fork error");
		return 1;
	}else if(pid == 0){
		if((pid = fork()) < 0){
			perror("fork error");
			return 1;
		}else if(pid > 0){
			printf("first child pid = %ld\n", (long)getpid());
			exit(0); // 退出第一个子进程
		}

		printf("second child pid = %ld\n", (long)getpid());
		// 等待地一个子进程退出，然后第二个子进程的父进程为 init 进程
		sleep(2);
		printf("second child parent pid = %ld\n", (long)getppid());
		printf("second child pid = %ld\n", (long)getpid());
		exit(0);
	}
	// 等待地一个子进程退出
	printf("child pid = %ld, parent pid = %ld\n", (long)pid, (long)getpid());
	if(waitpid(pid, NULL, 0) != pid){
		perror("waitpid error");
		return 1;
	}
	printf("first child finish.\n");
	exit(0);
}
