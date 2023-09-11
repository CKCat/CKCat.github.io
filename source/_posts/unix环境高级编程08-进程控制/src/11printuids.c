#include <stdio.h>
#include <unistd.h>

int main(){
	printf("RUID = %d, EUID = %d\n", getuid(), geteuid());
	return 0;
}
