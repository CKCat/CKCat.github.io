#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
	const char *fifo_path = "/tmp/myfifo";
	ssize_t bytes_read;
	char buffer[256];
	int fd;
	if((fd = open(fifo_path, O_RDONLY)) == -1){
		perror("open error");
		exit(1);
	}
	if((bytes_read = read(fd, buffer, sizeof(buffer))) == -1){
		perror("read error");
		exit(1);
	}
	printf("Received data: %s\n", buffer);
	close(fd);
	return 0;
}
