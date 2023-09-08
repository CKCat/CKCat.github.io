#include <stdio.h>
#include <unistd.h>

// 初始化段
int globvar = 6;

int main(){
	int var;
	pid_t pid;
	var = 88;
	printf("before vfork.\n");
	if((pid = vfork()) < 0){
		perror("vfork error");
		return 1;
	}else if(pid == 0){
		globvar++;
		var++;
		_exit(0); // 子进程退出
	}else{
		printf("child process id = %d\n", pid);
	}
	printf("pid = %ld, globvar = %d, var = %d\n", (long)getpid(), globvar, var);
	return 0;
}
