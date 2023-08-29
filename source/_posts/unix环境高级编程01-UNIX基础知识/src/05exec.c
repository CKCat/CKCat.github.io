#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>  /* standard input/output library functions */
#include <errno.h>  /* for definition of errno */
#include <stdarg.h> /* ISO C variable aruments */
#include <unistd.h> /* 提供了访问操作系统底层功能的接口 */
#include <string.h>
#include <stdlib.h>

#define MAXLINE 4094

void err_sys(const char *fmt, ...){
	va_list ap;
	char buf[MAXLINE];

	va_start(ap, fmt);
	vsnprintf(buf, MAXLINE-1, fmt, ap);
	snprintf(buf+strlen(buf), MAXLINE-strlen(buf)-1, ": %s", strerror(errno));
	strcat(buf, "\n");
	fflush(stdout);
	fputs(buf, stderr);
	fflush(NULL);
	va_end(ap);
	exit(1);
}

void err_ret(const char *fmt, ...){
	va_list ap;
	char buf[MAXLINE];

	va_start(ap, fmt);
	vsnprintf(buf, MAXLINE-1, fmt, ap);
	snprintf(buf+strlen(buf), MAXLINE-strlen(buf)-1, ": %s", strerror(errno));
	strcat(buf, "\n");
	fflush(stdout);
	fputs(buf, stderr);
	fflush(NULL);
	va_end(ap);
}

int main(int argc, char* argv[]){
	char buf[MAXLINE];
	pid_t pid;
	int status;
	printf("%%"); // 使用 % 作为命令提示符
	while(fgets(buf, MAXLINE, stdin) != NULL){
		if(buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = 0; // 替换\n为null
		if((pid = fork()) < 0)
			err_sys("fork error.");
		else if(pid == 0){
			// 子进程
			execlp(buf, buf, (char*)0); // 执行命令
			err_ret("couldn't, execute: %s", buf);
			exit(127);
		}
		// 父进程等待子进程终止
		if ((pid = waitpid(pid, &status, 0)) < 0)
			err_sys("waitpid error.");
		printf("%%");
	}
	return 0;
}
