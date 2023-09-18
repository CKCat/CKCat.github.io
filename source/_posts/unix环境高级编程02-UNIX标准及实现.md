---
title: unix环境高级编程02-UNIX标准及实现
date: 2023-04-10 17:20:15
tags: linux 编程
category: unix环境高级编程
---

## Unix 标准化

- `ANSI` 是美国国家标准学会（American National Standards Institute）的缩写。
- `ISO` 是国际标准化组织（International Organization for Standardization）的缩写。
- `IEC` 是国际电子技术委员会（International Electrotechnical Commission）的缩写。
- `IEEE` 是电气和电子工程师学会（Institute of Electrical and Electronics Engineers）的缩写。
- `POSIX` 指的是可移植操作系统接口（Portable Operating System Interface）。
- `XSI` 是 X/Open 系统接口（X/Open System Interface，XSI）的缩写。
- `SUS` 是单一 UNIX 规范（Single UNIX Specification）的缩写。
- `FIPS` 代表的是联邦信息处理标准（Federal Information Processing Standard）。

### ISO C

`ISO C` 标准的意图是提供 C 程序的可移植性，使其能适合于大量不同的操作系统，而不只是适合 UNIX 系统。现今的 UNIX 系统都提供 C 标准中定义的库函数，所以该标准库非常重要。

`ISO C` 标准定义的头文件：

| 头文件         | 说明                     |
| -------------- | ------------------------ |
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

`POSIX` 是一个最初由 `IEEE` 制订的标准族，后来则扩展成包括很多标记为 1003 的标准及标准草案。

`IEEE 1003.1` 标准通常称为 `POSIX.1`，该标准一直在进行更新发展，目前已经成熟稳定。

`POSIX.1` 包含了 `ISO C` 标准库函数。

`POSIX` 标准定义的必需的头文件：

| 头文件            | 说明                 |
| ----------------- | -------------------- |
| `<aio.h>`         | 异步 `I/O`           |
| `<cpio.h>`        | `cpio` 归档          |
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
| `<nl_types.h>`    | 消息类               |
| `<poll.h>`        | 投票函数             |
| `<pthread.h>`     | 线程                 |
| `<pwd.h>`         | 口令文件             |
| `<regex.h>`       | 正则表达式           |
| `<sched.h>`       | 执行调度             |
| `<semaphore.h>`   | 信号量               |
| `<strings.h>`     | 字符串操作           |
| `<tar.h>`         | `tar` 归档值         |
| `<termios.h>`     | 终端 `I/O`           |
| `<unistd.h>`      | 符号常量             |
| `<wordexp.h>`     | 字扩充类型           |
| `<arpa/inet.h>`   | 因特网定义           |
| `<net/if.h>`      | 套接字本地接口       |
| `<netinet/in.h>`  | 因特网地址族         |
| `<netinet/tcp.h>` | 传输控制协议定义     |
| `<sys/mman.h>`    | 存储管理声明         |
| `<sys/select.h>`  | `select` 函数        |
| `<sys/socket.h>`  | 套接字接口           |
| `<sys/stat.h>`    | 文件状态             |
| `<sys/statvfs.h>` | 文件系统信息         |
| `<sys/times.h>`   | 进程时间             |
| `<sys/types.h>`   | 基本系统数据类型     |
| `<sys/un.h> `     | unix 域套接字定义    |
| `<sys/utsnam.h>`  | 系统名               |
| `<sys/wait.h>`    | 进程控制             |

`POSIX` 标准定义的 `XSI` 可选头文件

| 头文件             | 说明             |
| ------------------ | ---------------- |
| `<fmtmsg.h>`       | 消息显示结构     |
| `<ftw.h>`          | 文件树漫游       |
| `<libgen.h>`       | 路径名管理函数   |
| `<ndbm.h>`         | 数据库操作       |
| `<search.h>`       | 搜索表           |
| `<syslog.h>`       | 系统出错日志记录 |
| `<utmpx.h>`        | 用户账户数据库   |
| `<sys/ipc.h>`      | `IPC`            |
| `<sys/msg.h>`      | `XSI` 消息队列   |
| `<sys/resource.h>` | 资源操作         |
| `<sys/sem.h>`      | `XSI` 信号量     |
| `<sys/shm.h>`      | `XSI` 共享存储   |
| `<sys/time.h>`     | 时间类型         |
| `<sys/uio.h>`      | 矢量 `I/O` 操作  |

`POSIX` 标准定义的可选头文件：

| 头文件       | 说明              |
| ------------ | ----------------- |
| `<mqueue.h>` | 消息队列          |
| `<spawn.h>`  | 实时 `spawn` 接口 |

`POSIX.1` 可选接口组和选项码

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

Single UNIX Specification（SUS，单一 UNIX 规范）是 `POSIX.1` 标准的一个超集，它定义了一些附加接口扩展了 `POSIX.1` 规范提供的功能。`POSIX.1` 相当于 Single UNIX Specification 中的基本规范部分。

Single UNIX Specification 是 Open Group 的出版物。

## UNIX 系统实现

UNIX 的各种版本和变体都起源于在 PDP-11 系统上运行的 UNIX 分时系统第 6 版（1976 年）和第 7 版（1979 年）（通常称为 V6 和 V7）。

SVR4（UNIX System V Release4）是 `AT&T` 的 UNIX 系统实验室的产品，SVR4 符合` POSIX 1003.1` 标准和 `X/Open XPG3` 标准。

BSD（Berkeley Software Distribution）是由加州大学伯克利分校的计算机系统研究组（CSRG）研究开发和分发的。`4.4BSD-Lite` 第 2 发行版是 CSRG 的最后一个 BSD 版本。

FreeBSD 基于 `4.4BSD-Lite` 操作系统。由 FreeBSD 项目产生的所有软件，包括其二进制代码和源代码，都是免费使用的。

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
- 与文件或目录无关的运行时限制（`sysconf` 函数）。
- 与文件或目录有关的运行时限制（`pathconf` 和 `fpathconf` 函数）。

### ISO C 限制

`ISO C `定义的所有编译时限制都列在头文件 `<limits.h>` 中，这些限制常量在一个给定系统中并不会改变。

### POSIX 限制

`POSIX.1` 定义了很多涉及操作系统实现限制的常量，我们只关心与基本 `POSIX.1` 接口有关的部分。
这些限制和常量分成下列 7 类。

1. 数值限制：`LONG_BIT`、`SSIZE_MAX` 和 `WORD_BIT`。
1. 最小可接受值，一个符合 `POSIX.1` 的实现应当提供至少这样大的值。
1. 最大值：`_POSIX_CLOCKRES_MIN` 。
1. 运行时可以增加的值：`CHARCLASS_NAME_MAX`、`COLL_WEIGHTS_MAX`、`LINE_MAX、NGROUPS_MAX `和 `RE_DUP_MAX`。
1. 运行时不变值（可能不确定）：主要是表中的 17 个常量。
1. 其他不变值：`NL_ARGMAX`、`NL_MSGMAX`、`NL_SETMAX` 和 `NL_TEXTMAX`。
1. 路径名可变值：`FILESIZEBITS`、`LINK_MAX`、`MAX_CANON`、`MAX_INPUT`、`NAME_MAX`、`PATH_MAX`、`PIPE_BUF` 和 `SYMLINK_MAX`。

`<limits.h>` 中的 `POSIX.1` 运行时不变值：

| 名称           | 说明                                        | 最小可接受值            |
| -------------- | ------------------------------------------- | ----------------------- |
| ARG_MAX        | `exec` 函数族的参数最大长度                 | `_POSIX_ARG_MAX`        |
| ATEXIT_MAX     | 可用 `atexit` 函数登记的最大函数个数        | 32                      |
| CHILD_MAX      | 每个实际用户 ID 的子进程最大个数            | `_POSIX_CHILD_MAX`      |
| DELAYTIMER_MAX | 定时器最大超限运行次数                      | `_POSIX_DELAYTIMER_MAX` |
| HOST_NAME_MAX  | `gethostname` 返回的主机名长度              | `_POSIX_HOST_NAME_MAX`  |
| LOGIN_NAME_MAX | 登录名最大长度                              | `_POSIX_LOGIN_NAME_MAX` |
| OPEN_MAX       | 赋予新建文件描述符的最大值+1                | `_POSIX_OPEN_MAX`       |
| PAGESIZE       | 系统内存页大小(以字节为单位)                | 1                       |
| RTSIG_MAX      | 为应用程序预留的实时信号的最大个数          | `_POSIX_RTSIG_MAX`      |
| SEM_NSEMS_MAX  | 一个进程可使用的信号量最大个数              | `_POSIX_SEM_NSEMS_MAX`  |
| SEM_VALUE_MAX  | 信号量的最大值                              | `_POSIX_SEM_VALUE_MAX`  |
| SIGQUEUE_MAX   | 一个进程可排队信号的最大个数                | `_POSIX_SIGQUEUE_MAX`   |
| STREAM_MAX     | 一个进程一次可打开的标准 `I/O` 流的最大个数 | `_POSIX_STREAM_MAX`     |
| SYMLOOPMAX     | 路径解析过程中可访问的符号链接数            | `_POSIX_SYMLOOP_MAX`    |
| TIMER_MAX      | 一个进程的定时器最大个数                    | `_POSIX_TIMER_MAX`      |
| TTY_NAME_MAX   | 终端设备名长度，其中包括终止的 `null` 字节  | `_POSIX_TTY_NAME_MAX`   |
| TZNAME_MAX     | 时区名的字节数                              | `_POSIX_TZNAME_MAX`     |

### XSI 限制

`XSI` 定义了代表实现限制的几个常量。

- 5 个最小可接受值。
- 运行时不变值（可能不确定）：`IOV_MAX` 和 `PAGE_SIZE`。

`<limits.h>`中的 `XSI` 最小值：

| 名称              | 说明                                              | 最小可接受值 | 典型值 |
| ----------------- | ------------------------------------------------- | ------------ | ------ |
| `NL_LANGMAX`      | 在 `LANG` 环境变量中最大字节数                    | 14           | 14     |
| `NZERO`           | 默认进程优先级                                    | 20           | 20     |
| `_XOPEN_IOV_MAX`  | `readv` 或 `writev` 可使用的最多 `iovec` 结构个数 | 16           | 16     |
| `_XOPEN_NAME_MAX` | 文件名中的字节数                                  | 255          | 255    |
| `_XOPEN_PATH_MAX` | 路径名中的字节数                                  | 1024         | 1024   |

### 函数 sysconf、pathconf 和 fpathconf

运行时限制可调用下面 3 个函数之一获得。

```c
#include <unistd.h>
long sysconf(int name);
long pathconf(const char *pathname, int name);
long fpathconf(int fd, int name);
```

参数：

- name：用于标识系统限制，以 `_SC_` 和 `_PC_` 开始的常量分别作用于 `sysconf` 和 `pathconf` 或 `fpathconf` 参数。
- pathname：路径名。
- fd：文件描述符。

返回值：

- 若成功，返回相应值。
- 若出错，返回-1。

说明：

- 如果 name 参数并不是一个合适的常量，这 3 个函数都返回 −1，并把 `errno` 置为 `EINVAL`。
- 有些 name 会返回一个变量值（返回值 ≥0）或者提示该值是不确定的。
- `_SC_CLK_TCK` 的返回值是每秒的时钟滴答数，用于 `times` 函数的返回值。

对 `sysconf` 的限制及 name 参数：

| 限制名           | 说明                                                                                             | name 参数              |
| ---------------- | ------------------------------------------------------------------------------------------------ | ---------------------- |
| ARG_MAX          | `exec` 函数的参数最大长度(字节)                                                                  | `_SC_ARG_MAX`          |
| ATEXIT_MAX       | 可用 `atexit` 函数登记的最大函数个数                                                             | `_SC_ATEXIT_MAX`       |
| CHILD_MAX        | 每个实际用户 ID 的最大进程数                                                                     | `_SC_CHILD_MAX`        |
| 时钟滴答/秒      | 每秒时钟滴答数                                                                                   | `_SC_CLK_TCK`          |
| COLL_WEIGHTS_MAX | 在本地定义文件中可以赋予 `LC_COLLATE` 顺序关键字项的最大权重数                                   | `_SC_COLL_WEIGHTS_MAX` |
| DELAYTIMER_MAX   | 定时器最大超限运行次数                                                                           | `_SC_DELAYTIMER_MAX`   |
| HOST_NAME_MAX    | `gethostname` 函数返回的主机名最大长度                                                           | `_SC_HOST_NAME_MAX`    |
| IOV_MAX          | `readv` 或 `writev` 函数可以使用最多的 `iovec` 结构的个数                                        | `_SC_IOV_MAX`          |
| LINE_MAX         | 实用程序输入行的最大长度                                                                         | `_SC_LINE_MAX`         |
| LOGIN_NAME_MAX   | 登录名的最大长度                                                                                 | `_SC_LOGIN_NAME_MAX`   |
| NGROUPS_MAX      | 每个进程同时添加的最大进程组 ID 数                                                               | `_SC_NGROUPS_MAX`      |
| OPEN_MAX         | 每个进程最大打开文件数                                                                           | `_SC_OPEN_MAX`         |
| PAGESIZE         | 系统存储页长度(字节数)                                                                           | `_SC_PAGESIZE`         |
| PAGE_SIZE        | 系统存储页长度(字节数)                                                                           | `_SC_PAGE_SIZE`        |
| RE_DUP_MAX       | 当使用间隔表示法 `\{m,n\}` 时，函数 `regexec` 和 `regcomp` 允许的基本正则表达式重复发生次数      | `_SC_RE_DUP_MAX`       |
| RTSIG_MAX        | 为应用程序预留的实时信号的最大个数                                                               | `_SC_RTSIG_MAX`        |
| SEM_NSEMS_MAX    | 一个进程可使用的信号量最大个数                                                                   | `_SC_SEM_NSEMS_MAX`    |
| SEM_VALUE_MAX    | 信号量的最大值                                                                                   | `_SC_SEM_VALUE_MAX`    |
| SIGQUEUE_MAX     | 一个进程可排队信号的最大个数                                                                     | `_SC_SIGQUEUE_MAX`     |
| STREAM_MAX       | 一个 `SCSTREAM_MAX` 进程在任意给定时刻标准 O 流的最大个数。如果定义，必须与 `FOPEN_MAX` 有相同值 | `_SC_STREAM_MAX`       |
| SYMLOOP_MAX      | 在解析路径名时，可遍历的符号链接数                                                               | `_SC_SYMLOOP_MAX`      |
| TIMER_MAX        | 每个进程的最大定时器个数                                                                         | `_SC_TIMER_MAX`        |
| TTY_NAME_MAX     | 终端设备名长度，包括终止 `null` 字节                                                             | `_SC_TTY_NAME_MAX`     |
| TZNAME_MAX       | 时区名中的最大字节数                                                                             | `_SC_TZNAME_MAX`       |

对 `pathconf` 和 `fpathconf` 的限制及 name 参数：

| 限制名                        | 说明                                                                    | name 参数                  |
| ----------------------------- | ----------------------------------------------------------------------- | -------------------------- |
| `FILESIZEBITS`                | 以带符号整型值表示在指定目录中允许的普通文件最大长度所需的最小位(bit)数 | `_PC_FILESIZEBITS`         |
| `LINK_MAX`                    | 文件链接计数的最大值                                                    | `_PC_LINK_MAX`             |
| `MAX_CANON`                   | 终端规范输入队列的最大字节数                                            | `_PC_MAX_CANON`            |
| `MAX_INPUT`                   | 终端输入队列可用空间的字节数                                            | `_PC_MAX_INPUT`            |
| `NAME_MAX`                    | 文件名的最大字节数(不包括终止 `null` 字节)                              | `_PC_NAME_MAX`             |
| `PATH_MAX`                    | 相对路径名的最大字节数，包括终止 `null` 字节                            | `_PC_PATH_MAX`             |
| `PIPE_BUF`                    | 能原子地写到管道的最大字节数                                            | `_PC_PIPE_BUF`             |
| `_POSIX_TIMESTAMP_RESOLUTION` | 文件时间戳的纳秒精度                                                    | `_PC_TIMESTAMP_RESOLUTION` |
| `SYMLINK_MAX`                 | 符号链接的字节数                                                        | `_PC_SYMLINK_MAX`          |

对于 `pathconf` 的参数 pathname 和 `fpathconf` 的参数 fd 有很多限制。如果不满足下列任何一个限制，则结果是未定义的。

- `_PC_MAX_CANON` 和 `_PC_MAX_INPUT` 引用的文件必须是终端文件。
- `_PC_LINK_MAX` 和 `_PC_TIMESTAMP_RESOLUTION` 引用的文件可以是文件或目录。如果是目录，则返回值用于目录本身，而不用于目录内的文件名项。
- `_PC_FILESIZEBITS` 和 `_PC_NAME_MAX` 引用的文件必须是目录，返回值用于该目录中的文件名。
- `_PC_PATH_MAX` 引用的文件必须是目录。当所指定的目录是工作目录时，返回值是相对路径名的最大长度。
- `_PC_PIPE_BUF` 引用的文件必须是管道、`FIFO` 或目录。在管道或 `FIFO` 情况下，返回值是对所引用的管道或 `FIFO` 的限制值。对于目录，返回值是对在该目录中创建的任一 `FIFO` 的限制值。
- `_PC_SYMLINK_MAX` 引用的文件必须是目录。返回值是该目录中符号链接可包含字符串的最大长度。

### 不确定的运行时限制

1. 路径名

很多程序需要为路径名分配存储区，一般来说，在编译时就为其分配了存储区，而且不同的程序使用各种不同的幻数作为数组长度。

例子，为路径名动态分配存储空间：

```cpp
// path_alloc.c
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>		/* ISO C variable aruments */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void err_sys(const char *fmt, ...);

#ifdef PATH_MAX
static long pathmax = PATH_MAX;
#else
static long pathmax = 0;
#endif

static long posix_version = 0;
static long xsi_version = 0;

#define PATH_MAX_GUESS 1024

char* path_alloc(size_t *sizep){
	char *ptr;
	size_t size;
	if(posix_version == 0)
		posix_version = sysconf(_SC_VERSION);
	if(xsi_version == 0)
		xsi_version = sysconf(_SC_XOPEN_VERSION);
	if(pathmax == 0){
		errno = 0;
		if((pathmax = pathconf("/", _PC_PATH_MAX)) < 0){
			if(errno == 0)
				pathmax = PATH_MAX_GUESS;
			else
				err_sys("pathconf error for _PC_PATH_MAX");
		}
		else{
			pathmax++;
		}
	}

	if((posix_version < 200112L) && (xsi_version < 4))
		size = pathmax - 1;
	else
		size = pathmax;

	if((ptr = malloc(size)) == NULL)
		err_sys("malloc error for pathname");
	if(sizep != NULL)
		*sizep = size;
	return (ptr);
}

int main(int argc, char* argv[]){
	size_t size = 0;
	path_alloc(&size);
	printf("pathmax = %ld\n", size);
	return 0;
}
```

编译运行：

```bash
$ gcc path_alloc.c
$ ./a.out
pathmax = 4096
```

如果 `<limits.h>` 中定义了常量 `PATH_MAX`，那么就没有任何问题；如果未定义，则需调用 `pathconf`。

2. 最大打开文件数

例子，确定文件描述符个数：

```c
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>		/* ISO C variable aruments */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void err_sys(const char *fmt, ...);

#ifdef OPEN_MAX
static long openmax = OPEN_MAX;
#else
static long openmax = 0;
#endif

#define OPEN_MAX_GUESS 256

long open_max(){
	if(openmax == 0){
		errno = 0;
		if((openmax = sysconf(_SC_OPEN_MAX)) < 0){
			if(errno == 0)
				openmax = OPEN_MAX_GUESS;
			else
				err_sys("sysconf error for _SC_OPEN_MAX");
		}
	}
	return openmax;
}

int main(int argc, char* argv[]){
	printf("openmax = %ld\n", open_max());
}
```

编译运行：

```bash
$ gcc openmax.c
$ ./a.out
openmax = 1024
```

在支持 Single UNIX Specification 中 `XSI` 扩展的系统上提供了 `getrlimit(2)`函数，它返回一个进程可以同时打开的描述符的最多个数。

`OPEN_MAX` 被 `POSIX` 称为运行时不变值，但是在支持 `XSI` 扩展的系统上，可以调用 `setrlimit(2)` 函数更改一个运行进程的 `OPEN_MAX` 值， 也可以使用 `C shell` 的 `limit` 或 `Bourne shell` 的 `ulimit` 命令更改这个值。如果系统支持这种功能，则可以更改 `open_max` 函数，使得每次调用此函数时都会调用 `sysconf`，而不只是在第一次调用此函数时调用 `sysconf`。

## 选项

如果我们要编写可移植的应用程序，而这些程序可能会依赖于这些可选的支持的功能，那么就需要一种可移植的方法来判断实现是否支持一个给定的选项。

POSIX.1 可选接口组和选项码：

| 选项码 | 符号常量                            | 说明                           |
| ------ | ----------------------------------- | ------------------------------ |
| ADV    | `_POSIX_ADVISORY_INFO`              | 建议性信息(实时)               |
| CPT    | `_POSIX_CPUTIME`                    | 进程 CPU 时间时钟(实时)        |
| FSC    | `_POSIX_FSYNC`                      | 文件同步                       |
| IP6    | `_POSIX_IPV6`                       | IPv6 接口                      |
| ML     | `_POSIX_MEMLOCK`                    | 进程存储区加锁(实时)           |
| MLR    | `_POSIX_MEMLOCK_RANGE`              | 存储区域加锁(实时)             |
| MON    | `_POSIX_MONOTONIC_CLOCK`            | 单调时钟(实时)                 |
| MSG    | `_POSIX_MESSAGE_PASSING`            | 消息传送(实时)                 |
| MX     | `__STDC_IEC_559__`                  | IEC60559 浮点选项              |
| PIO    | `_POSIX_PRIORITIZED_IO`             | 优先输入和输出                 |
| PS     | `_POSIX_PRIORITIZED_SCHEDULING`     | 进程调度(实时)                 |
| RPI    | `_POSIX_THREAD_ROBUST_PRIO_INHERIT` | 健壮的互斥量优先权继承(实时)   |
| RPP    | `_POSIX_THREAD_ROBUST_PRIO_PROTECT` | 健壮的互斥量优先权保护(实时 )  |
| RS     | `_POSIX_RAW_SOCKETS`                | 原始套接字                     |
| SHM    | `_POSIX_SHARED_MEMORY_OBJECTS`      | 共享存储对象(实时)             |
| SIO    | `_POSIX_SYNCHRONIZED_IO`            | 同步输入和输出(实时)           |
| SPN    | `_POSIX_SPAWN`                      | 产生(实时)                     |
| SS     | `_POSIX_SPORADIC_SERVER`            | 进程阵发性服务器(实时)         |
| TCT    | `_POSIX_THREAD_CPUTIME`             | 线程 CPU 时间时钟(实时)        |
| TPI    | `_POSIX_THREAD_PRIO_INHERIT`        | 非键壮的互斥量优先权继承(实时) |
| TPP    | `_POSIX_THREAD_PRIO_PROTECT`        | 非键壮的互斥量优先权保护(实时) |
| TPS    | `_POSIX_THREAD_PRIORITY_SCHEDULING` | 线程执行调度(实时)             |
| TSA    | `_POSIX_THREAD_ATTR_STACKADDR`      | 线程栈地址属性                 |
| TSH    | `_POSIX_THREAD_PROCESS_SHARED`      | 线程进程共享同步               |
| TSP    | `_POSIX_THREAD_SPORADIC_SERVER`     | 线程阵发性服务器(实时)         |
| TSS    | `_POSIX_THREAD_ATTR_STACKSIZE`      | 线程栈长度属性                 |
| TYM    | `_POSIX_TYPED_MEMORY_OBJECTS`       | 类型存储对象(实时)             |
| XSI    | `_XOPEN_UNIX`                       | X/Open 扩充接口                |

`pathconf` 和 `fpathconf` 的选项及 name 参数：

| 选项名                    | 说明                                   | name 参数              |
| ------------------------- | -------------------------------------- | ---------------------- |
| `_POSIX_CHOWN_RESTRICTED` | 使用 `chown` 是否是受限的              | `_PC_CHOWN_RESTRICTED` |
| `_POSIX_NO_TRUNC`         | 路径名长于 `NAMEMAX` 是否出错          | `_PC_NO_TRUNC`         |
| `_POSIX_VDISABLE`         | 若定义，可用此值禁用终端特殊字符       | `_PC_VDISABLE`         |
| `_POSIX_ASYNC_IO`         | 对相关联的文件是否可以使用异步 `I/O`   | `_PC_ASYNC_IO`         |
| `_POSIX_PRIO_IO`          | 对相关联的文件是否可以使用优先的 `I/O` | `_PC_PRIO_IO`          |
| `_POSIX_SYNC_IO`          | 对相关联的文件是否可以使用同步 `I/O`   | `_PC_SYNC_IO`          |
| `_POSIX2_SYMLINKS`        | 目录中是否支持符号链接                 | `_PC_2_SYMLINKS`       |

`sysconf` 的选项及 name 参数：

| 选项名                         | 说明                                              | name 参数                    |
| ------------------------------ | ------------------------------------------------- | ---------------------------- |
| `_POSIX_ASYNCHRONOUS_IO`       | 此实现是否支持 `POSIX` 异步 `I/O`                 | `_SC_ASYNCHRONOUS_IO`        |
| `_POSIX_BARRIERS`              | 此实现是否支持屏障                                | `_SC_BARRIERS`               |
| `_POSIX_CLOCK_SELECTION`       | 此实现是否支持时钟选择                            | `_SC_CLOCK_SELECTION`        |
| `_POSIX_JOB_CONTROL`           | 此实现是否支持作业控制                            | `_SC_JOB_CONTROL`            |
| `_POSIX_MAPPEDFILES`           | 此实现是否支持存储映像文件                        | `_SC_MAPPED_FILES`           |
| `_POSIX_MEMORY_PROTECTION`     | 此实现是否支持存储保护                            | `_SC_MEMORY_PROTECTION`      |
| `_POSIX_READER_WRITER_LOCKS`   | 此实现是否支持读者-写者锁                         | `_SC_READER_WRITER_LOCKS`    |
| `_POSIX_REALTIME_SIGNALS`      | 此实现是否支持实时信号                            | `_SC_REALTIME_SIGNALS`       |
| `_POSIX_SAVED_IDS`             | 此实现是否支持保存的设置用户 ID 和保存的设置组 ID | `_SC_SAVED_IDS`              |
| `_POSIX_SEMAPHORES`            | 此实现是否支持 `POSIX` 信号量                     | `_SC_SEMAPHORES`             |
| `_POSIX_SHELL`                 | 此实现是否支持 `POSIX shell`                      | `_SC_SHELL`                  |
| `_POSIXSPIN_LOCKS`             | 此实现是否支持旋转锁                              | `_SC_SPIN_LOCKS`             |
| `_POSIX_THREAD_SAFE_FUNCTIONS` | 此实现是否支持线程安全函数                        | `_SC_THREAD_SAFE_FUNCTIONS`  |
| `_POSIX_THREADS`               | 此实现是否支持线程                                | `_SC_THREADS`                |
| `_POSIX_TIMEOUTS`              | 此实现是否支持基于超时的变量选择函数              | `_SC_TIMEOUTS`               |
| `_POSIX_TIMERS`                | 此实现是否支持定时器                              | `_SC_TIMERS`                 |
| `_POSIX_VERSION`               | `POSIX.1` 版本                                    | `_SC_VERSION`                |
| `_XOPEN_CRYPT`                 | 此实现是否支持 `XSI` 加密可选组                   | `_SC_XOPEN_CRYPT`            |
| `_XOPEN_REALTIME`              | 此实现是否支持 `XSI` 实时选项组                   | `_SC_XOPEN_REALTIME`         |
| `_XOPEN_REALTIME_THREADS`      | 此实现是否支持实时线程选项组                      | `_SC_XOPEN_REALTIME_THREADS` |
| `_XOPEN_SHM`                   | 此实现是否支持 `XSI` 共享存储选项组               | `_SC_XOPEN_SHM`              |
| `_XOPEN_VERSION`               | `XSI` 版本                                        | `_SC_XOPEN_VERSION`          |

如同对限制的处理一样，`POSIX.1` 定义了 3 种处理选项的方法。

1. 编译时选项定义在 `<unistd.h>` 中。
1. 与文件或目录无关的运行时选项用 `sysconf` 函数来判断。
1. 与文件或目录有关的运行时选项通过调用 `pathconf` 或 `fpathconf` 函数来判断。

选项包括了上面三个表中的符号，如果符号常量未定义，则必须使用 `sysconf`、`pathconf` 或 `fpathconf` 来判断是否支持该选项。

对于每一个选项，有以下 3 种可能的平台支持状态。

1. 如果符号常量没有定义或者定义值为 −1，那么该平台在编译时并不支持相应选项。但是有一种可能，即在已支持该选项的新系统上运行老的应用时，即使该选项在应用编译时未被支持，但如今新系统运行时检查会显示该选项已被支持。
1. 如果符号常量的定义值大于 0，那么该平台支持相应选项。
1. 如果符号常量的定义值为 0，则必须调用 `sysconf`、`pathconf` 或 `fpathconf` 来判断相应选项是否受到支持。

如同系统限制一样，关于 `sysconf`、`pathconf` 和 `fpathconf` 如何处理选项，有如下几点值得注意。

1. `_SC_VERSION` 的返回值表示标准发布的年（以 4 位数表示）、月（以 2 位数表示）。该值可能是 `198808L`、`199009L`、`199506L` 或表示该标准后续版本的其他值。与 `SUSv3` （`POSIX.1 2001` 年版）相关连的值是 `200112L`，与 SUSv4（`POSIX.1 2008` 年版）相关连的值是 `200809L`。
1. `_SC_XOPEN_VERSION` 的返回值表示系统支持的 `XSI` 版本。与 `SUSv3` 相关联的值是 600，与 `SUSv4` 相关的值是 700。
1. `_SC_JOB_CONTROL`、`_SC_SAVED_IDS` 以及 `_PC_VDISABLE` 的值不再表示可选功能。虽然 `XPG4` 和 `SUS` 早期版本要求支持这些选项，但从 `SUSv3` 起，不再需要这些功能，但这些符号仍然被保留，以便向后兼容。
1. 符合 `POSIX.1-2008` 的平台还要求支持下列选项：
   - `_POSIX_ASYNCHRONOUS_IO`
   - `_POSIX_BARRIERS`
   - `_POSIX_CLOCK_SELECTION`
   - `_POSIX_MAPPED_FILES`
   - `_POSIX_MEMORY_PROTECTION`
   - `_POSIX_READER_WRITER_LOCKS`
   - `_POSIX_REALTIME_SIGNALS`
   - `_POSIX_SEMAPHORES`
   - `_POSIX_SPIN_LOCKS`
   - `_POSIX_THREAD_SAFE_FUNCTIONS`
   - `_POSIX_THREADS`
   - `_POSIX_TIMEOUTS`
   - `_POSIX_TIMERS`
     这些常量定义成具有值 `200809L`。相应的 `_SC` 符号同样是为了向后兼容而被保留下来的。
1. 如果对指定的 pathname 或 fd 已不再支持此功能，那么 `_PC_CHOWN_RESTRICTED` 和 `_PC_NO_TRUNC` 返回 −1，而 `errno` 不变，在所有符合 `POSIX` 的系统中，返回值将大于 0（表示该选项被支持）。
1. `_PC_CHOWN_RESTRICT` 引用的文件必须是一个文件或者是一个目录。如果是一个目录，那么返回值指明该选项是否可应用于该目录中的各个文件。
1. `_PC_NO_TRUNC` 和 `_PC_2_SYMLINKS` 引用的文件必须是一个目录。
1. `_PC_NO_TRUNC` 的返回值可用于目录中的各个文件名。
1. `_PC_VDISABLE` 引用的文件必须是一个终端文件。
1. `_PC_ASYNC_IO`、`_PC_PRIO_IO`和`_PC_SYNC_IO`引用的文件一定不能是一个目录。

## 功能测试宏

所有功能测试宏（feature test macro）都以下划线开始，常量 `_POSIX_C_SOURCE` 及 `_XOPEN_SOURCE` 就是功能测试宏。

如果在编译一个程序时，希望它只与 `POSIX` 的定义相关，而不与任何实现定义的常量冲突，那么就需要定义常量 `_POSIX_C_SOURCE` 。

当要使用功能测试宏时，通常在 `cc` 命令行中以下列方式定义：`cc -D_POSIX_C_SOURCE=200809L file.c`这使得 C 程序在包括任何头文件之前，定义了功能测试宏。
如果我们仅想使用 `POSIX.1` 定义，那么也可将源文件的第一行设置为：`#define_POSIX_C_SOURCE 200809L` 。

## 基本系统数据类型

头文件 `<sys/types.h>` 中定义了某些与实现有关的数据类型，它们被称为基本系统数据类型（primitive system data type）。还有很多这种数据类型定义在其他头文件中。在头文件中，这些数据类型都是用 C 的 `typedef` 来定义的。它们绝大多数都以 `_t` 结尾。用这种方式定义了这些数据类型后，就不再需要考虑因系统不同而变化的程序实现细节。

一些常用的基本系统数据类型：

| 类型           | 说明                                          |
| -------------- | --------------------------------------------- |
| `clock_t`      | 时钟滴答计数器(进程时间)                      |
| `comp_t`       | 压缩的时钟滴答(`POSIX1` 未定义)               |
| `dev_t`        | 设备号(主和次)                                |
| `fd_set`       | 文件描述符集                                  |
| `fpos_t`       | 文件位置                                      |
| `gid_t`        | 数值组 ID                                     |
| `ino_t`        | i 节点编号                                    |
| `mode_t`       | 文件类型，文件创建模式                        |
| `nlink_t`      | 目录项的链接计数                              |
| `off_t`        | 文件长度和偏移量(带符号的)(`lseek`)           |
| `pid_t`        | 进程 ID 和进程组 ID(带符号的)                 |
| `pthread_t`    | 线程 ID                                       |
| `ptrdiff_t`    | 两个指针相减的结果(带符号的)                  |
| `rlim_t`       | 资源限制                                      |
| `sig_atomic_t` | 能原子性地访问的数据类型                      |
| `sigset_t`     | 信号集                                        |
| `size_t`       | 对象(如字符串)长度(不带符号的)                |
| `ssize_t`      | 返回字节计数的函数(带符号的)(`read`、w`rite`) |
| `time_t`       | 日历时间的秒计数器                            |
| `uid_t`        | 数值用户 ID                                   |
| `wchar_t`      | 能表示所有不同的字符码                        |

## 标准之间的冲突

就整体而言，这些不同的标准之间配合得相当好。因为 `SUS` 基本说明和 `POSIX.1` 是同一个东西。需要关注的是 `ISO C` 标准和 `POSIX.1` 之间的差别。如果出现冲突，`POSIX.1` 服从 `ISO C` 标准。

`ISO C` 定义了 `clock` 函数，它返回进程使用的 CPU 时间，返回值是 `clock_t` 类型值，但 `ISO C` 标准没有规定它的单位。为了将此值变换成以秒为单位，需要将其除以在 `<time.h>` 头文件中定义的 `CLOCKS_PER_SEC`。
`POSIX.1` 定义了 `times` 函数，它返回其调用者及其所有终止子进程的 CPU 时间以及时钟时间，所有这些值都是 `clock_t` 类型值。`sysconf` 函数用来获得每秒滴答数，用于表示 `times` 函数的返回值。
`ISO C` 和 `POSIX.1` 用同一种数据类型（`clock_t`）来保存对时间的测量，但定义了不同的单位。其中 `clock` 返回微秒数（`CLOCK_PER_SEC` 是 100 万），而 `sysconf` 为每秒滴答数返回的值是 `100`。因此，我们在使用 `clock_t` 类型变量的时候，必须十分小心以免混淆不同的时间单位。

例子，使用 `clock` 获取运行时间：

```c
#include <stdio.h>
#include <time.h>

int main(int argc, char* argv[]){
	clock_t start_time, end_time;
	double cup_time_used;
	int sum = 0;
	start_time = clock();
	for(int i = 0; i < 1000000; i++){
		sum += i;
		printf("%d\n", sum);
	}
	end_time = clock();
	cup_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
	printf("CLOCKS_PER_SEC = %ld, cup_time_used = %lfs, sum = %d\n",CLOCKS_PER_SEC, cup_time_used, sum);
	return 0;
}

```

编译运行：

```bash
$ gcc clock.c
$ ./a.out
......
CLOCKS_PER_SEC = 1000000, cup_time_used = 3.305876s, sum = 1783293664
```

例子，使用 times 获取进程运行时间信息的 Linux 系统调用。

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>

int main(int argc, char* argv[]){
	struct tms start_time, end_time;
	clock_t start_clock, end_clock;
	int sum = 0;
	long ticks_per_second = sysconf(_SC_CLK_TCK);
	start_clock = times(&start_time);
	for(int i = 0; i < 1000000; i++){
		sum += i;
		printf("%d\n", sum); // 耗时
	}
	end_clock = times(&end_time);
	double user_time = (double)(end_time.tms_utime - start_time.tms_utime) / ticks_per_second;
	double system_time = (double)(end_time.tms_stime - start_time.tms_stime) / ticks_per_second;
	double cpu_time_used = (double)(end_time.tms_utime + end_time.tms_stime - start_time.tms_utime - start_time.tms_stime) / ticks_per_second;
	printf("ticks_per_second = %ld, cpu_time_used = %lf, sum = %d\n", ticks_per_second, cpu_time_used, sum);
	printf("用户模式时间：%f 秒\n", user_time);
	printf("系统模式时间：%f 秒\n", system_time);
	return 0;
}
```

编译运行：

```bash
$ gcc times.c
$ ./a.out
......
ticks_per_second = 100, cpu_time_used = 3.280000, sum = 1783293664
用户模式时间：0.100000 秒
系统模式时间：3.180000 秒
```

另一个可能产生冲突的地方是：在 `ISO C` 标准说明函数时，可能没有像 `POSIX.1` 那样严。在 `POSIX` 环境下，有些函数可能要求有一个与 C 环境下不同的实现，因为 `POSIX` 环境中有多个进程，而 `ISO C` 环境则很少考虑宿主操作系统。尽管如此，很多符合 `POSIX` 的系统为了兼容性也会实现 `ISO C` 函数。
