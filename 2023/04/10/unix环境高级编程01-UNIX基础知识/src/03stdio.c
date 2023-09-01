
#include <stdio.h>  /* standard input/output library functions */
#include <errno.h>  /* for definition of errno */
#include <stdarg.h> /* ISO C variable aruments */
#include <unistd.h> /* 提供了访问操作系统底层功能的接口 */
#include <string.h>
#include <stdlib.h>

#define MAXLINE 4096

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
	int c;
	while((c = getc(stdin)) != EOF){ 
		if(putc(c, stdout) == EOF)
			err_sys("output error");
	}
	if (ferror(stdin))
		err_sys("inputerror");
	return 0;
}
