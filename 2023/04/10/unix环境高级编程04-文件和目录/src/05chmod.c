#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main(){
	struct stat statbuf;
	// 开启设置组ID位，关闭组执行位
	if(stat("foo", &statbuf) < 0){
		perror("stat error for foo");
		exit(1);
	}
	if(chmod("foo",(statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0){
		perror("chmod error for foo");
		exit(1);
	}
	// 设置 mode 为 "rw-r--r--"
	if(chmod("bar", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0){
		perror("chmod error for bar");
		exit(1);
	}
	return 0;
}
