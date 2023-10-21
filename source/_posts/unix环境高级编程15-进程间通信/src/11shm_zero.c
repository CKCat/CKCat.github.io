#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>

#define NLOOPS	1000
#define SIZE	sizeof(long)	// 共享存储的大小

static int update(long *ptr){
	return (*ptr)++;
}

int main(){
	int fd, i, counter;
	pid_t pid;
	void *area;
	// 打开 /dev/zero 设备
	if((fd = open("/dev/zero", O_RDWR)) < 0){
		perror("open error");
		exit(1);
	}
	//  映射共享存储区
	if((area = mmap(0, SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
		perror("mmap error");
		exit(1);
	}
	close(fd);

	TELL_WAIT();
	// 父进程、子进程交替运行，各自对共享存储映射区中的长整型数加1。
	if((pid = fork()) < 0){
		perror("fork error");
		exit(1);
	}else if(pid > 0){
		for(i = 0; i < NLOOPS; i += 1){
			if((counter = update((long *)area)) != i){
				printf("parent: expected %d, got %d", i, counter);
				exit(1);
			}
			TELL_CHILD(pid);
			WAIT_CHILD();
		}
	}else{
		for(i = 1; i < NLOOPS + 1; i += 2){
			WAIT_PARENT();
			if((counter = update((long *)area)) != i){
				printf("child: expected %d, got %d", i, counter);
				exit(1);
			}
			TELL_PARENT(getppid());
		}
	}
	return 0;
}
