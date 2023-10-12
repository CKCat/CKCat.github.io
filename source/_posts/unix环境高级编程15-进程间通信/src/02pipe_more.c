#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define DEF_PAGER "/bin/more"	// 默认的 pager 程序

int main(int argc, char *argv[]){
	int n;
	int fd[2];
	pid_t pid;
	char *pager, *argv0;
	char line[4096];
	FILE *fp;

	if(argc != 2){
		printf("usage: a.out <pathname>");
		return 1;
	}
	if((fp = fopen(argv[1], "r")) == NULL){
		perror("fopen error");
		return 1;
	}
	if(pipe(fd) < 0){
		perror("pipe error");
		return 1;
	}

	if((pid = fork()) < 0){
		perror("fork error");
		return 1;
	}else if(pid > 0){  //父进程
		close(fd[0]);	// 关闭管道的读端
		// 读取 fp 的内容，写入管道的写端
		while(fgets(line, 4096, fp) != NULL){
			n = strlen(line);
			if(write(fd[1], line, n) != n){
				perror("write error to pipe");
				return 1;
			}
		}
		if(ferror(fp)){
			perror("fgets error");
			return 1;
		}
		close(fd[1]); // 关闭管道的写端
		// 等待子进程结束
		if(waitpid(pid, NULL, 0) < 0){
			perror("waitpid error");
			return 1;
		}
		return 0;
	}else{ // 子进程
		close(fd[1]);	// 关闭管道的写端
		// 如果已经具有所希望的值，那么就不应该复制。
		if(fd[0] != STDIN_FILENO){
			if(dup2(fd[0], STDIN_FILENO) != STDIN_FILENO){
				perror("dup2 error to stdin");
				return 1;
			}
			close(fd[0]); // dup2 后关闭管道的读端
		}
		// 获取 execl 函数的参数
		if((pager = getenv("PAGER")) == NULL)
			pager = DEF_PAGER;
		if((argv0 = strrchr(pager, '/')) != NULL)
			argv0++;
		else
			argv0 = pager;
		// 使用 execlp 函数执行 pager 程序
		if(execlp(pager, argv0, (char *)0) < 0){
			perror("execl error");
			return 1;
		}
	}
	return 0;
}
