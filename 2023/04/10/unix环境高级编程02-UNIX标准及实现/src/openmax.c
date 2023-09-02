#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>		/* ISO C variable aruments */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define MAXLINE 1024

static void
err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
	char	buf[MAXLINE];

	vsnprintf(buf, MAXLINE-1, fmt, ap);
	if (errnoflag)
		snprintf(buf+strlen(buf), MAXLINE-strlen(buf)-1, ": %s",
		  strerror(error));
	strcat(buf, "\n");
	fflush(stdout);		/* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(NULL);		/* flushes all stdio output streams */
}

void
err_sys(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	exit(1);
}

#ifdef OPEN_MAX
static long openmax = OPEN_MAX;
#else
static long openmax = 0;
#endif

#define OPEN_MAX_GUESS 256

long open_max(){
	if(openmax == 0){
		errno = 0;
		if((openmax = sysconf(_SC_OPEN_MAX)) < 0){
			if(errno == 0)
				openmax = OPEN_MAX_GUESS;
			else
				err_sys("sysconf error for _SC_OPEN_MAX");
		}
	}
	return openmax;
}

int main(int argc, char* argv[]){
	printf("openmax = %ld\n", open_max());
}
