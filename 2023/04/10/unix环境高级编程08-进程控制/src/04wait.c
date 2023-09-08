#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

void pr_exit(int status);

int main(){
	pid_t pid;
	int status;
	if((pid = fork()) < 0){
		perror("fork error");
		return 1;
	}else if(pid == 0)
		exit(7); // 子进程正常终止
	if(wait(&status) != pid){
		perror("wait error");
		return 1;
	}
	pr_exit(status);

	if((pid = fork()) < 0){
		perror("fork error");
		return 1;
	}else if(pid == 0)
		abort(); // 子进程异常终止
	if(wait(&status) != pid){
		perror("wait error");
		return 1;
	}
	pr_exit(status);


	if((pid = fork()) < 0){
		perror("fork error");
		return 1;
	}else if(pid == 0)
		status /= 0; // 子进程除零异常终止
	if(wait(&status) != pid){
		perror("wait error");
		return 1;
	}
	pr_exit(status);
	return 0;
}
void pr_exit(int status){
    if(WIFEXITED(status)) // 若为正常终止子进程返回的状态，则为真。
        printf("normal termination, exit status = %d\n", WEXITSTATUS(status)); // WEXITSTATUS(status) 获取子进程传送给 exit 参数的低8位。
    else if(WIFSIGNALED(status)) //若为异常终止子进程返回的状态，则为真。
        // WTERMSIG(status) 获取子进程终止的信号编号， WCOREDUMP(status) 若已产生终止进程的 core文件，则返回真。
        printf("abnormal termination, signal number = %d%s\n", WTERMSIG(status),
#ifdef WCOREDUMP
    WCOREDUMP(status) ? " (core file generated)": "");
#else
    "");
#endif
    else if(WIFSTOPPED(status)) // 若当前暂停进程的返回的状态，则为真。
    // WSTOPSIG(status) 获取子进程暂停的信号编号。
        printf("child stopped, signal number = %d\n", WSTOPSIG(status));
}

	
