#include <unistd.h>
#include <stdio.h>
#include <malloc.h>

int main(){
	char *ptr;
	size_t size;
	if(chdir("/usr/include/") < 0){
		perror("chdir error");
		return 1;
	}
	ptr = malloc(1024);
	if(getcwd(ptr, 1024) == NULL){
		perror("getcwd error");
		return 1;
	}
	printf("cwd = %s\n", ptr);
	return 0;
}
