#include <sys/socket.h>
#include <unistd.h>

#define MAXSLEEP 128

int connect_retry(int domain, int type, int protocol, 
				const struct sockaddr *addr, socklen_t alen){
	int numsec, fd;
	// 尝试使用指数退避算法进行 connect 操作
	for(numsec = 1; numsec <= MAXSLEEP; numsec <<=1){
		if((fd = socket(domain, type, protocol)) < 0)
			return -1;
		if(connect(fd, addr, alen) == 0){
			return fd;
		}
		close(fd);
		// 如果 connect 失败，再次尝试之前延迟。
		if(numsec <= MAXSLEEP/2)
			sleep(numsec);
	}
	return -1;
}
