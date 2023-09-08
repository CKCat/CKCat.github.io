#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "07sig.h"

static void charatatime(char *);

int main(){
	pid_t pid;
	TELL_WAIT();
	if((pid = fork()) < 0){
		perror("fork error");
		return 1;
	}else if(pid == 0){
		WAIT_PARENT();
		charatatime("output from child\n");
	}else{
		charatatime("output from parent\n");
		TELL_CHILD(pid);
	}
	return 0;
}
static void charatatime(char *str){
	char *ptr;
	int c;
	setbuf(stdout, NULL);
	for(ptr = str; (c = *ptr++) != 0;)
		putc(c, stdout);
}
