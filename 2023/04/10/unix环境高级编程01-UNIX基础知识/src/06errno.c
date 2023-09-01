#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(int argc, char* argv[]){
	fprintf(stderr, "EACCES: %s\n", strerror(EACCES));
	errno = ENOENT;
	perror("error meassage");
	return 0;
}
