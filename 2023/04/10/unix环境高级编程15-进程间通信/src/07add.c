#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(){
	int n, a, b;
	char line[4096];
	while((n = read(STDIN_FILENO, line, 4096)) > 0){
		line[n] = 0;
		if(sscanf(line, "%d%d", &a, &b) == 2){
			sprintf(line, "%d\n", a + b);
			n = strlen(line);
			if(write(STDOUT_FILENO, line, n) != n){
				perror("write error");
				exit(1);
			}
		}else{
			if(write(STDOUT_FILENO, "invalid args\n", 13) != 13){
				perror("write error");
				exit(1);
			}
		}
	}
	return 0;
}
