#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len);
void set_fl(int fd, int flags);

#define	read_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define	write_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

extern void TELL_PARENT(pid_t);
extern void TELL_CHILD(pid_t);
extern void WAIT_PARENT(void);
extern void WAIT_CHILD(void);
extern void	TELL_WAIT(void);

int main(int argc, char *argv[]){
	int fd;
	pid_t pid;
	char buf[5];
	struct stat statbuf;
	if(argc != 2){
		fprintf(stderr, "usage: %s filename\n", argv[0]);
		exit(1);
	}
	if((fd = open(argv[1], O_RDWR |O_CREAT | O_TRUNC, FILE_MODE)) < 0){
		perror("open error");
		exit(1);
	}
	if(write(fd, "abcdef", 6) != 6){
		perror("write error");
		exit(1);
	}
	// 开启设置组ID并且关闭组执行
	if(fstat(fd, &statbuf) < 0){
		perror("fstat error");
		exit(1);
	}
	if(fchmod(fd, (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0){
		perror("fchmod error");
		exit(1);
	}
	
	TELL_WAIT();
	if((pid = fork()) < 0){
		perror("fork error");
		exit(1);
	}else if(pid > 0){
		// 父进程对整个文件进行写锁
		if(write_lock(fd, 0, SEEK_SET, 0) < 0){
			perror("write_lock error");
			exit(1);
		}
		TELL_CHILD(pid);
		if(waitpid(pid, NULL, 0) < 0){
			perror("waitpid error");
			exit(1);
		}
	}else{
		WAIT_PARENT();
		set_fl(fd, O_NONBLOCK);
		// 首先子进程对整个文件进行读锁，看看会有什么错误。
		if(read_lock(fd, 0, SEEK_SET, 0) != -1){
			perror("child: read_lock succeeded");
			exit(1);
		}
		printf("read_lock of already-locked region returns %d\n", errno);
		// 然后试图读强制性锁的文件内容
		if(lseek(fd, 0, SEEK_SET) == -1){
			perror("lseek error");
			exit(1);
		}
		if(read(fd, buf, 2) < 0){
			perror("read failed (mandatory locking works)");
			exit(1);
		}else{
			printf("read OK (no mandatory locking), buf = %2.2s\n", buf);
		}
	}
	return 0;
}

void set_fl(int fd, int flags){
	int val;
	if((val = fcntl(fd, F_GETFL, 0)) < 0){
		perror("fcntl F_GETFL error");
		exit(1);
	}
	val |= flags; // 设置标志
	if(fcntl(fd, F_SETFL, val) < 0){
		perror("fcntl F_SETFL, error");
		exit(1);
	}
}

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len){
	struct flock lock;
	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;

	return fcntl(fd, cmd, &lock);
}
