#include <stdio.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	int i, fd;
	struct stat statbuf;
	struct timespec times[2];
	for(i = 1; i < argc; i++){
		if(stat(argv[i], &statbuf) < 0){
			perror("stat error");
			exit(1);
		}
		if((fd = open(argv[i], O_RDWR|O_TRUNC)) < 0){
			perror("open error");
			exit(1);
		}
		times[0] = statbuf.st_atim;
		times[1] = statbuf.st_mtim;
		if(futimens(fd, times) < 0){
			perror(" futimens error");
			exit(1);
		}
	}
	return 0;
}
