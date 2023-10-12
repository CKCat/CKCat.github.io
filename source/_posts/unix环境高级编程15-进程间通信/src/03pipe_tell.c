#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static int pfd1[2], pfd2[2];

void TELL_WAIT(){
	// 在调用 fork 之前创建两个管道
	if(pipe(pfd1) < 0 || pipe(pfd2) < 0){
		perror("pipe error");
		exit(1);
	}
}

void TELL_PARENT(pid_t pid){
	// 子进程向管道写入一个字符 c
	if(write(pfd2[1], "c", 1) != 1){
		perror("write pfd2[1] error");
		exit(1);
	}
}

void WAIT_PARENT(){
	char c;
	// 子进程从管道读取一个字符 p
	if(read(pfd1[0], &c, 1) != 1){
		perror("read pfd[0] error");
		exit(1);
	}
	if(c != 'p'){
		perror("WAIT_PARENT: incorrect data");
		exit(1);
	}
}

void TELL_CHILD(pid_t pid){
	// 父进程向管道写入一个字符 p 
	if(write(pfd1[1], "p", 1) != 1){
		perror("write pfd1[1] error");
		exit(1);
	}
}

void WAIT_CHILD(){
	char c;
	// 父进程从管道读取一个字符 c
	if(read(pfd2[0], &c, 1) != 1){
		perror("read pfd2[0] error");
		exit(1);
	}
	if(c != 'c'){
		perror("WAIT_CHILD: incorrect data");
		exit(1);
	}
}

