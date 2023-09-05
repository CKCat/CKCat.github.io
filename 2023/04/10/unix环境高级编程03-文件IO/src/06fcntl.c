#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

void set_fl(int fd, int flags){
	int val;
	if ((val = fcntl(fd, F_GETFL, 0)) < 0){
		printf("fcntl G_GETFL error.\n");
		exit(1);
	}
	// 开启一个或多个文件状态标志
	val |= flags;
	if(fcntl(fd, F_SETFL, val) < 0){
		printf("fcntl F_SETFL error.\n");
		exit(1);
	}
}

void print_fl(int fd){
	int val;
	if((val = fcntl(fd, F_GETFL, 0)) < 0){
		printf("fcntl error for fd %d\n", fd); 
		exit(1);
	}
	switch (val & O_ACCMODE){ // 必须用屏蔽字 O_ACCMODE 取得访问方式位	
		case O_RDONLY:
			printf("read only");
			break;
		case O_WRONLY:
			printf("write only");
			break;
		case O_RDWR:
			printf("read write");
			break;
		default:
			printf("unkown access mode");
			break;
	}
	if(val & O_APPEND)
		printf(", apend");
	if(val & O_NONBLOCK)
		printf(", nonblocking");
	if(val & O_SYNC)
		printf(", synchronous write");

	#if !defined(_POSIX_C_SOURCE) && defined(O_FSYNC) && (O_FSYNC != OSYNC)
		if(val & O_FSYNC)
			printf(", synchronous write");
	#endif
	putchar('\n');
}

int main(int argc, char* argv[]){
	int fd;
	if(argc != 2){
		printf("usage: a.out <descriptor>");
		return 0;
	}
	fd = atoi(argv[1]);
	print_fl(fd);
	set_fl(fd, O_APPEND);
	print_fl(fd);
	return 0;
}


