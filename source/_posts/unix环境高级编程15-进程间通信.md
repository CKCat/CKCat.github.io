---
title: unix环境高级编程15-进程间通信
date: 2023-04-10 17:23:15
tags: linux 编程
category: unix环境高级编程
---

# 进程间通信

过去，UNIX 系统 IPC 是各种进程通信方式的统称，但是，这些通信方式中极少有能在所有 UNIX 系统实现中进行移植的。随着 POSIX 和 TheOpen Group（以前是 X/Open）标准化的推进和影响的扩大，情况已得到改善，但差别仍然存在。

## 管道

管道是 UNIX 系统 IPC 的最古老形式，所有 UNIX 系统都提供此种通信机制。管道有以下两种局限性。

- 历史上，它们是半双工的（即数据只能在一个方向上流动）。现在，某些系统提供全双工管道，但是为了最佳的可移植性，我们决不应预先假定系统支持全双工管道。
- 管道只能在具有公共祖先的两个进程之间使用。通常，一个管道由一个进程创建，在进程调用 fork 之后，这个管道就能在父进程和子进程之间使用了。

尽管有这两种局限性，半双工管道仍是最常用的 IPC 形式。每当在管道中键入一个命令序列，让 shell 执行时，shell 都会为每一条命令单独创建一个进程，然后用管道将前一条命令进程的标准输出与后一条命令的标准输入相连接。

管道是通过调用 pipe 函数创建的。

```cpp
#include <unistd.h>
int pipe(int fd[2]);
```

返回值：若成功，返回 0，若出错，返回-1

经由参数 fd 返回两个文件描述符：`fd[0]`为读而打开，`fd[1]`为写而打开。`fd[1]`的输出是`fd[0]`的输入。

图中给出了两种描绘半双工管道的方法。左图显示管道的两端在一个进程中相互连接，右图则强调数据需要通过内核在管道中流动。

![](unix环境高级编程15-进程间通信/描绘半双工管道的两种方法.png)

fstat 函数对管道的每一端都返回一个 FIFO 类型的文件描述符。可以用 S_ISFIFO 宏来测试管道。

单个进程中的管道几乎没有任何用处。通常，进程会先调用 pipe，接着调用 fork，从而创建从父进程到子进程的 IPC 通道，反之亦然。

![](unix环境高级编程15-进程间通信/fork之后的半双工管道.png)

fork 之后做什么取决于我们想要的数据流的方向。对于从父进程到子进程的管道，父进程关闭管道的读端（`fd[0]`），子进程关闭写端（`fd[1]`）。对于一个从子进程到父进程的管道，父进程关闭`fd[1]`，子进程关闭`fd[0]`。

![](unix环境高级编程15-进程间通信/从父进程到子进程的管道.png)

当管道的一端被关闭后，下列两条规则起作用。

1. 当读（read）一个写端已被关闭的管道时，在所有数据都被读取后，read 返回 0，表示文件结束。
2. 如果写（write）一个读端已被关闭的管道，则产生信号 SIGPIPE。如果忽略该信号或者捕捉该信号并从其处理程序返回，则 write 返回 −1，errno 设置为 EPIPE。

在写管道（或 FIFO）时，常量 PIPE_BUF 规定了内核的管道缓冲区大小。

例子，经由管道从父进程向子进程传送数据。

```cpp
#include "apue.h"

int
main(void)
{
	int		n;
	int		fd[2];
	pid_t	pid;
	char	line[MAXLINE];

	if (pipe(fd) < 0)
		err_sys("pipe error");
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid > 0) {		/* parent */
		close(fd[0]);
		write(fd[1], "hello world\n", 12);
	} else {					/* child */
		close(fd[1]);
		n = read(fd[0], line, MAXLINE);
		write(STDOUT_FILENO, line, n);
	}
	exit(0);
}
```

在上面的例子中，直接对管道描述符调用了 read 和 write。更有趣的是将管道描述符复制到了标准输入或标准输出上。通常，子进程会在此之后执行另一个程序，该程序或者从标准输入（已创建的管道）读数据，或者将数据写至其标准输出（该管道）。

例子，将文件复制到分页程序。

```cpp
#include "apue.h"
#include <sys/wait.h>

#define	DEF_PAGER	"/bin/more"		/* default pager program */

int
main(int argc, char *argv[])
{
	int		n;
	int		fd[2];
	pid_t	pid;
	char	*pager, *argv0;
	char	line[MAXLINE];
	FILE	*fp;

	if (argc != 2)
		err_quit("usage: a.out <pathname>");

	if ((fp = fopen(argv[1], "r")) == NULL)
		err_sys("can't open %s", argv[1]);
	if (pipe(fd) < 0)
		err_sys("pipe error");

	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid > 0) {								/* parent */
		close(fd[0]);		/* close read end */

		/* parent copies argv[1] to pipe */
		while (fgets(line, MAXLINE, fp) != NULL) {
			n = strlen(line);
			if (write(fd[1], line, n) != n)
				err_sys("write error to pipe");
		}
		if (ferror(fp))
			err_sys("fgets error");

		close(fd[1]);	/* close write end of pipe for reader */

		if (waitpid(pid, NULL, 0) < 0)
			err_sys("waitpid error");
		exit(0);
	} else {										/* child */
		close(fd[1]);	/* close write end */
        // 在复制之前应当比较该描述符的值是否已经具有所希望的值。
		if (fd[0] != STDIN_FILENO) {
			if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
				err_sys("dup2 error to stdin");
			close(fd[0]);	/* don't need this after dup2 */
		}

		/* get arguments for execl() */
		if ((pager = getenv("PAGER")) == NULL)
			pager = DEF_PAGER;
		if ((argv0 = strrchr(pager, '/')) != NULL)
			argv0++;		/* step past rightmost slash */
		else
			argv0 = pager;	/* no slash in pager */

		if (execl(pager, argv0, (char *)0) < 0)
			err_sys("execl error for %s", pager);
	}
	exit(0);
}
```

上面代码先创建一个管道，fork 一个子进程，使子进程的标准输入成为管道的读端，然后调用 exec，执行用的分页程序。在调用 fork 之前，先创建一个管道。调用 fork 之后，父进程关闭其读端，子进程关闭其写端。然后子进程调用 dup2，使其标准输入成为管道的读端。当执行分页程序时，其标准输入将是管道的读端。

## 函数 popen 和 pclose

常见的操作是创建一个连接到另一个进程的管道，然后读其输出或向其输入端发送数据，为此，标准 I/O 库提供了两个函数 popen 和 pclose。这两个函数实现的操作是：创建一个管道， fork 一个子进程，关闭未使用的管道端，执行一个 shell 运行命令，然后等待命令终止。

```cpp
#include <stdio.h>
FILE *popen(const char *cmdstring, const char *type);
```

返回值：若成功，返回文件指针；若出错，返回 NULL

```cpp
int pclose(FILE *fp);
```

返回值：若成功，返回 cmdstring 的终止状态；若出错，返回-1

函数 popen 先执行 fork，然后调用 exec 执行 cmdstring，并且返回一个标准 I/O 文件指针。如果 type 是"r"，则文件指针连接到 cmdstring 的标准输出。如果 type 是"w"，则文件指针连接到 cmdstring 的标准输入，如图 15-10 所示。

pclose 函数关闭标准 I/O 流，等待命令终止，然后返回 shell 的终止状态。如果 shell 不能被执行，则 pclose 返回的终止状态与 shell 已执行 exit(127)一样。

cmdstring 由 Bourne shell 以下列方式执行：

```bash
sh -c cmdstring
```

例子，用 popen 向分页程序传送文件。

```cpp
#include "apue.h"
#include <sys/wait.h>

#define	PAGER	"${PAGER:-more}" /* environment variable, or default */

int
main(int argc, char *argv[])
{
	char	line[MAXLINE];
	FILE	*fpin, *fpout;

	if (argc != 2)
		err_quit("usage: a.out <pathname>");
	if ((fpin = fopen(argv[1], "r")) == NULL)
		err_sys("can't open %s", argv[1]);

	if ((fpout = popen(PAGER, "w")) == NULL)
		err_sys("popen error");

	/* copy argv[1] to pager */
	while (fgets(line, MAXLINE, fpin) != NULL) {
		if (fputs(line, fpout) == EOF)
			err_sys("fputs error to pipe");
	}
	if (ferror(fpin))
		err_sys("fgets error");
	if (pclose(fpout) == -1)
		err_sys("pclose error");

	exit(0);
}
```

shell 命令 `${PAGER:-more}` 的意思是：如果 shell 变量 PAGER 已经定义，且其值非空，则使用其值，否则使用字符串 more。

例子，函数popen和pclose。
```cpp
#include "apue.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

/*
 * Pointer to array allocated at run-time.
 */
static pid_t	*childpid = NULL;

/*
 * From our open_max(), {Prog openmax}.
 */
static int		maxfd;

FILE *
popen(const char *cmdstring, const char *type)
{
	int		i;
	int		pfd[2];
	pid_t	pid;
	FILE	*fp;

	/* only allow "r" or "w" */
	if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0) {
		errno = EINVAL;
		return(NULL);
	}

	if (childpid == NULL) {		/* first time through */
		/* allocate zeroed out array for child pids */
		maxfd = open_max();
		if ((childpid = calloc(maxfd, sizeof(pid_t))) == NULL)
			return(NULL);
	}

	if (pipe(pfd) < 0)
		return(NULL);	/* errno set by pipe() */
	if (pfd[0] >= maxfd || pfd[1] >= maxfd) {
		close(pfd[0]);
		close(pfd[1]);
		errno = EMFILE;
		return(NULL);
	}

	if ((pid = fork()) < 0) {
		return(NULL);	/* errno set by fork() */
	} else if (pid == 0) {							/* child */
		if (*type == 'r') {
			close(pfd[0]);
			if (pfd[1] != STDOUT_FILENO) {
				dup2(pfd[1], STDOUT_FILENO);
				close(pfd[1]);
			}
		} else {
			close(pfd[1]);
			if (pfd[0] != STDIN_FILENO) {
				dup2(pfd[0], STDIN_FILENO);
				close(pfd[0]);
			}
		}

		/* close all descriptors in childpid[] */
		for (i = 0; i < maxfd; i++)
			if (childpid[i] > 0)
				close(i);

		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
		_exit(127);
	}

	/* parent continues... */
	if (*type == 'r') {
		close(pfd[1]);
		if ((fp = fdopen(pfd[0], type)) == NULL)
			return(NULL);
	} else {
		close(pfd[0]);
		if ((fp = fdopen(pfd[1], type)) == NULL)
			return(NULL);
	}

	childpid[fileno(fp)] = pid;	/* remember child pid for this fd */
	return(fp);
}

int
pclose(FILE *fp)
{
	int		fd, stat;
	pid_t	pid;

	if (childpid == NULL) {
		errno = EINVAL;
		return(-1);		/* popen() has never been called */
	}

	fd = fileno(fp);
	if (fd >= maxfd) {
		errno = EINVAL;
		return(-1);		/* invalid file descriptor */
	}
	if ((pid = childpid[fd]) == 0) {
		errno = EINVAL;
		return(-1);		/* fp wasn't opened by popen() */
	}

	childpid[fd] = 0;
	if (fclose(fp) == EOF)
		return(-1);

	while (waitpid(pid, &stat, 0) < 0)
		if (errno != EINTR)
			return(-1);	/* error other than EINTR from waitpid() */

	return(stat);	/* return child's termination status */
}
```
POSIX.1要求popen关闭那些以前调用popen打开的、现在仍然在子进程中打开着的I/O流。为此，在子进程中从头逐个检查childpid数组的各个元素，关闭仍旧打开着的描述符。

若pclose的调用者已经为信号SIGCHLD设置了一个信号处理程序，则pclose中的waitpid调用将返回一个错误EINTR。因为允许调用者捕捉此信号（或者任何其他可能中断waitpid调用的信号），所以当waitpid被一个捕捉到的信号中断时，我们只是再次调用waitpid。

注意，popen决不应由设置用户ID或设置组ID程序调用。当它执行命令时，popen等同于：

```c++
execl("/bin/sh", "sh", "-c", command, NULL);
```
它在从调用者继承的环境中执行shell，并由shell解释执行command。

popen特别适用于执行简单的过滤器程序，它变换运行命令的输入或输出。当命令希望构造它自己的管道时，就是这种情形。

例子，用popen对输入进行变换处理。
```cpp
// myuclc.c
#include "apue.h"
#include <ctype.h>

int
main(void)
{
	int		c;

	while ((c = getchar()) != EOF) {
		if (isupper(c))
			c = tolower(c);
		if (putchar(c) == EOF)
			err_sys("output error");
		if (c == '\n')
			fflush(stdout);
	}
	exit(0);
}
```
上面的例子将标准输入复制到标准输出，在复制时将大写字符变换为小写字符。

```cpp
#include "apue.h"
#include <sys/wait.h>

int
main(void)
{
	char	line[MAXLINE];
	FILE	*fpin;

	if ((fpin = popen("myuclc", "r")) == NULL)
		err_sys("popen error");
	for ( ; ; ) {
		fputs("prompt> ", stdout);
		fflush(stdout);
		if (fgets(line, MAXLINE, fpin) == NULL)	/* read from pipe */
			break;
		if (fputs(line, stdout) == EOF)
			err_sys("fputs error to pipe");
	}
	if (pclose(fpin) == -1)
		err_sys("pclose error");
	putchar('\n');
	exit(0);
}
```
使用 popen 调用上面的程序。因为标准输出通常是行缓冲的，而提示并不包含换行符，所以在写了
提示之后，需要调用fflush。

## 协同进程
