#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

extern void TELL_PARENT(pid_t);
extern void TELL_CHILD(pid_t);
extern void WAIT_PARENT(void);
extern void WAIT_CHILD(void);
extern void TELL_WAIT(void);

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len){
	struct flock lock;
	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;

	return fcntl(fd, cmd, &lock);
}

#define	writew_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

static void lockabyte(const char *name, int fd, off_t offset){
	if(writew_lock(fd, offset, SEEK_SET, 1) < 0){
		perror("writew_lock error");
		exit(1);
	}
	printf("%s: got the lock, byte %lld.\n", name, (long long)offset);
}

int main(){
	int fd;
	pid_t pid;

	// 创建一个文件并且写入 2 字节
	if((fd = creat("tmplock", FILE_MODE)) < 0){
		perror("creat error");
		exit(1);
	}
	if(write(fd, "ab", 2) != 2){
		perror("write error");
		exit(1);
	}

	TELL_WAIT();
	if((pid = fork()) < 0){
		perror("fork error");
		exit(1);
	}else if(pid == 0){ // 子进程
		lockabyte("child", fd, 0);
		TELL_PARENT(getppid());
		WAIT_PARENT();
		lockabyte("child", fd, 1);
	}else{ // 父进程
		lockabyte("parent", fd, 1);
		TELL_CHILD(pid);
		WAIT_CHILD();
		lockabyte("parent", fd, 0);
	}
	return 0;
}
