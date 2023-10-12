#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

static pid_t *childpid = NULL;	// 指向运行时分配的数组
static int maxfd;				// 最大文件描述符数

extern int open_max();

FILE* popen(const char *cmdstring, const char *type){
	int i;
	int pfd[2];
	pid_t pid;
	FILE *fp;
	// 仅仅只允许 type 为 r 或 w 
	if((type[0] != 'r' && type[0] != 'w') || type[1] != 0){
		errno = EINVAL;
		return NULL;
	}
	if(childpid == NULL){	// 第一次执行时分配数组存放子进程的 pid
		maxfd = open_max();
		if((childpid = calloc(maxfd, sizeof(pid_t))) == NULL)
			return NULL;
	}
	if(pipe(pfd) < 0)
		return NULL;
	if(pfd[0] >= maxfd || pfd[1] >= maxfd){
		close(pfd[0]);
		close(pfd[1]);
		errno = EMFILE;
		return NULL;
	}
	if((pid = fork()) < 0){
		return NULL;
	}else if(pid == 0){		// 子进程
		if(*type == 'r'){	// 写管道重定向到标准输出
			close(pfd[0]);
			if(pfd[1] != STDOUT_FILENO){
				dup2(pfd[1], STDOUT_FILENO);
				close(pfd[1]);
			}
		}else{				// 读管道重定向到标准输入
			close(pfd[1]);
			if(pfd[0] != STDIN_FILENO){
				dup2(pfd[0], STDIN_FILENO);
				close(pfd[0]);
			}
		}
		// 关闭 childpid 中所有文件描述符
		for(i = 0; i <maxfd; i++)
			if(childpid[i] > 0)
				close(i);
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
		_exit(127);
	}
	// 父进程
	if(*type == 'r'){	// 返回读管道对应的文件指针
		close(pfd[1]);
		if((fp = fdopen(pfd[0], type)) == NULL)
			return NULL;
	}else{				//  返回写管道对应的文件指针
		close(pfd[0]);
		if((fp = fdopen(pfd[1], type)) == NULL)
			return NULL;
	}
	childpid[fileno(fp)] = pid;	// 记住此文件描述符的子进程PID。
	return fp;
}

int pclose(FILE *fp){
	int fd, stat;
	pid_t pid;
	if(childpid == NULL){
		errno = EINVAL;
		return -1;		// popen() 没有被调用过
	}
	fd = fileno(fp);
	if(fd >= maxfd){
		errno = EINVAL;
		return -1;		// 非法的文件描述符
	}
	if((pid = childpid[fd]) == 0){
		errno = EINVAL;
		return -1;		// fp 不是被 popen 打开的
	}
	childpid[fd] = 0;
	if(fclose(fp) == EOF)
		return -1;
	while(waitpid(pid, &stat, 0) < 0)
		if (errno != EINTR)
			return -1;	// 从 waitpid() 返回的错误，除了 EINTR 以外
	return stat;		// 返回子进程终止的状态
}
