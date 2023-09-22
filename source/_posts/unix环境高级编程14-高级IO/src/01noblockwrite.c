#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

void clr_fl(int fd, int flags){
	int val;
	if((val = fcntl(fd, F_GETFL, 0)) < 0){
		perror("fcntl F_GETFL error");
		exit(1);
	}
	val &= ~flags; // 清除标志
	if(fcntl(fd, F_SETFL, val) < 0){
		perror("fcntl F_SETFL, error");
		exit(1);
	}
}

void set_fl(int fd, int flags){
	int val;
	if((val = fcntl(fd, F_GETFL, 0)) < 0){
		perror("fcntl F_GETFL error");
		exit(1);
	}
	val |= flags; // 设置标志
	if(fcntl(fd, F_SETFL, val) < 0){
		perror("fcntl F_SETFL, error");
		exit(1);
	}
}

char buf[500000];

int main(){
	int ntowrite, nwrite;
	char *ptr;

	ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
	fprintf(stderr, "read %d bytes\n", ntowrite);

	set_fl(STDOUT_FILENO, O_NONBLOCK); // 设置非阻塞

	ptr = buf;
	while(ntowrite > 0){
		errno = 0;
		nwrite = write(STDOUT_FILENO, ptr, ntowrite);
		fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);

		if(nwrite > 0){
			ptr += nwrite;
			ntowrite -= nwrite;
		}
	}
	clr_fl(STDOUT_FILENO, O_NONBLOCK); //清除非阻塞
	return 0;
}
