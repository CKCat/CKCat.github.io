#include <stdio.h>

void pr_stdio(const char *, FILE *);
int is_unbuffered(FILE *);
int is_linebuffered(FILE *);
int buffer_size(FILE *);

int main(int argc, char *argv[]){
	FILE *fp;
	fputs("enter any character\n", stdout);
	if(getchar() == EOF){
		perror("getchar error");
		return 1;
	}
	fputs("one line to standard error\n", stderr);

	pr_stdio("stdin", stdin);
	pr_stdio("stdout", stdout);
	pr_stdio("stderr", stderr);

	if((fp = fopen("/etc/passwd", "r")) == NULL){
		perror("fopen error");
		return 1;
	}
	if(getc(fp) == EOF){
		perror("getc error");
		return 1;
	}
	pr_stdio("/etc/passwd", fp);
	return 0;
}

void pr_stdio(const char* name, FILE *fp){
	printf("stream = %s,", name);
	if(is_unbuffered(fp))
		printf("unbuffered");
	else if(is_linebuffered(fp))
		printf("line buffered");
	else
		printf("fully buffered");
	printf(", buffer size = %d\n", buffer_size(fp));
}

#if defined(_IO_UNBUFFERED)

int is_unbuffered(FILE *fp){
    return (fp->_flags & _IO_UNBUFFERED);
}

int is_linebuffered(FILE *fp){
    return (fp->_flags & _IO_LINE_BUF);
}

int buffer_size(FILE *fp){
    return (fp->_IO_buf_end - fp->_IO_buf_base);
}

#elif defined(__SNBF)

int is_unbuffered(FILE *fp){
    return (fp->_flags & __SNBF);
}

int is_linebuffered(FILE *fp){
    return (fp->_flags & __SLBF);
}

int buffer_size(FILE *fp){
    return (fp->_bf._size);
}

#elif defined(_IONBF) // ubuntu 20.4 执行下面语句

#ifdef _LP64
#define _flag __pad[4]
#define _ptr __pad[1]
#define _base __pad[2]
#endif

int is_unbuffered(FILE *fp){
    return (fp->_flags & _IONBF);
}

int is_linebuffered(FILE *fp){
    return (fp->_flags & _IOLBF);
}

int buffer_size(FILE *fp){
#ifdef _LP64
    return (fp->_IO_buf_end - fp->_IO_read_base);
#elif
    return (BUFSIZ);
#endif
}
#else

#error unknown stdio implementation!
#endif
