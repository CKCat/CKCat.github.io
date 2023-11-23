#include <sys/socket.h>
#include <unistd.h>

#define MAXSLEEP 128

int connect_retry(int sockfd, const struct sockaddr *addr, socklen_t alen){
	int numsec;
	// 尝试使用指数退避算法进行 connect 操作
	for(numsec = 1; numsec <= MAXSLEEP; numsec <<=1){
		if(connect(sockfd, addr, alen) == 0){
			return 0;
		}
		// 如果 connect 失败，再次尝试之前延迟。
		if(numsec <= MAXSLEEP/2)
			sleep(numsec);
	}
	return -1;
}
