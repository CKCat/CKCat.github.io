#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
	if(open("tmpfile", O_RDWR) < 0){
		perror("open tmpfile error");
		exit(1);
	}
	if(unlink("tmpfile") < 0){
		perror("unlink tmpfile error");
		exit(1);
	}
	printf("file unlinked.\n");
	sleep(15); // 此时文件链接计数为 0 ，但是由于还在被该进程使用，并未被删除。
	printf("done.\n");
	return 0;
}
