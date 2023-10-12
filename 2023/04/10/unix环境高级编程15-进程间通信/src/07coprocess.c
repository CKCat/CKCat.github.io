#include <unistd.h>
#include <stdio.h>
#include <sys/signal.h>
#include <string.h>
#include <stdlib.h>

static void sig_pipe(int);

int main(){
	int n, fd1[2], fd2[2];
	pid_t pid;
	char line[4096];
	// 注册 SIGPIPE 信号处理函数
	if(signal(SIGPIPE, sig_pipe) == SIG_ERR){
		perror("signal error");
		exit(1);
	}
	// 创建两个管道
	if(pipe(fd1) < 0 || pipe(fd2) < 0){
		perror("pipe error");
		exit(1);
	}
	if((pid = fork()) < 0){
		perror("fork error");
		exit(1);
	}else if(pid > 0){
		// 父进程关闭管道1的读端和管道2的写端
		close(fd1[0]);
		close(fd2[1]);

		while(fgets(line, 4096, stdin) != NULL){
			n = strlen(line);
			if(write(fd1[1], line, n) != n){
				perror("write error");
				exit(1);
			}
			if((n = read(fd2[0], line, 4096)) < 0){
				perror("read error");
				exit(1);
			}
			if(n == 0){
				printf("child closed pipe.\n");
				break;
			}
			line[n] = 0;
			if(fputs(line, stdout) == EOF){
				perror("fputs error");
				exit(1);
			}
		}
		if(ferror(stdin)){
			perror("fgets error on stdin");
			exit(1);
		}
		exit(0);
	}else{
		// 子进程关闭管道1的写端和管道2的读端
		close(fd1[1]);
		close(fd2[0]);
		if(fd1[0] != STDIN_FILENO){
			if(dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO){
				perror("dup2 error to stdin");
				exit(1);
			}
			close(fd1[0]);
		}
		if(fd2[1] != STDOUT_FILENO){
			if(dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO){
				perror("dup2 error to stdout");
				exit(1);
			}
			close(fd2[1]);
		}
		if(execl("./add", "add", (char *)0) < 0){
			perror("execl error");
			exit(1);
		}
	}
	exit(0);
}

static void sig_pipe(int signo){
	printf("SIGPIPE caught\n");
	exit(1);
}
