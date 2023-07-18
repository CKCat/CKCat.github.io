---
title: unix环境高级编程14-高级IO
date: 2023-04-10 17:23:03
tags: unix
category: unix环境高级编程
---

# 高级 I/O

## 非阻塞 I/O

非阻塞 I/O 使我们可以发出 open、read 和 write 这样的 I/O 操作，并使这些操作不会永远阻塞。如果这种操作不能完成，则调用立即出错返回，表示该操作如继续执行将阻塞。

对于一个给定的描述符，有两种为其指定非阻塞 I/O 的方法。

1. 如果调用 open 获得描述符，则可指定 O_NONBLOCK 标志。
2. 对于已经打开的一个描述符，则可调用 fcntl，由该函数打开 O_NONBLOCK 文件状态标志。

例子，非阻塞 write

```cpp
#include "apue.h"
#include <errno.h>
#include <fcntl.h>

char	buf[500000];

int
main(void)
{
	int		ntowrite, nwrite;
	char	*ptr;

	ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
	fprintf(stderr, "read %d bytes\n", ntowrite);

	set_fl(STDOUT_FILENO, O_NONBLOCK);	/* set nonblocking */

	ptr = buf;
	while (ntowrite > 0) {
		errno = 0;
		nwrite = write(STDOUT_FILENO, ptr, ntowrite);
		fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);

		if (nwrite > 0) {
			ptr += nwrite;
			ntowrite -= nwrite;
		}
	}

	clr_fl(STDOUT_FILENO, O_NONBLOCK);	/* clear nonblocking */

	exit(0);
}
```

程序是一个非阻塞 I/O 的实例，它从标准输入读 500 000 字节，并试图将它们写到标准输出上。该程序先将标准输出设置为非阻塞的，然后用 for 循环进行输出，每次 write 调用的结果都在标准错误上打印。

若标准输出是终端，则期望 write 有时返回小于 500 000 的一个数字，有时返回错误。若标准输出是终端，则期望 write 有时返回小于 500 000 的一个数字，有时返回错误。

## 记录锁

记录锁（record locking）的功能是：当第一个进程正在读或修改文件的某个部分时，使用记录锁可以阻止其他进程修改同一文件区。

POSIX.1 标准的基础是 fcntl 方法。图 14-2 列出了各种系统提供的不同形式的记录锁。

| 系统            | 建议性 | 强制性 | fcntl | lockf | flock |
| --------------- | ------ | ------ | ----- | ----- | ----- |
| SUS             | x      |        | x     | XSI   |       |
| XSI             | x      |        | x     | x     | x     |
| FreeBSD 8.0     | x      |        | x     | x     | x     |
| Linux 3.2.0     | x      | x      | x     | x     | x     |
| Mac OS X 10.6.8 | x      |        | x     | x     | x     |
| Solaris 10      | x      | x      | x     | x     | x     |

fcntl 记录锁

```cpp
#include <fcnt1.h>
int fcnt1(int fd, int cmd, .../* struct flock *flockptr */);
```

返回值：若成功，依赖于 cmd，否则，返回 −1

对于记录锁，cmd 是 F_GETLK、F_SETLK 或 F_SETLKW。第三个参数（我们将调用 flockptr）是一个指向 flock 结构的指针。

```cpp
struct flock {
    short l_type; /* F_RDLCK, F_WRLCK, or F_UNLCK */
    short l_whence; /* SEEK_SET, SEEK_CUR, or SEEK_END */
    off_t l_start; /* offset in bytes, relative to l_whence */
    off_t l_len; /* length, in bytes; 0 means lock to EOF */
    pid_t l_pid; /* returned with F_GETLK */
};
```

对 flock 结构说明如下。

- 所希望的锁类型：F_RDLCK（共享读锁）、F_WRLCK（独占性写锁）或 F_UNLCK（解锁一个区域）。
- 要加锁或解锁区域的起始字节偏移量（l_start 和 l_whence）。
- 区域的字节长度（l_len）。
- 进程的 ID（l_pid）持有的锁能阻塞当前进程（仅由 F_GETLK 返回）。

关于加锁或解锁区域的说明还要注意下列几项规则。

- 指定区域起始偏移量的两个元素与 lseek 函数（见 3.6 节）中最后两个参数类似。l_whence 可选用的值是 SEEK_SET、SEEK_CUR 或 SEEK_END。
- 锁可以在当前文件尾端处开始或者越过尾端处开始，但是不能在文件起始位置之前开始。
- 如若 l_len 为 0，则表示锁的范围可以扩展到最大可能偏移量。这意味着不管向该文件中追加写了多少数据，它们都可以处于锁的范围内（不必猜测会有多少字节被追加写到了文件之后），而且起始位置可以是文件中的任意一个位置。
- 为了对整个文件加锁，我们设置 l_start 和 l_whence 指向文件的起始位置，并且指定长度（l_len）为 0。（有多种方法可以指定文件起始处，但常用的方法是将 l_start 指定为 0，l_whence 指定为 SEEK_SET。）

上面提到了两种类型的锁：共享读锁（l_type为L_RDLCK）和独占性写锁（L_WRLCK）。基本规则是：任意多个进程在一个给定的字节上可以有一把共享的读锁，但是在一个给定字节上只能有一个进程有一把独占写锁。进一步而言，如果在一个给定字节上已经有一把或多把读锁，则不能在该字节上再加写锁；如果在一个字节上已经有一把独占性写锁，则不能再对它加任何读锁。在图14-3中示出了这些兼容性规则。

![](unix环境高级编程14-高级IO/不同类型锁彼此之间的兼容性.png)

上面说明的兼容性规则适用于不同进程提出的锁请求，并不适用于单个进程提出的多个锁请求。如果一个进程对一个文件区间已经有了一把锁，后来该进程又企图在同一文件区间再加一把锁，那么新锁将替换已有锁。

加读锁时，该描述符必须是读打开。加写锁时，该描述符必须是写打开。

下面说明一下fcntl函数的3种命令。

F_GETLK 判断由flockptr所描述的锁是否会被另外一把锁所排斥（阻塞）。如果存在一把锁，它阻止创建由flockptr所描述的锁，则该现有锁的信息将重写flockptr指向的信息。如果不存在这种情况，则除了将l_type设置为F_UNLCK之外， flockptr所指向结构中的其他信息保持不变。

F_SETLK 设置由 flockptr 所描述的锁。如果我们试图获得一把读锁（l_type 为F_RDLCK）或写锁（l_type为F_WRLCK），而兼容性规则阻止系统给我们这把锁，那么fcntl会立即出错返回，此时errno设置为EACCES或EAGAIN。此命令也用来清除由flockptr指定的锁（l_type为F_UNLCK）。

F_SETLKW这个命令是F_SETLK的阻塞版本（命令名中的W表示等待（wait））。如果所请求的读锁或写锁因另一个进程当前已经对所请求区域的某部分进行了加锁而不能被授予，那么调用进程会被置为休眠。如果请求创建的锁已经可用，或者休眠由信号中断，则该进程被唤醒。

应当了解，用F_GETLK测试能否建立一把锁，然后用F_SETLK或F_SETLKW企图建立那把锁，这两者不是一个原子操作。因此不能保证在这两次fcntl调用之间不会有另一个进程插入并建立一把相同的锁。如果不希望在等待锁变为可用时产生阻塞，就必须处理由F_SETLK返回的可能的出错。

例子，加锁或解锁一个文件区域的函数。
```cpp
#include "apue.h"
#include <fcntl.h>

int
lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
	struct flock	lock;

	lock.l_type = type;		/* F_RDLCK, F_WRLCK, F_UNLCK */
	lock.l_start = offset;	/* byte offset, relative to l_whence */
	lock.l_whence = whence;	/* SEEK_SET, SEEK_CUR, SEEK_END */
	lock.l_len = len;		/* #bytes (0 means to EOF) */

	return(fcntl(fd, cmd, &lock));
}

```
因为大多数锁调用是加锁或解锁一个文件区域（命令F_GETLK很少使用），故通常使用下列5个宏中的一个，这5个宏都定义在apue.h中。
```cpp
#define	read_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define	readw_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define	write_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define	writew_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define	un_lock(fd, offset, whence, len) \
			lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))
```
例子，测试一把锁。
```cpp
#include "apue.h"
#include <fcntl.h>

pid_t
lock_test(int fd, int type, off_t offset, int whence, off_t len)
{
	struct flock	lock;

	lock.l_type = type;		/* F_RDLCK or F_WRLCK */
	lock.l_start = offset;	/* byte offset, relative to l_whence */
	lock.l_whence = whence;	/* SEEK_SET, SEEK_CUR, SEEK_END */
	lock.l_len = len;		/* #bytes (0 means to EOF) */

	if (fcntl(fd, F_GETLK, &lock) < 0)
		err_sys("fcntl error");

	if (lock.l_type == F_UNLCK)
		return(0);		/* false, region isn't locked by another proc */
	return(lock.l_pid);	/* true, return pid of lock owner */
}
```
如果存在一把锁，它阻塞由参数指定的锁请求，则此函数返回持有这把现有锁的进程的进程ID，否则此函数返回0。通常用下面两个宏来调用此函数。

```cpp
#define	is_read_lockable(fd, offset, whence, len) \
			(lock_test((fd), F_RDLCK, (offset), (whence), (len)) == 0)
#define	is_write_lockable(fd, offset, whence, len) \
			(lock_test((fd), F_WRLCK, (offset), (whence), (len)) == 0)
```
注意，进程不能使用 lock_test 函数测试它自己是否在文件的某一部分持有一把锁。F_GETLK 命令的定义说明，返回信息指示是否有现有的锁阻止调用进程设置它自己的锁。因为F_SETLK和F_SETLKW命令总是替换调用进程现有的锁（若已存在），所以调用进程决不会阻塞在自己持有的锁上，于是，F_GETLK命令决不会报告调用进程自己持有的锁。

例子，死锁。
如果两个进程相互等待对方持有并且不释放（锁定）的资源时，则这两个进程就处于死锁状态。如果一个进程已经控制了文件中的一个加锁区域，然后它又试图对另一个进程控制的区域加锁，那么它就会休眠，在这种情况下，有发生死锁的可能性。

```cpp
#include "apue.h"
#include <fcntl.h>

static void
lockabyte(const char *name, int fd, off_t offset)
{
	if (writew_lock(fd, offset, SEEK_SET, 1) < 0)
		err_sys("%s: writew_lock error", name);
	printf("%s: got the lock, byte %lld\n", name, (long long)offset);
}

int
main(void)
{
	int		fd;
	pid_t	pid;

	/*
	 * Create a file and write two bytes to it.
	 */
	if ((fd = creat("templock", FILE_MODE)) < 0)
		err_sys("creat error");
	if (write(fd, "ab", 2) != 2)
		err_sys("write error");

	TELL_WAIT();
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {			/* child */
		lockabyte("child", fd, 0);
		TELL_PARENT(getppid());
		WAIT_PARENT();
		lockabyte("child", fd, 1);
	} else {						/* parent */
		lockabyte("parent", fd, 1);
		TELL_CHILD(pid);
		WAIT_CHILD();
		lockabyte("parent", fd, 0);
	}
	exit(0);
}
```
检测到死锁时，内核必须选择一个进程接收出错返回。



关于记录锁的自动继承和释放有3条规则。
1. 锁与进程和文件两者相关联。这有两重含义：第一重很明显，当一个进程终止时，它所建立的锁全部释放；第二重则不太明显，无论一个描述符何时关闭，该进程通过这一描述符引用的文件上的任何一把锁都会释放（这些锁都是该进程设置的）。

2. 由fork产生的子进程不继承父进程所设置的锁。这意味着，若一个进程得到一把锁，然后调用 fork，那么对于父进程获得的锁而言，子进程被视为另一个进程。对于通过 fork 从父进程处继承过来的描述符，子进程需要调用 fcntl 才能获得它自己的锁。这个约束是有道理的，因为锁的作用是阻止多个进程同时写同一个文件。如果子进程通过fork继承父进程的锁，则父进程和子进程就可以同时写同一个文件。

3. 在执行exec后，新程序可以继承原执行程序的锁。但是注意，如果对一个文件描述符设置了执行时关闭标志，那么当作为exec的一部分关闭该文件描述符时，将释放相应文件的所有锁。

例子，在文件整体上加一把写锁。
```cpp
#include <unistd.h>
#include <fcntl.h>

int
lockfile(int fd)
{
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	return(fcntl(fd, F_SETLK, &fl));
}
```

在对相对于文件尾端的字节范围加锁或解锁时需要特别小心。大多数实现按照 l_whence的SEEK_CUR或SEEK_END值，用l_start以及文件当前位置或当前长度得到绝对文件偏移量。但是，常常需要相对于文件的当前长度指定一把锁，但又不能调用fstat来得到当前文件长度，因为我们在该文件上没有锁。

当对文件的一部分加锁时，内核将指定的偏移量变换成绝对文件偏移量。另外，除了指定一个绝对偏移量（SEEK_SET）之外，fcntl还允许我们相对于文件中的某个点指定该偏移量，这个点是指当前偏移量（SEEK_CUR）或文件尾端（SEEK_END）。当前偏移量和文件尾端可能会不断变化，而这种变化又不应影响现有锁的状态，所以内核必须独立于当前文件偏移量或文件尾端而记住锁。

## I/O多路转接

### 函数select和pselect

在所有POSIX兼容的平台上，select函数使我们可以执行I/O多路转接。传给select的参数告诉内核：
- 我们所关心的描述符；
- 对于每个描述符我们所关心的条件（是否想从一个给定的描述符读，是否想写一个给定的描述符，是否关心一个给定描述符的异常条件）；
- 愿意等待多长时间（可以永远等待、等待一个固定的时间或者根本不等待）。

从select返回时，内核告诉我们：
- 已准备好的描述符的总数量；
- 对于读、写或异常这3个条件中的每一个，哪些描述符已准备好。

使用这种返回信息，就可调用相应的I/O函数（一般是read或write），并且确知该函数不会阻塞。

```cpp
#include <sys/select.h>
int select(int maxfdp1, fd_set *restrict readfds, fd_set *restrict writefds, 
    fd_set *restrict exceptfds, struct timeval *restrict tvptr);
```
返回值：准备就绪的描述符数目；若超时，返回0；若出错，返回−1

最后一个参数，它指定愿意等待的时间长度，单位为秒和微秒。有以下3种情况。
- `tvptr == NULL` 永远等待。如果捕捉到一个信号则中断此无限期等待。当所指定的描述符中的一个已准备好或捕捉到一个信号则返回。如果捕捉到一个信号，则select返回-1，errno设置为EINTR。
- `tvptr->tv_sec == 0 && tvptr->tv_usec == 0` 根本不等待。测试所有指定的描述符并立即返回。这是轮询系统找到多个描述符状态而不阻塞select函数的方法。
- `tvptr->tv_sec != 0 || tvptr->tv_usec != 0` 等待指定的秒数和微秒数。当指定的描述符之一已准备好，或当指定的时间值已经超过时立即返回。如果在超时到期时还没有一个描述符准备好，则返回值是 0。与第一种情况一样，这种等待可被捕捉到的信号中断。

中间3个参数readfds、writefds和exceptfds是指向描述符集的指针。这3个描述符集说明了我们关心的可读、可写或处于异常条件的描述符集合。每个描述符集存储在一个fd_set数据类型中。这个数据类型是由实现选择的，它可以为每一个可能的描述符保持一位。我们可以认为它只是一个很大的字节数组。

对于fd_set数据类型，唯一可以进行的处理是：分配一个这种类型的变量，将这种类型的一个变量值赋给同类型的另一个变量，或对这种类型的变量使用下列4个函数中的一个。
```cpp
#include <sys/select.h>
int FD_ISSET(int fd, fd_set *fdset);
void FD_CLR(int fd, fd_set *fdset);
void FD_SET(int fd, fd_set *fdset);
void FD_ZERO(fd_set *fdset);
```
这些接口可实现为宏或函数。调用FD_ZERO将一个fd_set变量的所有位设置为0。要开启描述符集中的一位，可以调用FD_SET。调用 FD_CLR可以清除一位。最后，可以调用FD_ISSET测试描述符集中的一个指定位是否已打开。

select第一个参数maxfdp1的意思是“最大文件描述符编号值加1”。


POSIX.1也定义了一个select的变体，称为pselect。
```cpp
#include <sys/select.h>
int pselect(int maxfdp1, fd_set *restrict readfds, fd_set *restrict writefds, 
    fd_set *restrict exceptfds, const struct timespec *restrict tsptr, 
    const sigset_t *restrict sigmask);
```
返回值：准备就绪的描述符数目；若超时，返回0；若出错，返回−1

除下列几点外，pselect与select相同。
- select的超时值用timeval结构指定，但pselect使用timespec结构。timespec结构以秒和纳秒表示超时值，而非秒和微秒。如果平台支持这样的时间精度，那么timespec就能提供更精准的超时时间。
- pselect的超时值被声明为const，这保证了调用pselect不会改变此值。
- pselect 可使用可选信号屏蔽字。若 sigmask 为 NULL，那么在与信号有关的方面， pselect 的运行状况和 select 相同。否则，sigmask 指向一信号屏蔽字，在调用pselect时，以原子操作的方式安装该信号屏蔽字。在返回时，恢复以前的信号屏蔽字。

## 函数poll
poll函数类似于select，但是程序员接口有所不同。poll函数可用于任何类型的文件描述符。
```cpp
#include <poll.h>
int poll(struct pollfd fdarray[], nfds_t nfds, int timeout);
```
返回值：准备就绪的描述符数目；若超时，返回0；若出错，返回-1

## 异步I/O


### POSIX异步I/O
进行异步I/O之前需要先初始化AIO控制块，调用aio_read函数来进行异步读操作，或调用aio_write函数来进行异步写操作。
```cpp
#include <aio.h>
int aio_read(struct aiocb *aiocb);
int aio_write(struct aiocb *aiocb);
```
两个函数的返回值：若成功，返回0；若出错，返回−1

要想强制所有等待中的异步操作不等待而写入持久化的存储中，可以设立一个 AIO 控制块并调用aio_fsync函数。
```cpp
#include <aio.h>
int aio_fsync(int op, struct aiocb *aiocb);
```
返回值：若成功，返回0；若出错，返回−1

为了获知一个异步读、写或者同步操作的完成状态，需要调用aio_error函数。
```cpp
#include <aio.h>
int aio_error(const struct aiocb *aiocb);
```
如果异步操作成功，可以调用aio_return函数来获取异步操作的返回值。
```cpp
#include <aio.h>
ssize_t aio_return(const struct aiocb *aiocb);
```

如果在完成了所有事务时，还有异步操作未完成时，可以调用aio_suspend函数来阻塞进程，直到操作完成。
```cpp
#include <aio.h>
int aio_suspend(const struct aiocb *const list[], int nent, const struct timespec *timeout);
```
返回值：若成功，返回0；若出错，返回−1

当还有我们不想再完成的等待中的异步I/O操作时，可以尝试使用aio_cancel函数来取消它们。

```cpp
#include <aio.h>
int aio_cancel(int fd, struct aiocb *aiocb);
```

还有一个函数也被包含在异步I/O接口当中，尽管它既能以同步的方式来使用，又能以异步的方式来使用，这个函数就是lio_listio。该函数提交一系列由一个AIO控制块列表描述的I/O请求。
```cpp
#include <aio.h>
int lio_listio(int mode, struct aiocb *restrict const list[restrict],
    int nent, struct sigevent *restrict sigev);
```
返回值：若成功，返回0；若出错，返回−1


## 函数readv和writev
readv和writev函数用于在一次函数调用中读、写多个非连续缓冲区。有时也将这两个函数称为散布读（scatter read）和聚集写（gather write）。
```cpp
#include <sys/uio.h>
ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
```
两个函数的返回值：已读或已写的字节数；若出错，返回−1


## 函数readn和writen

## 存储映射I/O

储映射I/O（memory-mapped I/O）能将一个磁盘文件映射到存储空间中的一个缓冲区上，于是，当从缓冲区中取数据时，就相当于读文件中的相应字节。与此类似，将数据存入缓冲区时，相应字节就自动写入文件。

为了使用这种功能，应首先告诉内核将一个给定的文件映射到一个存储区域中。这是由mmap函数实现的。
```cpp
#include <sys/mman.h>
void *mmap(void *addr, size_t len, int prot, int flag, intfd, off_t off);
```
返回值：若成功，返回映射区的起始地址；若出错，返回MAP_FAILED

调用mprotect可以更改一个现有映射的权限。
```cpp
#include <sys/mman.h>
int mprotect(void *addr, size_t len, int prot);
```
返回值：若成功，返回0；若出错，返回-1

如果共享映射中的页已修改，那么可以调用 msync 将该页冲洗到被映射的文件中。msync函数类似于fsync，但作用于存储映射区。
```cpp
#include <sys/mman.h>
int msync(void *addr, size_t len, int flags);
```
返回值：若成功，返回0；若出错，返回-1

当进程终止时，会自动解除存储映射区的映射，或者直接调用munmap函数也可以解除映射区。关闭映射存储区时使用的文件描述符并不解除映射区。
```cpp
#include <sys/mman.h>
int munmap(void *addr, size_t len);
```
返回值：若成功，返回0；若出错，返回−1

