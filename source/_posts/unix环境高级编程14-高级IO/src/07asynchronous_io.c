#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <aio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BSZ 4096
#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define NBUF 8	

enum rwop{
	UNUSED = 0,
	READ_PENDING = 1,
	WRITE_PENDING = 2
};

struct buf{
	enum rwop op;
	int last;
	struct aiocb aiocb;
	unsigned char data[BSZ];
};

struct buf bufs[NBUF];

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
	int ifd, ofd, i, j, n, err, numop;
	struct stat sbuf;
	const struct aiocb *aiolist[NBUF];
	off_t off = 0;

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
	if(fstat(ifd, &sbuf) < 0){
		perror("fstat failed");
		return 1;
	}

	// 初始化缓冲区，使用8个缓冲区，最多 8 个异步 I/O 请求处于等待状态
	for(i = 0; i < NBUF; i++){
		bufs[i].op = UNUSED;
		bufs[i].aiocb.aio_buf = bufs[i].data;
		bufs[i].aiocb.aio_sigevent.sigev_notify = SIGEV_NONE;
		aiolist[i] = NULL;
	}

	numop = 0;
	for(;;){
		for(i = 0; i < NBUF; i++){
			switch(bufs[i].op){
			case UNUSED:
				// 如果还有未读数据则从输入的文件读取
				if(off < sbuf.st_size){
					bufs[i].op = READ_PENDING;
					bufs[i].aiocb.aio_fildes = ifd;
					bufs[i].aiocb.aio_offset = off;
					off += BSZ;
					if(off >= sbuf.st_size)
						bufs[i].last = 1;
					bufs[i].aiocb.aio_nbytes = BSZ;
					if(aio_read(&bufs[i].aiocb) < 0){
						perror("aio_read failed");
						return 1;
					}
					aiolist[i] = &bufs[i].aiocb;
					numop++;
				}
				break;
			case READ_PENDING:
				// 当 aio_error 返回非 EINPROGRESS 表明操作完成。
				if((err = aio_error(&bufs[i].aiocb)) == EINPROGRESS)
					continue;
				// 当返回值是 0 以外的值时，说明操作失败了。
				if(err != 0){
					if(err == -1){
						perror("aio_error failed");
						return 1;
					}else{
						perror("read failed");
						return 1;
					}
				}
				// 使用 aio_return 获取 I/O 操作的返回值
				if((n = aio_return(&bufs[i].aiocb)) < 0){
					perror("aio_return failed");
					return 1;
				}
				// 读取完成，对缓冲区进行转换并且写入
				if(n != BSZ && !bufs[i].last){
					printf("short read (%d %d)",n, BSZ);
					return 1;
				}
				for(j = 0; j < n; j++)
					bufs[i].data[j] = translate(bufs[i].data[j]);
				bufs[i].op = WRITE_PENDING;
				bufs[i].aiocb.aio_fildes = ofd;
				bufs[i].aiocb.aio_nbytes = n;

				if(aio_write(&bufs[i].aiocb) < 0){
					perror("aio_write failed");
					return 1;
				}
				// 保留我们在aiolist中的位置。
				break;
			case WRITE_PENDING:
				if((err = aio_error(&bufs[i].aiocb)) == EINPROGRESS)
					continue;
				if(err != 0){
					if (err == -1){
						perror("aio_error failed");
						return 1;
					}else{
						perror("write fialed");
						return 1;
					}
				}
				// 写入完成；将缓冲区标记为未使用。
				if((n = aio_return(&bufs[i].aiocb)) < 0){
					perror("aio_return failed");
					return 1;
				}
				if( n != bufs[i].aiocb.aio_nbytes){
					printf("short write (%d %d)", n, BSZ);
					return 1;
				}
				aiolist[i] = NULL;
				bufs[i].op = UNUSED;
				numop--;
				break;
			}
		}
		if(numop == 0){
			if(off >= sbuf.st_size)
					break;
		}else{
			// 当所有异步 I/O 控制块都在使用中时，通过调用 aio_suspend 来等待操作完成。
			if(aio_suspend(aiolist, NBUF, NULL) < 0){
				perror("aio_suspend failed");
				return 1;
			}
		}
	}
	bufs[0].aiocb.aio_fildes = ofd;
	if(aio_fsync(O_SYNC, &bufs[0].aiocb) < 0){
		perror("aio_fsync failed");
		return 1;
	}
	return 0;
}