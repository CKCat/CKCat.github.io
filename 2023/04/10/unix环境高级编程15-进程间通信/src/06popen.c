#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(){
	char line[4096];
	FILE *fpin;
	if((fpin = popen("./myuclc", "r")) == NULL){
		perror("popen error");
		return 1;
	}
	for(;;){
		fputs("prompt> ", stdout);
		fflush(stdout);
		if(fgets(line, 4096, fpin) == NULL)
			break;
		if(fputs(line, stdout) == EOF){
			perror("fputs error to pipe");
			return 1;
		}
	}
	if(pclose(fpin) == -1){
		perror("pclose error");
		return 1;
	}
	putchar('\n');
	return 0;
}	
