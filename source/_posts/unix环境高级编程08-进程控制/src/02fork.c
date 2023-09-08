#include <stdio.h>
#include <unistd.h>

// 初始化段
int globvar = 6;
char buf[] = "a write to stdout.\n";

int main(){
	int var;
	pid_t pid;
	var = 88;
	if(write(STDOUT_FILENO, buf, sizeof(buf)-1) != (sizeof(buf)-1)){
		perror("write error");
		return 0;
	}
	printf("before fork.\n");
	if((pid = fork()) < 0){ // 子进程
		perror("fork error");
		return 0;
	}else if(pid == 0){
		globvar++;
		var++;
	}else{// 父进程
		printf("child process id = %d.\n", pid);
		sleep(1);
	}
	printf("pid = %ld, globvar = %d, var = %d.\n", (long)getpid(), globvar, var);
	return 0;
}
