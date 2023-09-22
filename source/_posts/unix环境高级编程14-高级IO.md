---
title: unix环境高级编程14-高级IO
date: 2023-04-10 17:23:03
tags: linux 编程
category: unix环境高级编程
---

## 非阻塞 `I/O`

非阻塞 `I/O` 使我们可以发出 `open`、`read` 和 `write` 这样的 `I/O` 操作，并使这些操作不会永远阻塞。如果这种操作不能完成，则调用立即出错返回，表示该操作如继续执行将阻塞。

对于一个给定的描述符，有两种为其指定非阻塞 `I/O` 的方法。

1. 如果调用 `open` 获得描述符，则可指定 `O_NONBLOCK` 标志。
2. 对于已经打开的一个描述符，则可调用 `fcntl`，由该函数打开 `O_NONBLOCK` 文件状态标志。

`POSIX.1` 要求，对于一个非阻塞的描述符如果无数据可读，则 `read` 返回 −1，`errno` 被设置为 `EAGAIN`。

例子， 长的非阻塞 `write`。

```cpp
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

void clr_fl(int fd, int flags){
	int val;
	if((val = fcntl(fd, F_GETFL, 0)) < 0){
		perror("fcntl F_GETFL error");
		exit(1);
	}
	val &= ~flags; // 清除标志
	if(fcntl(fd, F_SETFL, val) < 0){
		perror("fcntl F_SETFL, error");
		exit(1);
	}
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

char buf[500000];

int main(){
	int ntowrite, nwrite;
	char *ptr;

	ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
	fprintf(stderr, "read %d bytes\n", ntowrite);

	set_fl(STDOUT_FILENO, O_NONBLOCK); // 设置非阻塞

	ptr = buf;
	while(ntowrite > 0){
		errno = 0;
		nwrite = write(STDOUT_FILENO, ptr, ntowrite);
		fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);

		if(nwrite > 0){
			ptr += nwrite;
			ntowrite -= nwrite;
		}
	}
	clr_fl(STDOUT_FILENO, O_NONBLOCK); //清除非阻塞
	return 0;
}
```

编译运行：

```bash
$ ls -l file.txt
-rw-rw-r-- 1 ckcat ckcat 1880192 Sep 21 18:20 file.txt
$ ./a.out < file.txt >temp.file
read 500000 bytes
nwrite = 500000, errno = 0

$ ./a.out <file.txt 2>stderr.out
$ cat stderr.out
read 500000 bytes
nwrite = 148493, errno = 0
nwrite = -1, errno = 11
nwrite = -1, errno = 11
nwrite = -1, errno = 11
...
nwrite = 8168, errno = 0
nwrite = 7775, errno = 0
nwrite = 7785, errno = 0
nwrite = 8399, errno = 0
nwrite = 3913, errno = 0
nwrite = 7821, errno = 0
nwrite = 7696, errno = 0
nwrite = 43444, errno = 0
```

程序是一个非阻塞 `I/O` 的实例，它从标准输入读 500 000 字节，并试图将它们写到标准输出上。该程序先将标准输出设置为非阻塞的，然后用 `for` 循环进行输出，每次 `write` 调用的结果都在标准错误上打印。

若标准输出是普通文件，则可以期望 `write` 只执行一次。若标准输出是终端，则期望 `write` 有时返回小于 500 000 的一个数字，有时返回错误。

在该系统（ubuntu 20.04）上，`errno` 值 35 对应的是 `EAGAIN`。

```c
#define	EAGAIN		11	/* Try again */
```

## 记录锁

记录锁（record locking）的功能是：当第一个进程正在读或修改文件的某个部分时，使用记录锁可以阻止其他进程修改同一文件区。

### 1. 历史

对早期 UNIX 系统的其中一个批评是它们不能用来运行数据库系统，其原因是这些系统不支持对部分文件加锁。在 UNIX 系统寻找进入商用计算环境的途径时，很多系统开发小组以各种不同方式增加了对记录锁的支持。

`POSIX.1` 标准的基础是 `fcntl` 方法。下表列出了各种系统提供的不同形式的记录锁。

| 系统            | 建议性 | 强制性 | fcntl | lockf | flock |
| --------------- | ------ | ------ | ----- | ----- | ----- |
| SUS             | •      |        | •     | XSI   |       |
| XSI             | •      |        | •     | •     | •     |
| FreeBSD 8.0     | •      |        | •     | •     | •     |
| Linux 3.2.0     | •      | •      | •     | •     | •     |
| Mac OS X 10.6.8 | •      |        | •     | •     | •     |
| Solaris 10      | •      | •      | •     | •     | •     |

### 2. `fcntl` 记录锁

`fcntl` 函数的原型。

```cpp
#include <fcnt1.h>
int fcnt1(int fd, int cmd, .../* struct flock *flockptr */);
```

返回值：

- 若成功，依赖于 cmd，
- 否则，返回 −1。

其中，参数 `fd` 是文件描述符，`cmd` 是命令参数，第三个参数 `flockptr` 是一个指向 `flock` 结构的指针。

```cpp
struct flock {
    short l_type;   // 类型：F_RDLCK、F_WRLCK 或 F_UNLCK
    short l_whence; // 起始位置：SEEK_SET、SEEK_CUR 或 SEEK_END
    off_t l_start;  // 锁定的起始位置
    off_t l_len;    // 锁定的长度
    pid_t l_pid;    // 持有锁的进程ID（仅用于 F_GETLK 命令）
};
```

对 `flock` 结构说明如下。

- 所希望的锁类型：`F_RDLCK`（共享读锁）、`F_WRLCK`（独占性写锁）或 `F_UNLCK`（解锁一个区域）。
- 要加锁或解锁区域的起始字节偏移量（`l_start` 和 `l_whence`）。
- 区域的字节长度（`l_len`）。
- 进程的 ID（`l_pid`）持有的锁能阻塞当前进程（仅由 `F_GETLK` 返回）。

关于加锁或解锁区域的说明还要注意下列几项规则。

- 指定区域起始偏移量的两个元素与 `lseek` 函数中最后两个参数类似。
- 锁可以在当前文件尾端处开始或者越过尾端处开始，但是不能在文件起始位置之前开始。
- 如若 `l_len` 为 `0`，则表示锁的范围可以扩展到最大可能偏移量。这意味着不管向该文件中追加写了多少数据，它们都可以处于锁的范围内，而且起始位置可以是文件中的任意一个位置。
- 为了对整个文件加锁，我们设置 `l_start` 和 `l_whence` 指向文件的起始位置，并且指定长度 `l_len`为 0。

上面提到了两种类型的锁：共享读锁（`F_RDLCK`）和独占性写锁（`F_WRLCK`）。基本规则是：任意多个进程在一个给定的字节上可以有一把共享的读锁，但是在一个给定字节上只能有一个进程有一把独占写锁。进一步而言，如果在一个给定字节上已经有一把或多把读锁，则不能在该字节上再加写锁；如果在一个字节上已经有一把独占性写锁，则不能再对它加任何读锁。下图中示出了这些兼容性规则。

![](unix环境高级编程14-高级IO/不同类型锁彼此之间的兼容性.png)

**上面说明的兼容性规则适用于不同进程提出的锁请求，并不适用于单个进程提出的多个锁请求。** 如果一个进程对一个文件区间已经有了一把锁，后来该进程又企图在同一文件区间再加一把锁，那么新锁将替换已有锁。

加读锁时，该描述符必须是读打开。加写锁时，该描述符必须是写打开。

常见的命令参数包括：

- `F_GETLK`：检查特定锁定是否已经被其他锁定所阻塞，如果存在一个锁会阻止我们创建锁定，则该已存在的锁的信息会覆盖 `flockptr` 所指向的信息。如果不存在阻止我们创建锁定的锁，则除了将 `l_type` 成员设置为 `F_UNLCK` 之外，`flockptr` 指向的结构保持不变。
- `F_SETLK`：设置锁定，如果无法获取锁定则立即返回，此时 `errno` 设置为 `EAGAIN`。
- `F_SETLKW`：这个命令是 `F_SETLK` 的阻塞版本，如果无法获取锁定，则调用进程进入睡眠状态等待直到锁定可用。进程会在以下情况下被唤醒：锁定变得可用，或者被信号中断。

应当了解，用 `F_GETLK` 测试能否建立一把锁，然后用 `F_SETLK` 或 `F_SETLKW` 企图建立那把锁，这两者不是一个原子操作。因此不能保证在这两次 `fcntl` 调用之间不会有另一个进程插入并建立一把相同的锁。如果不希望在等待锁变为可用时产生阻塞，就必须处理由 `F_SETLK` 返回的可能的出错。

在设置或释放文件上的一把锁时，系统按要求组合或分裂相邻区。如下图所示，对第 150 字节解锁，则内核维护两个锁，再次加锁，则把 3 个相邻的加锁取合并。

![](unix环境高级编程14-高级IO/文件字节范围锁.png)

例子，加锁或解锁一个文件区域的函数。

```cpp
#include <fcntl.h>

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len){
	struct flock lock;
	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;

	return fcntl(fd, cmd, &lock);
}
```

因为大多数锁调用是加锁或解锁一个文件区域（命令 `F_GETLK` 很少使用），故通常使用下列 5 个宏中的一个。

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
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len){
	struct flock lock;
	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;

	if(fcntl(fd, F_GETLK, &lock) < 0){
		perror("fcntl error");
		exit(1);
	}
	if(lock.l_type == F_UNLCK)
		return 0;
	return lock.l_pid;
}
```

如果存在一把锁，它阻塞由参数指定的锁请求，则此函数返回持有这把现有锁的进程的进程 ID，否则此函数返回 0。通常用下面两个宏来调用此函数。

```cpp
#define	is_read_lockable(fd, offset, whence, len) \
			(lock_test((fd), F_RDLCK, (offset), (whence), (len)) == 0)
#define	is_write_lockable(fd, offset, whence, len) \
			(lock_test((fd), F_WRLCK, (offset), (whence), (len)) == 0)
```

注意，进程不能使用 `lock_test` 函数测试它自己是否在文件的某一部分持有一把锁。`F_GETLK` 命令的定义说明，返回的信息适用于已经存在的锁，这些锁会阻止我们创建自己的锁。由于 `F_SETLK` 和 `F_SETLKW` 命令总是替换进程的现有锁（如果存在），我们永远无法在自己的锁上阻塞；因此，`F_GETLK` 命令将永远不会报告我们自己的锁。

例子，死锁。

如果两个进程相互等待对方持有并且不释放的资源时，则这两个进程就处于死锁状态。如果一个进程已经控制了文件中的一个加锁区域，然后它又试图对另一个进程控制的区域加锁，那么它就会休眠，在这种情况下，有发生死锁的可能性。

```cpp
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

extern void TELL_PARENT(pid_t);
extern void TELL_CHILD(pid_t);
extern void WAIT_PARENT(void);
extern void WAIT_CHILD(void);
extern void	TELL_WAIT(void);

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
```

编译运行：

```bash
$ gcc 04deadlock.c tellwait.c
$ ./a.out
parent: got the lock, byte 1.
child: got the lock, byte 0.
writew_lock error: Resource deadlock avoided
child: got the lock, byte 1.
```

子进程对第 0 字节加锁，父进程对第 1 字节加锁。然后，它们中的每一个又试图对对方已经加锁的字节加锁，此时就会出现死锁。

检测到死锁时，内核必须选择一个进程接收出错返回，不同的系统实现不同。

### 3. 锁的隐含继承和释放

关于记录锁的自动继承和释放有 3 条规则。

1. 锁与进程和文件两者相关联。这有两重含义：第一重很明显，当一个进程终止时，它所建立的锁全部释放；第二重则不太明显，无论一个描述符何时关闭，该进程通过这一描述符引用的文件上的任何一把锁都会释放。

2. 由 `fork` 产生的子进程不继承父进程所设置的锁。这意味着，若一个进程得到一把锁，然后调用 `fork`，那么对于父进程获得的锁而言，子进程被视为另一个进程。对于通过 `fork` 从父进程处继承过来的描述符，子进程需要调用 `fcntl` 才能获得它自己的锁。这个约束是有道理的，因为锁的作用是阻止多个进程同时写同一个文件。如果子进程通过 `fork` 继承父进程的锁，则父进程和子进程就可以同时写同一个文件。

3. 在执行 `exec` 后，新程序可以继承原执行程序的锁。但是注意，如果对一个文件描述符设置了执行时关闭标志，那么当作为 `exec` 的一部分关闭该文件描述符时，将释放相应文件的所有锁。

### 4. FreeBSD 实现

考虑一个进程，它执行下列语句：

```c
fd1 = open(pathname, ...);
write_lock(fd1, 0, SEEK_SET, 1);
/* parent write locks byte 0 */
if ((pid = fork()) > 0) {
	/* parent */
	fd2 = dup(fd1);
	fd3 = open(pathname, ...);
} else if (pid == 0) {
	read_lock(fd1, 1, SEEK_SET, 1); /* child read locks byte 1 */
}
pause();
```

下图显示了父进程和子进程暂停（执行 pause()）后的数据结构情况。

![](unix环境高级编程14-高级IO/关于记录锁的FreeBSD数据结构.png)

图中显示了两个 `lockf` 结构，一个是由父进程调用 `write_lock` 形成的，另一个则是由子进程调用 `read_lock` 形成的。每一个结构都包含了相应的进程 ID。

在父进程中，关闭 `fd1`、`fd2` 或 `fd3` 中的任意一个都将释放由父进程设置的写锁。在关闭这 3 个描述符中的任意一个时，内核会从该描述符所关联的 `i` 节点开始，逐个检查 `lockf` 链接表中的各项，并释放由调用进程持有的各把锁。内核并不清楚（也不关心）父进程是用这 3 个描述中的哪一个来设置这把锁的。

例子，在文件整体上加一把写锁。

```cpp
#include <unistd.h>
#include <fcntl.h>

int lockfile(int fd){
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	return(fcntl(fd, F_SETLK, &fl));
}
```

另一种方法是用 `write_lock` 函数定义 `lockfile` 函数。

```c
#define lockfile(fd) write_lock((fd), 0, SEEK_SET, 0)
```

### 5. 在文件尾端加锁

在对相对于文件尾端的字节范围加锁或解锁时需要特别小心。大多数实现按照 `l_whence` 的 `SEEK_CUR` 或 `SEEK_END` 值，用 `l_start` 以及文件当前位置或当前长度得到绝对文件偏移量。但是，常常需要相对于文件的当前长度指定一把锁，但又不能调用 `fstat` 来得到当前文件长度，因为我们在该文件上没有锁。

考虑以下代码序列：

```c
writew_lock(fd, 0, SEEK_END, 0);
write(fd, buf, 1);
un_lock(fd, 0, SEEK_END);
write(fd, buf, 1);
```

该代码序列所做的可能并不是你所期望的。它得到一把写锁，该写锁从当前文件尾端起，包括以后可能追加写到该文件的任何数据。由此代码序列造成的文件锁状态如图所示：

![](unix环境高级编程14-高级IO/文件区域锁.png)

当对文件的一部分加锁时，内核将指定的偏移量变换成绝对文件偏移量。另外，除了指定一个绝对偏移量（`SEEK_SET`）之外，`fcntl` 还允许我们相对于文件中的某个点指定该偏移量，这个点是指当前偏移量（`SEEK_CUR`）或文件尾端（`SEEK_END`）。当前偏移量和文件尾端可能会不断变化，而这种变化又不应影响现有锁的状态，所以内核必须独立于当前文件偏移量或文件尾端而记住锁。

### 6. 建议性锁和强制性锁

建议性锁不能阻止对文件有写权限的任何其他进程写这个数据库文件。

强制性锁会让内核检查每一个 open、read 和 write，验证调用进程是否违背了正在访问的文件上的某一把锁。强制性锁有时也称为强迫方式锁（enforcement-mode locking）。


## `I/O` 多路转接

### 函数 `select` 和 `pselect`

在所有 `POSIX` 兼容的平台上，`select` 函数使我们可以执行 `I/O` 多路转接。传给 `select` 的参数告诉内核：

- 我们所关心的描述符；
- 对于每个描述符我们所关心的条件（是否想从一个给定的描述符读，是否想写一个给定的描述符，是否关心一个给定描述符的异常条件）；
- 愿意等待多长时间（可以永远等待、等待一个固定的时间或者根本不等待）。

从 select 返回时，内核告诉我们：

- 已准备好的描述符的总数量；
- 对于读、写或异常这 3 个条件中的每一个，哪些描述符已准备好。

使用这种返回信息，就可调用相应的 `I/O` 函数（一般是 read 或 write），并且确知该函数不会阻塞。

```cpp
#include <sys/select.h>
int select(int maxfdp1, fd_set *restrict readfds, fd_set *restrict writefds,
    fd_set *restrict exceptfds, struct timeval *restrict tvptr);
```

返回值：准备就绪的描述符数目；若超时，返回 0；若出错，返回 −1

最后一个参数，它指定愿意等待的时间长度，单位为秒和微秒。有以下 3 种情况。

- `tvptr == NULL` 永远等待。如果捕捉到一个信号则中断此无限期等待。当所指定的描述符中的一个已准备好或捕捉到一个信号则返回。如果捕捉到一个信号，则 select 返回-1，errno 设置为 EINTR。
- `tvptr->tv_sec == 0 && tvptr->tv_usec == 0` 根本不等待。测试所有指定的描述符并立即返回。这是轮询系统找到多个描述符状态而不阻塞 select 函数的方法。
- `tvptr->tv_sec != 0 || tvptr->tv_usec != 0` 等待指定的秒数和微秒数。当指定的描述符之一已准备好，或当指定的时间值已经超过时立即返回。如果在超时到期时还没有一个描述符准备好，则返回值是 0。与第一种情况一样，这种等待可被捕捉到的信号中断。

中间 3 个参数 readfds、writefds 和 exceptfds 是指向描述符集的指针。这 3 个描述符集说明了我们关心的可读、可写或处于异常条件的描述符集合。每个描述符集存储在一个 fd_set 数据类型中。这个数据类型是由实现选择的，它可以为每一个可能的描述符保持一位。我们可以认为它只是一个很大的字节数组。

对于 fd_set 数据类型，唯一可以进行的处理是：分配一个这种类型的变量，将这种类型的一个变量值赋给同类型的另一个变量，或对这种类型的变量使用下列 4 个函数中的一个。

```cpp
#include <sys/select.h>
int FD_ISSET(int fd, fd_set *fdset);
void FD_CLR(int fd, fd_set *fdset);
void FD_SET(int fd, fd_set *fdset);
void FD_ZERO(fd_set *fdset);
```

这些接口可实现为宏或函数。调用 FD_ZERO 将一个 fd_set 变量的所有位设置为 0。要开启描述符集中的一位，可以调用 FD_SET。调用 FD_CLR 可以清除一位。最后，可以调用 FD_ISSET 测试描述符集中的一个指定位是否已打开。

select 第一个参数 maxfdp1 的意思是“最大文件描述符编号值加 1”。

POSIX.1 也定义了一个 select 的变体，称为 pselect。

```cpp
#include <sys/select.h>
int pselect(int maxfdp1, fd_set *restrict readfds, fd_set *restrict writefds,
    fd_set *restrict exceptfds, const struct timespec *restrict tsptr,
    const sigset_t *restrict sigmask);
```

返回值：准备就绪的描述符数目；若超时，返回 0；若出错，返回 −1

除下列几点外，pselect 与 select 相同。

- select 的超时值用 timeval 结构指定，但 pselect 使用 timespec 结构。timespec 结构以秒和纳秒表示超时值，而非秒和微秒。如果平台支持这样的时间精度，那么 timespec 就能提供更精准的超时时间。
- pselect 的超时值被声明为 const，这保证了调用 pselect 不会改变此值。
- pselect 可使用可选信号屏蔽字。若 sigmask 为 NULL，那么在与信号有关的方面， pselect 的运行状况和 select 相同。否则，sigmask 指向一信号屏蔽字，在调用 pselect 时，以原子操作的方式安装该信号屏蔽字。在返回时，恢复以前的信号屏蔽字。

## 函数 poll

poll 函数类似于 select，但是程序员接口有所不同。poll 函数可用于任何类型的文件描述符。

```cpp
#include <poll.h>
int poll(struct pollfd fdarray[], nfds_t nfds, int timeout);
```

返回值：准备就绪的描述符数目；若超时，返回 0；若出错，返回-1

## 异步 `I/O`

### POSIX 异步 `I/O`

进行异步 `I/O` 之前需要先初始化 AIO 控制块，调用 aio_read 函数来进行异步读操作，或调用 aio_write 函数来进行异步写操作。

```cpp
#include <aio.h>
int aio_read(struct aiocb *aiocb);
int aio_write(struct aiocb *aiocb);
```

两个函数的返回值：若成功，返回 0；若出错，返回 −1

要想强制所有等待中的异步操作不等待而写入持久化的存储中，可以设立一个 AIO 控制块并调用 aio_fsync 函数。

```cpp
#include <aio.h>
int aio_fsync(int op, struct aiocb *aiocb);
```

返回值：若成功，返回 0；若出错，返回 −1

为了获知一个异步读、写或者同步操作的完成状态，需要调用 aio_error 函数。

```cpp
#include <aio.h>
int aio_error(const struct aiocb *aiocb);
```

如果异步操作成功，可以调用 aio_return 函数来获取异步操作的返回值。

```cpp
#include <aio.h>
ssize_t aio_return(const struct aiocb *aiocb);
```

如果在完成了所有事务时，还有异步操作未完成时，可以调用 aio_suspend 函数来阻塞进程，直到操作完成。

```cpp
#include <aio.h>
int aio_suspend(const struct aiocb *const list[], int nent, const struct timespec *timeout);
```

返回值：若成功，返回 0；若出错，返回 −1

当还有我们不想再完成的等待中的异步 `I/O` 操作时，可以尝试使用 aio_cancel 函数来取消它们。

```cpp
#include <aio.h>
int aio_cancel(int fd, struct aiocb *aiocb);
```

还有一个函数也被包含在异步 `I/O` 接口当中，尽管它既能以同步的方式来使用，又能以异步的方式来使用，这个函数就是 lio_listio。该函数提交一系列由一个 AIO 控制块列表描述的 `I/O` 请求。

```cpp
#include <aio.h>
int lio_listio(int mode, struct aiocb *restrict const list[restrict],
    int nent, struct sigevent *restrict sigev);
```

返回值：若成功，返回 0；若出错，返回 −1

## 函数 readv 和 writev

readv 和 writev 函数用于在一次函数调用中读、写多个非连续缓冲区。有时也将这两个函数称为散布读（scatter read）和聚集写（gather write）。

```cpp
#include <sys/uio.h>
ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
```

两个函数的返回值：已读或已写的字节数；若出错，返回 −1

## 函数 readn 和 writen

## 存储映射 `I/O`

储映射 `I/O`（memory-mapped `I/O`）能将一个磁盘文件映射到存储空间中的一个缓冲区上，于是，当从缓冲区中取数据时，就相当于读文件中的相应字节。与此类似，将数据存入缓冲区时，相应字节就自动写入文件。

为了使用这种功能，应首先告诉内核将一个给定的文件映射到一个存储区域中。这是由 mmap 函数实现的。

```cpp
#include <sys/mman.h>
void *mmap(void *addr, size_t len, int prot, int flag, intfd, off_t off);
```

返回值：若成功，返回映射区的起始地址；若出错，返回 MAP_FAILED

调用 mprotect 可以更改一个现有映射的权限。

```cpp
#include <sys/mman.h>
int mprotect(void *addr, size_t len, int prot);
```

返回值：若成功，返回 0；若出错，返回-1

如果共享映射中的页已修改，那么可以调用 msync 将该页冲洗到被映射的文件中。msync 函数类似于 fsync，但作用于存储映射区。

```cpp
#include <sys/mman.h>
int msync(void *addr, size_t len, int flags);
```

返回值：若成功，返回 0；若出错，返回-1

当进程终止时，会自动解除存储映射区的映射，或者直接调用 munmap 函数也可以解除映射区。关闭映射存储区时使用的文件描述符并不解除映射区。

```cpp
#include <sys/mman.h>
int munmap(void *addr, size_t len);
```

返回值：若成功，返回 0；若出错，返回 −1
