#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
	// 命名管道的路径和名称
	const char *fifo_path = "/tmp/myfifo";
	const char *msg = "Hello named pipe!";
	int fd;
	// 创建命名管道
	if(mkfifo(fifo_path, 0666) == -1){
		perror("mkfifo error");
		exit(1);
	}
	printf("Named pipe created at %s.\n", fifo_path);
	// 打开管道进行写入操作
	
	if((fd = open(fifo_path, O_WRONLY)) == -1){
		perror("open error");
		exit(1);
	}
	write(fd, msg, strlen(msg) + 1);
	close(fd);
	return 0;
}
