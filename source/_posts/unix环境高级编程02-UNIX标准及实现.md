---
title: unix环境高级编程02-UNIX标准及实现
date: 2023-04-10 17:20:15
tags: unix
category: unix环境高级编程
---

## Unix 标准化

ANSI 是美国国家标准学会（American National Standards Institute）的缩写。
ISO 是国际标准化组织（International Organization for Standardization）的缩写。
IEC 是国际电子技术委员会（International Electrotechnical Commission）的缩写。
IEEE 是电气和电子工程师学会（Institute of Electrical and Electronics Engineers）的缩写。
POSIX 指的是可移植操作系统接口（Portable Operating System Interface）。
XSI 是 X/Open 系统接口（X/Open System Interface，XSI）的缩写。
SUS 是单一 UNIX 规范（Single UNIX Specification）的缩写。
FIPS 代表的是联邦信息处理标准（Federal Information Processing Standard）。

### ISO C

ISO C 标准的意图是提供 C 程序的可移植性，使其能适合于大量不同的操作系统，而不只是适合 UNIX 系统。现今的 UNIX 系统都提供 C 标准中定义的库函数，所以该标准库非常重要。

ISO C 标准定义的头文件

| 头文件       | 说明                     |
| ------------ | ------------------------ |
| `<assert.h>`   | 断言                     |
| `<complex.h>`  | 复数运算支持             |
| `<ctype.h>`    | 字符分类和映射支持       |
| `<errno.h>`    | 出错码                   |
| `<fenv.h>`     | 浮点环境                 |
| `<float.h>`    | 浮点常量及特性           |
| `<inttypes.h>` | 整型格式变换             |
| `<iso646.h>`   | 赋值、关系及一元操作符宏 |
| `<limits.h>`   | 实现常量                 |
| `<locale.h>`   | 本地化类别及相关定义     |
| `<math.h>`     | 数学函数、类型声明及常量 |
| `<setjmp.h>`   | 非局部 goto              |
| `<signal.h>`   | 信号                     |
| `<stdarg.h>`   | 可变长度参数表           |
| `<stdbool.h>`  | bool 类型和值            |
| `<stddef.h> `  | 标准定义                 |
| `<stdint.h>`   | 整型                     |
| `<stdio.h>`    | 标准 IO                  |
| `<stdlib.h>`   | 实用函数                 |
| `<string.h>`   | 字符串操作               |
| `<tgmath.h>`   | 通用类型数学宏           |
| `<time.h>`     | 时间和日期               |
| `<wchar.h>`    | 多字节和宽字符支持       |
| `<wctype.h>`   | 宽字符分类和映射支持     |

### IEEE POSIX

POSIX 是一个最初由 IEEE 制订的标准族，后来则扩展成包括很多标记为 1003 的标准及标准草案。

IEEE 1003.1 标准通常称为 POSIX.1，该标准一直在进行更新发展，目前已经成熟稳定。

POSIX.1 包含了 ISO C 标准库函数。

POSIX 标准定义的必需的头文件

| 头文件          | 说明                 |
| --------------- | -------------------- |
| `<aio.h>`         | 异步 `I/O`             |
| `<cpio.h>`        | cpio 归档            |
| `<dirent.h>`      | 目录项               |
| `<dlfcn.h>`       | 动态链接             |
| `<fcntl.h>`       | 文件控制             |
| `<fnmatch.h>`     | 文件名匹配类型       |
| `<glob.h>`        | 路径名模式匹配与生成 |
| `<grp.h>`         | 组文件               |
| `<iconv.h>`       | 代码集变换实用程序   |
| `<langinfo.h>`    | 语言信息常量         |
| `<monetary.h>`    | 货币类型与函数       |
| `<netdb.h>`       | 网络数据库操作       |
| `<n1_types.h>`    | 消息类               |
| `<po11.h>`        | 投票函数             |
| `<pthread.h>`     | 线程                 |
| `<pwd.h>`         | 口令文件             |
| `<regex.h>`       | 正则表达式           |
| `<sched.h>`       | 执行调度             |
| `<semaphore.h>`   | 信号量               |
| `<strings.h>`     | 字符串操作           |
| `<tar.h>`         | tar 归档值           |
| `<termios.h>`     | 终端 `I/O`             |
| `<unistd.h>`      | 符号常量             |
| `<wordexp.h>`     | 字扩充类型           |
| `<arpa/inet.h>`   | 因特网定义           |
| `<net/if.h>`      | 套接字本地接口       |
| `<netinet/in.h>`  | 因特网地址族         |
| `<netinet/tcp.h>` | 传输控制协议定义     |
| `<sys/mman.h>`    | 存储管理声明         |
| `<sys/select.h>`  | select 函数          |
| `<sys/socket.h>`  | 套接字接口           |
| `<sys/stat.h>`    | 文件状态             |
| `<sys/statvfs.h>` | 文件系统信息         |
| `<sys/times.h>`   | 进程时间             |
| `<sys/types.h>`   | 基本系统数据类型     |
| `<sys/un.h> `     | unix 域套接字定义    |
| `<sys/utsnam.h>`  | 系统名               |
| `<sys/wait.h>`    | 进程控制             |

POSIX 标准定义的 XSI 可选头文件

| 头文件           | 说明             |
| ---------------- | ---------------- |
| `<fmtmsg.h>`       | 消息显示结构     |
| `<ftw.h>`          | 文件树漫游       |
| `<libgen.h>`       | 路径名管理函数   |
| `<ndbm.h>`         | 数据库操作       |
| `<search.h>`       | 搜索表           |
| `<syslog.h>`       | 系统出错日志记录 |
| `<utmpx.h>`        | 用户账户数据库   |
| `<sys/ipc.h>`      | IPC              |
| `<sys/msg.h>`      | XSI 消息队列     |
| `<sys/resource.h>` | 资源操作         |
| `<sys/sem.h>`      | XSI 信号量       |
| `<sys/shm.h>`      | XSI 共享存储     |
| `<sys/time.h>`     | 时间类型         |
| `<sys/uio.h>`      | 矢量 `I/O` 操作    |
| `<mqueue.h>`       | 消息队列         |
| `<spawn.h>`        | 实时 spawn 接口  |

POSIX.1 可选接口组和选项码

| 选项码 | 符号常量                            | 说明                             |
| ------ | ----------------------------------- | -------------------------------- |
| ADV    | `_POSIX_ADVISORY_INFO`              | 建议性信息（实时）               |
| CPT    | `_POSIX_CPUTIME`                    | 进程 CPU 时间时钟（实时）        |
| FSC    | `_POSIX_FSYNC`                      | 文件同步                         |
| IP6    | `_POSIX_IPV6`                       | IPv6 接口                        |
| ML     | `_POSIX_MEMLOCK`                    | 进程存储区域加锁（实时）         |
| MLR    | `_POSIX_MEMLOCK_RANGE`              | 存储区域加锁（实时）             |
| MON    | `_POSIX_MONOTONIC_CLOCK`            | 单调时钟（实时）                 |
| MSG    | `_POSIX_MESSAGE_PASSING`            | 消息传送（实时）                 |
| MX     | `__STDC_IEC_559__`                  | IEC 60559 浮点选项               |
| PIO    | `_POSIX_PRIORITIZED_IO`             | 优先输入和输出                   |
| PS     | `_POSIX_PRIORITIZED_SCHEDULING`     | 进程调度（实时）                 |
| RPI    | `_POSIX_THREAD_ROBUST_PRIO_INHERIT` | 健壮的互斥量优先权继承（实时）   |
| RPP    | `_POSIX_THREAD_ROBUST_PRIO_PROTECT` | 健壮的互斥量优先权保护（实时）   |
| RS     | `_POSIX_RAW_SOCKETS`                | 原始套接字                       |
| SHM    | `_POSIX_SHARED_MEMORY_OBJECTS`      | 共享存储对象（实时）             |
| SIO    | `_POSIX_SYNCHRONIZED_IO`            | 同步输入和输出（实时）           |
| SPN    | `_POSIX_SPAWN`                      | 产生（实时）                     |
| SS     | `_POSIX_SPORADIC_SERVER`            | 进程阵发性服务器（实时）         |
| TCT    | `_POSIX_THREAD_CPUTIME`             | 线程 CPU 时间时钟（实时）        |
| TPI    | `_POSIX_THREAD_PRIO_INHERIT`        | 非健壮的互斥量优先权继承（实时） |
| TPP    | `_POSIX_THREAD_PRIO_PROTECT`        | 非健壮的互斥量优先保护（实时）   |
| TPS    | `_POSIX_THREAD_PRIORITY_SCHEDULING` | 线程执行调度（实时）             |
| TSA    | `_POSIX_THREAD_ATTR_STACKADDR`      | 线程栈地址属性                   |
| TSH    | `_POSIX_THREAD_PROCESS_SHARED`      | 线程共享同步                     |
| TSP    | `_POSIX_THREAD_SPORADIC_SERVVER`    | 线程阵发性服务器（实时）         |
| TSS    | `_POSIX_THREAD_ATTR_STACKSIZE`      | 线程栈长度属性                   |
| TYM    | `_POSIX_TYPED_MEMORY_OBJECTS`       | 类型存储对象（实时）             |
| XSI    | `_XOPEN_UNIX`                       | X/Open 扩充接口                  |

### Single UNIX Specification

Single UNIX Specification（SUS，单一 UNIX 规范）是 POSIX.1 标准的一个超集，它定义了一些附加接口扩展了 POSIX.1 规范提供的功能。POSIX.1 相当于 Single UNIX Specification 中的基本规范部分。

Single UNIX Specification 是 Open Group 的出版物

## UNIX 系统实现

UNIX 的各种版本和变体都起源于在 PDP-11 系统上运行的 UNIX 分时系统第 6 版（1976 年）和第 7 版（1979 年）（通常称为 V6 和 V7）。

SVR4（UNIX System V Release4）是 AT&T 的 UNIX 系统实验室的产品，SVR4 符合 POSIX 1003.1 标准和 X/Open XPG3 标准。

BSD（Berkeley Software Distribution）是由加州大学伯克利分校的计算机系统研究组（CSRG）研究开发和分发的。4.4BSD-Lite 第 2 发行版是 CSRG 的最后一个 BSD 版本。

FreeBSD 基于 4.4BSD-Lite 操作系统。由 FreeBSD 项目产生的所有软件，包括其二进制代码和源代码，都是免费使用的。

Linux 是一种提供类似于 UNIX 的丰富编程环境的操作系统，在 GNU 公用许可证的指导下， Linux 是免费使用的。

Mac OS X 使用了完全不同的技术。其核心操作系统称为“Darwin”，它基于 Mach 内核、FreeBSD 操作系统以及具有面向对象框架的驱动和其他内核扩展的结合。

Solaris 是由 Sun Microsystems（现为 Oracle）开发的 UNIX 系统版本，它基于 SVR4。

## 标准和实现的关系

标准只是接口的规范，前面提到的各个标准定义了任一实际系统的子集。

## 限制

UNIX 系统实现定义了很多幻数和常量，其中有很多已被硬编码到程序中，或用特定的技术确定。
以下两种类型的限制是必需的。

- 编译时限制，编译时限制可在头文件中定义。
- 运行时限制，运行时限制则要求进程调用一个函数获得限制值。

为了解决这类问题，提供了以下 3 种限制。

- 编译时限制（头文件）。
- 与文件或目录无关的运行时限制（sysconf 函数）。
- 与文件或目录有关的运行时限制（pathconf 和 fpathconf 函数）。

### ISO C 限制

ISO C 定义的所有编译时限制都列在头文件`<limits.h>`中，这些限制常量在一个给定系统中并不会改变。

### POSIX 限制

POSIX.1 定义了很多涉及操作系统实现限制的常量，我们只关心与基本 POSIX.1 接口有关的部分。
这些限制和常量分成下列 7 类。

- 数值限制：LONG_BIT、SSIZE_MAX 和 WORD_BIT。
- 最小可接受值，一个符合 POSIX.1 的实现应当提供至少这样大的值。
- 最大值：`_POSIX_CLOCKRES_MIN` 。
- 运行时可以增加的值：CHARCLASS_NAME_MAX、COLL_WEIGHTS_MAX、LINE_MAX、NGROUPS_MAX 和 RE_DUP_MAX。
- 运行时不变值（可能不确定）：图 2-9 中的 17 个常量（加上 12.2 节中介绍的 4 个常量和 14.5 节中介绍的 3 个常量）。
- 其他不变值：NL_ARGMAX、NL_MSGMAX、NL_SETMAX 和 NL_TEXTMAX。
- 路径名可变值：FILESIZEBITS、LINK_MAX、MAX_CANON、MAX_INPUT、NAME_MAX、PATH_MAX、PIPE_BUF 和 SYMLINK_MAX。

### XSI 限制

XSI 定义了代表实现限制的几个常量。

- 5 个最小可接受值。
- 运行时不变值（可能不确定）：IOV_MAX 和 PAGE_SIZE。

### 函数 sysconf、pathconf 和 fpathconf

运行时限制可调用下面 3 个函数之一获得。

```c
#include <unistd.h>
long sysconf(int name);
long pathconf(const char *pathname, int name);
long fpathconf(int fd, int name);
```

参数：

- name：用于标识系统限制，以 `_SC_` 和 `_PC_` 开始的常量分别作用于 sysconf 和 pathconf 或 fpathconf 参数。
- pathname：路径名。
- fd：文件描述符。

返回值：

- 若成功，返回相应值；
- 若出错，返回-1。

- 如果 name 参数并不是一个合适的常量，这 3 个函数都返回 −1，并把 errno 置为 EINVAL。
- 有些 name 会返回一个变量值（返回值 ≥0）或者提示该值是不确定的。
- `_SC_CLK_TCK` 的返回值是每秒的时钟滴答数，用于 times 函数的返回值。

对于 pathconf 的参数 pathname 和 fpathconf 的参数 fd 有很多限制。如果不满足其中任何一个限制，则结果是未定义的。

- `_PC_MAX_CANON` 和 `_PC_MAX_INPUT` 引用的文件必须是终端文件。
- `_PC_LINK_MAX` 和 `_PC_TIMESTAMP_RESOLUTION` 引用的文件可以是文件或目录。如果是目录，则返回值用于目录本身，而不用于目录内的文件名项。
- `_PC_FILESIZEBITS` 和 `_PC_NAME_MAX` 引用的文件必须是目录，返回值用于该目录中的文件名。
- `_PC_PATH_MAX` 引用的文件必须是目录。当所指定的目录是工作目录时，返回值是相对路径名的最大长度。
- `_PC_PIPE_BUF` 引用的文件必须是管道、FIFO 或目录。在管道或 FIFO 情况下，返回值是对所引用的管道或 FIFO 的限制值。对于目录，返回值是对在该目录中创建的任一 FIFO 的限制值。
- `_PC_SYMLINK_MAX` 引用的文件必须是目录。返回值是该目录中符号链接可包含字符串的最大长度。

### 不确定的运行时限制

1. 路径名
   如果 `<limits.h>` 中定义了常量 `PATH_MAX`，那么就没有任何问题；如果未定义，则需调用 `pathconf`。

1. 最大打开文件数

支持 Single UNIX Specification 中 XSI 扩展的系统提供了 getrlimit(2)函数，它返回一个进程可以同时打开的描述符的最多个数。

OPEN_MAX 被 POSIX 称为运行时不变值，但是在支持 XSI 扩展的系统上，可以调用 setrlimit(2)函数更改一个运行进程的 OPEN_MAX 值， 也可以使用 C shell 的 limit 或 Bourne shell 的 ulimit 命令更改这个值。

例子，为路径名动态分配存储区以及确定文件描述符个数。

```c
#include "../apue.h"
#include <errno.h>
#include <limits.h>

#ifdef PATH_MAX
static long pathmax = PATH_MAX;
#else
static long pathmax = 0;
#endif

static long posix_version = 0;
static long xsi_version = 0;

#define PAHT_MAX_GUESS 1024

char* path_alloc(size_t *sizep){
    char *ptr;
    size_t size;
    if(posix_version == 0)
        posix_version = sysconf(_SC_VERSION);
    if (xsi_version)
        xsi_version = sysconf(_SC_XOPEN_VERSION);

    if (pathmax == 0){
        errno = 0;
        if (pathmax = pathconf("/", _PC_PATH_MAX) < 0){
            if(errno == 0)
                pathmax = PAHT_MAX_GUESS;
            else
                err_sys("pathconf error for _PC_PATH_MAX");
        }else{
            pathmax++;
        }
    }

    if((posix_version < 200112L) && (xsi_version < 4))
        size = pathmax + 1;
    else
        size = pathmax;

    if ((ptr = malloc(size)) == NULL)
        err_sys("malloc error for pathname");

    if(sizep != NULL)
        *sizep = size;
    return (ptr);

}

#ifdef OPEN_MAX
static long openmax = OPEN_MAX;
#else
static long openmax = 0;
#endif

#define OPEN_MAX_GUESS 256

long open_max(void){
    if(openmax == 0){
        errno = 0;
        if ((openmax = sysconf(_SC_OPEN_MAX)) < 0){
            if (errno == 0)
                openmax = OPEN_MAX_GUESS;
            else
                err_sys("sysconf error for _SC_OPEN_MAX");
        }
    }
    return (openmax);
}

int main(int argc, char const *argv[]){
    char *ptr = NULL;
    size_t size = 0;
    size_t openmax = 0;

    ptr = path_alloc(&size);
    printf("ptr = %p, size = %ld\n", ptr, size);
    printf("openmax = %ld\n", open_max());
    return 0;
}
```

编译运行：

```bash
$ gcc 2.5.5path.c ../error.c
$ ./a.out
ptr = 0x55efe7c75260, size = 4096
openmax = 1048576
```

## 选项

POSIX.1 定义了 3 种处理选项的方法。

- 编译时选项定义在<unistd.h>中。
- 与文件或目录无关的运行时选项用 sysconf 函数来判断。
- 与文件或目录有关的运行时选项通过调用 pathconf 或 fpathconf 函数来判断。

## 标准之间的冲突
