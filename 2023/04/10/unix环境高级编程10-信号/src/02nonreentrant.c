#include <stdio.h>
#include <signal.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void my_alarm(int signo){
	struct passwd *rootptr;
	printf("in signal handler\n");
	if((rootptr = getpwnam("root")) == NULL){
		perror("getpwnam root error");
		exit(1);
	}
	alarm(1);
}

int main(){
	struct passwd *ptr;
	signal(SIGALRM, my_alarm);
	alarm(1);
	for(;;){
		if((ptr = getpwnam("ckcat")) == NULL){
			perror("getpwnam ckcat error");
			exit(1);
		}
		if(strcmp(ptr->pw_name, "ckcat") != 0){
			printf("return value corrupted!pw_name = %s\n", ptr->pw_name);
		}
	}
	return 0;
}
