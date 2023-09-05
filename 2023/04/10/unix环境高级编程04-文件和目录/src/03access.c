#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]){
	if(argc != 2){
		printf("usage: a.out pathname.\n");
		exit(1);
	}
	if(access(argv[1], F_OK) < 0){
		perror("no file");
		exit(1);
	}

	if(access(argv[1], R_OK) < 0)
		perror("access error");
	else
		printf("read access OK\n");

	if (open(argv[1], O_RDONLY) < 0)
		perror("open error");
	else
		printf("open for reading OK\n");
	return 0;
}
