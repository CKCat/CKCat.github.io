#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define COPYINCR (1024*1024*1024) // 1GB
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main(int argc, char *argv[]){
	int fdin, fdout;
	void *src, *dst;
	size_t copysz;
	struct stat sbuf;
	off_t fsz = 0;
	
	if(argc != 3){
		printf("usage:%s <fromfile> <tofile>", argv[0]);
		return 1;
	}
	if((fdin = open(argv[1], O_RDONLY)) < 0){
		perror("can't open fromfile for reading");
		return 1;
	}
	if((fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) < 0){
		perror("can't write tofile for writing");
		return 1;
	}
	if(fstat(fdin, &sbuf) < 0){
		perror("fstat error");
		return 1;
	}
	if(ftruncate(fdout, sbuf.st_size) < 0){
		perror("ftruncate error");
		return 1;
	}

	while(fsz < sbuf.st_size){
		if((sbuf.st_size - fsz) > COPYINCR)
			copysz = COPYINCR;
		else
			copysz = sbuf.st_size - fsz;
		if((src = mmap(0, copysz, PROT_READ, MAP_SHARED, fdin, fsz)) == MAP_FAILED){
			perror("mmap error for input");
			return 1;
		}
		if((dst = mmap(0, copysz, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, fsz)) == MAP_FAILED){
			perror("mmap error for output");
			return 1;
		}
		memcpy(dst, src, copysz);
		munmap(src, copysz);
		munmap(dst, copysz);
		fsz += copysz;
	}
	return 0;
}
