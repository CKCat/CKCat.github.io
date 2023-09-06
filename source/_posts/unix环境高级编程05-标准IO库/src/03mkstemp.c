#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

void make_temp(char* template);

int main(){
	char good_template[] = "/tmp/dirXXXXXX";
	char *bad_template = "/tmp/dirXXXXXX"; // 只读

	printf("trying to create first temp file...\n");
	make_temp(good_template);
	printf("trying to create second temp file...\n");
	make_temp(bad_template);
	return 0;
}

void make_temp(char* template){
	int fd;
	struct stat sbuf;
	if((fd = mkstemp(template)) < 0){
		printf("can't create temp file");
		return;
	}
	printf("temp name = %s\n", template);
	close(fd);
	if(stat(template, &sbuf) < 0){
		if(errno == ENONET)
			printf("file doesn't exist\n");
		else{
			perror("stat failed");
			return;
		}
	}else{
		printf("file exists\n");
		unlink(template);
	}
}
