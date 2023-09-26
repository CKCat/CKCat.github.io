#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

#define BSZ 4096
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

unsigned char buf[BSZ];

unsigned char translate(unsigned char c){
	if(isalpha(c)){
		if(c >= 'n')
			c -= 13;
		else if (c >= 'a')
			c += 13;
		else if (c >= 'N')
			c -= 13;
		else
			c += 13;
	}
	return c;
}

int main(int argc, char *argv[]){
	int ifd, ofd, i, n, nw;
	if(argc != 3){
		perror("usage: a.out infile outfile");
		return 1;
	}
	if((ifd = open(argv[1], O_RDONLY)) < 0){
		perror("can't open infile");
		return 1;
	}
	if((ofd = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, FILE_MODE)) < 0){
		perror("can't create outfile");
		return 1;
	}

	while((n = read(ifd, buf, BSZ)) > 0){
		for(i = 1; i < n; i++)
			buf[i] = translate(buf[i]);
		if((nw = write(ofd, buf, n)) != n){
			if (nw < 0){
				perror("write failed");
				return 1;
			}
			else{
				printf("short write(%d %d)", nw, n);
				return 1;
			}
		}
	}
	fsync(ofd);
	return 0;
}
