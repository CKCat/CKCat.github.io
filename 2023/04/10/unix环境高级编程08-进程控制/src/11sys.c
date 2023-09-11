#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	int status;
	if(argc < 2){
		perror("command-line argument required");
		return 1;
	}
	if((status = system(argv[1])) < 0){
		perror("system() error");
		return 1;
	}
	return 0;
}

