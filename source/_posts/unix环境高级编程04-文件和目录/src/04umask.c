#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define RWRWRW (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

int main(){
	umask(0);
	if(creat("foo", RWRWRW) < 0){
		perror("creat error for foo.\n");
		exit(1);
	}
	umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (creat("bar", RWRWRW) < 0){
		perror("creat error for bar.\n");
		exit(1);
	}
	return 0;
}
