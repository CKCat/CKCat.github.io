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

int main(int argc, char* argv[]){
	int n;
	char buf[MAXLINE];
	while((n = read(STDIN_FILENO, buf, MAXLINE)) > 0)
		if(write(STDOUT_FILENO, buf, n) != n)
			err_sys("write error");
	if (n < 0)
		err_sys("read error");
	return 0;
}
