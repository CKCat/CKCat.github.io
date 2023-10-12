#include <stdio.h>
#include <unistd.h>

int main(){
	int n;
	int fd[2];
	pid_t pid;
	char line[4096];

	if(pipe(fd) < 0){
		perror("pipe error");
		return 1;
	}
	if((pid = fork()) < 0){
		perror("fork error");
		return 1;
	}else if(pid > 0){
		close(fd[0]);
		write(fd[1], "hello wrold\n", 12);
	}else{
		close(fd[1]);
		n = read(fd[0], line, 4096);
		write(STDOUT_FILENO, line, n);
	}
	return 0;
}
