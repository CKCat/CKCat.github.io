---
title: unix环境高级编程16-网络IPC-套接字
date: 2023-04-10 17:23:26
tags: linux 编程
category: unix环境高级编程
---

## 套接字描述符

套接字是通信端点的抽象，正如使用文件描述符访问文件，应用程序用套接字描述符访问套接字。套接字描述符在 UNIX 系统中被当作是一种文件描述符，许多处理文件描述符的函数（如 `read` 和 `write`）可以用于处理套接字描述符。

为创建一个套接字，调用 `socket` 函数。

```c
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
```

返回值：

- 若成功，返回文件（套接字）描述符；
- 若出错，返回 −1。

参数：

- `domain`：指定套接字的地址家族或协议族。常见的地址家族包括：

  - `AF_INET`：IPv4 地址家族，用于 Internet 协议。
  - `AF_INET6`：IPv6 地址家族，用于新一代 Internet 协议。
  - `AF_UNIX` 或 `AF_LOCAL`：本地通信地址家族，用于本地进程间通信。
  - `AF_NETLINK`：Linux 内核通信地址家族。
  - `AF_UNSPEC`：可以代表“任何”域。

- `type`：指定套接字的类型，决定了套接字的特性。常见的套接字类型包括：
  - `SOCK_STREAM`：面向连接的流套接字，提供可靠的、双向的、基于字节流的通信。通常用于 `TCP` 协议。
  - `SOCK_DGRAM`：数据报套接字，提供无连接的、不可靠的、基于数据报的通信。通常用于 `UDP` 协议。
  - `SOCK_RAW`：原始套接字，用于访问底层协议数据包。
  - `SOCK_SEOPACKET`：固定长度的、有序的、可靠的、面向连接的报文传递。
- `protocol`：指定套接字使用的协议。通常可以设置为 0，这将根据地址家族和套接字类型自动选择合适的协议。例如，如果 `domain` 是 `AF_INET`，`type` 是 `SOCK_STREA`M，那么将使用 `TCP` 协议。下表列出了为因特网域套接字定义的协议。

  | 协议         | 描述                                                 |
  | ------------ | ---------------------------------------------------- |
  | IPPROTO_IP   | IPv4 网际协议                                        |
  | IPPROTO_PV6  | IPv6 网际协议(在 `POSIX.1` 中为可选)                 |
  | IPPROTO_ICMP | 因特网控制报文协议(Internet ControlMessage Protocol) |
  | IPPROTO_RAW  | 原始 IP 数据包协议(在 `POSIX.1` 中为可选)            |
  | IPPROTO_TCP  | 传输控制协议                                         |
  | IPPROTO_UDP  | 用户数据报协议(User Datagram Protocol)               |

虽然套接字描述符本质上是一个文件描述符，但不是所有参数为文件描述符的函数都可以接受套接字描述符。

下表列出了文件描述符函数使用套接字时的行为。

| 函数                   | 使用套接字时的行为                                                                                                   |
| ---------------------- | -------------------------------------------------------------------------------------------------------------------- |
| `close`                | 释放套接字                                                                                                           |
| `dup` 和 `dup2`        | 和一般文件描述符一样复制                                                                                             |
| `fchdir`               | 失败，并且将 `errno` 设置为 `ENOTDIR`                                                                                |
| `fchomod`              | 未指定                                                                                                               |
| `fchown`               | 由实现定义                                                                                                           |
| `fcntl`                | 支持一些命令，包括 `F_DUPFD`、`FDUPFD_CLOEXEC`、`F_GETED`、`F_GETFL`、`F_GETOWN`、`F_SETFD`、`F_SETFL` 和 `F_SETOWN` |
| `fdatasync` 和 `fsync` | 由实现定义                                                                                                           |
| `fstat`                | 支持一些 `stat` 结构成员，但如何支持由实现定义                                                                       |
| `ftruncate`            | 未指定                                                                                                               |
| `ioctl`                | 支持部分命令，依赖于底层设备驱动                                                                                     |
| `lseek`                | 由实现定义(通常失败时会将 `errno` 设为 `ESPIPE`)                                                                     |
| `mmap`                 | 未指定                                                                                                               |
| `poll`                 | 正常工作                                                                                                             |
| `pread` 和 `pwrite`    | 失败时会将 `errno` 设为 `ESPIPE`                                                                                     |
| `read`                 | 与没有任何标志位的 `recv` 等价                                                                                       |
| `select`               | 正常工作                                                                                                             |
| `write`                | 与没有任何标志位的 `send` 等价                                                                                       |

套接字通信是双向的。可以采用 `shutdown` 函数来禁止一个套接字的 `I/O`。

```c
#include <sys/socket.h>
int shutdown (int sockfd, int how);
```

返回值：

- 若成功，返回 0；
- 若出错，返回 −1。

参数：

- `sockfd`：要关闭的套接字的文件描述符。
- `how`：关闭方式，指定了关闭的行为。常见的取值包括：
  - `SHUT_RD`：关闭套接字的读取端（接收数据的一侧）。在此之后，套接字不能再接收数据，但可以继续发送数据。
  - `SHUT_WR`：关闭套接字的写入端（发送数据的一侧）。在此之后，套接字可以继续接收数据，但不能再发送数据。
  - `SHUT_RDWR`：同时关闭套接字的读取端和写入端。此操作等同于关闭整个套接字连接。

`shutdown` 允许使一个套接字处于不活动状态，和引用它的文件描述符数目无关。其次，有时可以很方便地关闭套接字双向传输中的一个方向。

## 寻址

目标通信进程标识由两部分组成。一部分是计算机的网络地址，它可以帮助标识网络上我们想与之通信的计算机；另一部分是该计算机上用端口号表示的服务，它可以帮助标识特定的进程。

### 字节序

与同一台计算机上的进程进行通信时，一般不用考虑字节序。网络协议指定了字节序，因此异构计算机系统能够交换协议信息而不会被字节序所混淆。`TCP/IP` 协议栈使用大端字节序。对于 `TCP/IP`，地址用网络字节序来表示，所以应用程序有时需要在处理器的字节序与网络字节序之间转换它们。

对于 `TCP/IP` 应用程序，有 4 个用来在处理器字节序和网络字节序之间实施转换的函数。

```c
#include <arpa/inet.h>
uint32_t htonl(uint32_t hostint32);
uint16_t htons(uint16_t hostint16);
uint32_t ntohl(uint32_t netint32);
uint16_t ntohs(uint16_t netint16);
```

4 个函数返回值：

- 以网络字节序表示的 32 位整数。
- 以网络字节序表示的 16 位整数。
- 以主机字节序表示的 32 位整数。
- 以主机字节序表示的 16 位整数。

h 表示“主机”字节序，n 表示“网络”字节序。l 表示“长”（即 4 字节）整数，s 表示“短”（即 4 字节）整数。对于系统来说，把这些函数实现为宏也是很常见的。

### 地址格式

一个地址标识一个特定通信域的套接字端点，地址格式与这个特定的通信域相关。为使不同格式地址能够传入到套接字函数，地址会被强制转换成一个通用的地址结构 `sockaddr`：

```c
struct sockaddr {
    sa_family_t sa_family; /* address family */
    char sa_data[];        /* variable-length address */
    ...
};
```

套接字实现可以自由地添加额外的成员并且定义 `sa_data` 成员的大小。在 Linux 中，该结构定义如下：

```c
struct sockaddr {
    sa_family_t sa_family;   /* address family */
    char sa_data[14];        /* variable-length address */
};
```

因特网地址定义在 `<netinet/in.h>` 头文件中。在 IPv4 因特网域（`AF_INET`）中，套接字地址用结构 `sockaddr_in` 表示：

```c
struct in_addr {
    in_addr_t s_addr;         /* IPv4 address */
};
struct sockaddr_in {
    sa_family_t sin_family;   /* address family */
    in_port_t sin_port;       /* port number */
    struct in_addr sin_addr;  /* IPv4 address */
};
```

数据类型 `in_port_t` 定义成 `uint16_t`。数据类型 `in_addr_t` 定义成 `uint32_t`。这些整数类型在 `<stdint.h>` 中定义并指定了相应的位数。

与 `AF_INET` 域相比较，IPv6 因特网域（`AF_INET6`）套接字地址用结构 `sockaddr_in6` 表示：

```c
struct_in6_addr {
    uint8_t s6_addr[16];         /* IPv6 address */
};
struct sockaddr_in6 {
    sa_family_t sin6_family;     /* address family */
    in_port_t sin6_port;         /* port number */
    uint32_t sin6_flowinfo;      /* traffic class and flow info */
    struct in6_addr sin6_addr;   /* IPv6 address*/
    uint32_t sin6_scope_id;      /* set of interfaces for scope */
};
```

这些都是 `Single UNIX Specification` 要求的定义。每个实现可以自由添加更多的字段。在 Linux 中，`sockaddr_in` 定义如下：

```c
struct sockaddr_in {
    sa_family_t sin_family;     /* address family */
    in_port_t sin_port;         /* port number */
    struct in6_addr sin6_addr;  /* IPv4 address */
    unsigned char sin_zero[8];  /* filler */
};
```

尽管 `sockaddr_in` 与 `sockaddr_in6` 结构相差比较大，但它们均被强制转换成 `sockaddr` 结构输入到套接字例程中。

有时，需要打印出能被人理解而不是计算机所理解的地址格式。有两个新函数 `inet_ntop` 和 `inet_pton` 具有相似的功能，而且同时支持 IPv4 地址和 IPv6 地址。

```c
#include <arpa/inet.h>
const char *inet_ntop(int domain, const void *restrict addr, char *restrict str, socklen_t size);
```

返回值：

- 若成功，返回地址字符串指针；
- 若出错，返回 `NULL` 。

参数：

- `domain`：地址家族，可以是 `AF_INET`（IPv4）或 `AF_INET6`（IPv6）。
- `addr`：指向以人类可读形式表示的 IP 地址字符串的指针。
- `str`：指向一个缓冲区的指针，用于存储转换后的字符串形式的 IP 地址。
- `size`：`str` 缓冲区的大小。

函数 `inet_ntop` 将网络字节序的二进制地址转换成文本字符串格式。

```c
int inet_pton(int domain, const char * restrict str, void *restrict addr);
```

返回值：

- 若成功，返回 1；若格式无效，返回 0；
- 若出错，返回 −1 。

参数：

- `domain`：地址家族，可以是 `AF_INET`（IPv4）或 `AF_INET6`（IPv6）。
- `addr`：指向以人类可读形式表示的 IP 地址字符串的指针。
- `str`：指向要存储转换后 IP 地址的内存位置的指针。

`inet_pton` 将文本字符串格式转换成网络字节序的二进制地址。

### 地址查询

理想情况下，应用程序不需要了解一个套接字地址的内部结构。如果一个程序简单地传递一个类似于 `sockaddr` 结构的套接字地址，并且不依赖于任何协议相关的特性，那么可以与提供相同类型服务的许多不同协议协作。

通过调用 `gethostent`，可以找到给定计算机系统的主机信息。

```c
#include <netdb.h>
struct hostent *gethostent(void);
```

返回值：

- 若成功，返回指针；
- 若出错，返回 `NULL` 。

`gethostent` 函数用于获取主机数据库中的下一个主机条目。它通常与 `sethostent` 配合使用。在使用前，你需要首先调用 `sethostent` 来初始化主机数据库的读取。

```c
void sethostent(int stayopen);
void endhostent(void);
```

函数 `sethostent` 会打开文件，如果文件已经被打开，那么将其回绕。当 `stayopen` 参数用于指定是否应该从头开始读取主机数据库文件，0 表示不从头开始，非 0 表示从头开始。

函数 `endhostent` 用于关闭主机数据库文件，释放相关资源，通常在使用完 `gethostent` 之后调用进行清理资源。

当 `gethostent` 返回时，会得到一个指向 `hostent` 结构的指针，该结构可能包含一个静态的数据缓冲区，每次调用 `gethostent`，缓冲区都会被覆盖。`hostent` 结构至少包含以下成员：

```c
struct hostent{
    char *h_name;         /* name of host */
    char **h_aliases;     /* pointer to alternate host name array */
    int h_addrtype;       /* address type */
    int h_length;         /* length in bytes of address */
    char **h_addr_list;   /* pointer to array of network addresses */
    ...
};
```

返回的地址采用网络字节序。

另外两个函数 `gethostbyname` 和 `gethostbyaddr`，原来包含在 `hostent` 函数中，现在则被认为是过时的。

能够采用一套相似的接口来获得网络名字和网络编号。

```c
#include <netdb.h>
struct netent *getnetbyaddr (uint32_t net, int type);
struct netent *getnetbyname(const char *name);
struct netent *getnetent(void);
void setnetent(int stayopen);
void endnetent(void);
```

前 3 个函数的返回值：

- 若成功，返回指针；
- 若出错，返回 `NULL`。

`getnetbyaddr` 用于根据给定的网络号获取网络数据库中的网络条目。
`getnetbyname` 用于根据给定的网络名获取网络数据库中的网络条目。
`getnetent` 用于获取网络数据库中的下一个网络条目。
`setnetent` 用于打开网络数据库文件以供读取。
`endnetent` 用于关闭网络数据库文件，释放相关资源。

`netent` 结构至少包含以下字段：

```c
struct netent {
    char *n_name;     /* network name */
    char **n_aliases; /* alternate network name array pointer */
    int n_addrtype;   /* address type */
    uint32_t n_net;   /* network number */
    ...
};
```

网络编号按照网络字节序返回。地址类型是地址族常量之一（如 `AF_INET`）。

我们可以用以下函数在协议名字和协议编号之间进行映射。

```c
#include <netdb.h>
struct protoent *getprotobyname(const char *name);
struct protoent *getprotobynumber(int proto);
struct protoent *getprotoent(void);
void setprotoent(int stayopen);
void endprotoent(void);
```

前 3 个函数的返回值：

- 若成功，返回指针；
- 若出错，返回 `NULL`。

`getprotobyname` 用于根据给定的协议名获取协议数据库中的协议条目。
`getprotobynumber` 用于根据给定的协议号获取协议数据库中的协议条目。
`getprotoent` 用于获取协议数据库中的下一个协议条目。
`setprotoent` 用于打开协议数据库文件以供读取。
`endprotoent` 用于关闭协议数据库文件，释放相关资源。

`POSIX.1` 定义的 `protoent` 结构至少包含以下成员：

```c
struct protoent {
    char *p_name;         /* protocol name */
    char **p_ aliases;    /* pointer to altername protocol name array */
    int p_proto;          /* protocol number */
    ...
};
```

服务是由地址的端口号部分表示的。每个服务由一个唯一的众所周知的端口号来支持。可以使用函数 `getservbyname` 将一个服务名映射到一个端口号，使用函数 `getservbyport` 将一个端口号映射到一个服务名，使用函数 `getservent` 顺序扫描服务数据库。

```c
#include <netdb.h>
struct servent *getservbyname(const char *name, const char *proto);
struct servent *getserbyport(int port, const char *proto);
struct servent *getservent(void);
void setservent(int stayopen);
void endservent(void);
```

前 3 个函数的返回值：

- 若成功，返回指针，
- 若出错，返回 `NULL`。

`servent` 结构至少包含以下成员：

```c
struct servent{
char *s_name; /* service name */
char **s_aliases; /* pointer to alternate service name array */
int s_port; /* port number */
char *s_proto; /* name of protocol */
...
};
```

`POSIX.1` 定义了若干新的函数，允许一个应用程序将一个主机名和一个服务名映射到一个地址，或者反之。这些函数代替了较老的函数 `gethostbyname` 和 `gethostbyaddr`。

`getaddrinfo` 函数允许将一个主机名和一个服务名映射到一个地址。

```c
#include <sys/socket.h>
#include <netdb.h>
int getaddrinfo(const char *restrict host, const char *restrict service,
                const struct addrinfo *restrict hint, struct addrinfo **restrict res);
void freeaddrinfo(struct addrinfo *ai);
```

`getaddrinfo` 返回值：

- 若成功，返回 0；
- 若出错，返回非 0 错误码。

`getaddrinfo` 参数：

- `host`：要查询的主机名或 IP 地址。可以是主机名、IPv4 地址或 IPv6 地址的字符串形式，或者是 `NULL`。
- `service`：要查询的服务名或端口号。通常是端口号的字符串形式，也可以是已知的服务名，如 "http"。可以为 `NULL`。
- `hint`：一个指向 `struct addrinfo` 结构的指针，用于过滤地址的模板，包括 `ai_family`、`ai_flags`、`ai_protocol` 和 `ai_socktype` 字段。剩余的整数字段必须设置为 0，指针字段必须为空。
- `res`：一个指向 `struct addrinfo` 结构指针的指针。如果函数成功，将在这里返回一个指向地址信息链表的指针。

`getaddrinfo` 函数返回一个链表结构 `addrinfo`。可以用 `freeaddrinfo` 来释放一个或多个这种结构，这取决于用 `ai_next` 字段链接起来的结构有多少。

`addrinfo` 结构的定义至少包含以下成员：

```c
struct addrinfo {
    int ai_flags; /* customize behavior */
    int ai_family; /* address family */
    int ai_socktype; /* socket type */
    int ai_protocol; /* protocol */
    socklen_t ai_addrlen; /* length in bytes of address */
    struct sockaddr *ai_addr; /* address */
    char *ai_canonname; /* canonical name of host */
    struct addrinfo *ai_next; /* next in list */
    ...
};
```

`addrinfo` 结构的标志

| 标志             | 描述                                                  |
| ---------------- | ----------------------------------------------------- |
| `AI_ADDRCONFIG`  | 查询配置的地址类型(IPv4 或 IPv6)                      |
| `AI_ALL`         | 查找 IPv4 和 IPv6 地址(仅用于 `AI_V4MAPPED`)          |
| `AI_CANONNAME`   | 需要一个规范的名字(与别名相对)                        |
| `AI_NUMERICHOST` | 以数字格式指定主机地址，不翻译                        |
| `AI_NUMERICSERV` | 将服务指定为数字端口号，不翻译                        |
| `AI_PASSIVE`     | 套接字地址用于监听绑定                                |
| `AI_V4MAPPED`    | 如没有找到 IPv6 地址，返回映射到 IPv6 格式的 IP4 地址 |

如果 `getaddrinfo` 失败，不能使用 `perror` 或 `strerror` 来生成错误消息，而是要调用 `gai_strerror` 将返回的错误码转换成错误消息。

```c
#include <netdb.h>
const char *gai_strerror(int error);
```

返回值：

- 指向描述错误的字符串的指针。

`getnameinfo` 函数将一个地址转换成一个主机名和一个服务名。

```c
#include <sys/socket.h>
#include <netdb.h>
int getnameinfo(const struct sockaddr *restrict addr, socklen_t alen, char *restrict host,
                socklen_t hostlen, char *restrict service, socklen_t servlen, int flags);
```

返回值：

- 若成功，返回 0；
- 若出错，返回非 0 值。

参数：
`addr`：指向包含地址信息的 `struct sockaddr` 结构的指针。
`alen`：`addr` 结构的大小（以字节为单位）。
`host`：一个字符数组，用于存储主机名。
`hostlen`：`host` 数组的大小（以字节为单位）。
`service`：一个字符数组，用于存储服务名（通常是端口号）。
`servlen`：`service` 数组的大小（以字节为单位）。
`flags`：一组标志，可以影响解析的行为。

下表为 `flags` 参数提供的一些控制转换的方式。

| 标志              | 描述                                       |
| ----------------- | ------------------------------------------ |
| `NI_DGRAM`        | 服务基于数据报而非基于流                   |
| `NI_NAMEREQD`     | 如果找不到主机名，将其作为一个错误对待     |
| `NI_NOFODN`       | 对于本地主机，仅返回全限定域名的节点名部分 |
| `NI_NUMERICHOST`  | 返回主机地址的数字形式，而非主机名         |
| `NI_NUMERICSCOPE` | 对于 IPv6，返回范围 I 的数字形式，而非名字 |
| `NI_NUMERICSERV`  | 返回服务地址的数字形式(即端口号)，而非名字 |

例子，打印主机和服务信息。

```c
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>

void print_family(struct addrinfo *aip)
{
    printf(" family ");
    switch (aip->ai_family)
    {
    case AF_INET:
        printf("inet");
        break;
    case AF_INET6:
        printf("inet6");
        break;
    case AF_UNIX:
        printf("unix");
        break;
    case AF_UNSPEC:
        printf("unspecified");
        break;
    default:
        printf("unknown");
    }
}

void print_type(struct addrinfo *aip)
{
    printf(" type ");
    switch (aip->ai_socktype)
    {
    case SOCK_STREAM:
        printf("stream");
        break;
    case SOCK_DGRAM:
        printf("datagram");
        break;
    case SOCK_SEQPACKET:
        printf("seqpacket");
        break;
    case SOCK_RAW:
        printf("raw");
        break;
    default:
        printf("unknown (%d)", aip->ai_socktype);
    }
}

void print_protocol(struct addrinfo *aip)
{
    printf(" protocol ");
    switch (aip->ai_protocol)
    {
    case 0:
        printf("default");
        break;
    case IPPROTO_TCP:
        printf("TCP");
        break;
    case IPPROTO_UDP:
        printf("UDP");
        break;
    case IPPROTO_RAW:
        printf("RAW");
        break;
    default:
        printf("unknown (%d)", aip->ai_protocol);
    }
}

void print_flags(struct addrinfo *aip)
{
    printf(" flags ");
    if (aip->ai_flags == 0)
        printf("0");
    else
    {
        if (aip->ai_flags & AI_PASSIVE)
            printf("passive");
        if (aip->ai_flags & AI_CANONNAME)
            printf("canon");
        if (aip->ai_flags & AI_NUMERICHOST)
            printf("mumhost");
        if (aip->ai_flags & AI_NUMERICSERV)
            printf("numserv");
        if (aip->ai_flags & AI_V4MAPPED)
            printf("v4mapped");
        if (aip->ai_flags & AI_ALL)
            printf("all");
    }
}

int main(int argc, char const *argv[])
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    struct sockaddr_in *sinp;
    const char *addr;
    int err;
    char abuf[INET_ADDRSTRLEN];

    if(argc != 3){
        printf("usage: %s nodename service\n", argv[0]);
        exit(1);
    }
    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = 0;
    hint.ai_socktype = 0;
    hint.ai_protocol = 0;
    hint.ai_addrlen = 0;
    hint.ai_addr = NULL;
    hint.ai_canonname = NULL;
    hint.ai_next = NULL;
    if ((err = getaddrinfo(argv[1], argv[2], &hint, &ailist)) != 0){
        printf("getaddrinfo() error: %s\n", gai_strerror(err));
        exit(1);
    }
    for(aip = ailist; aip != NULL; aip = aip->ai_next){
        print_flags(aip);
        print_family(aip);
        print_type(aip);
        print_protocol(aip);
        printf("\n\thost %s", aip->ai_canonname?aip->ai_canonname:"-");
        if(aip->ai_family == AF_INET){
            sinp = (struct sockaddr_in *)aip->ai_addr;
            addr = inet_ntop(AF_INET, &sinp->sin_addr, abuf, INET_ADDRSTRLEN);
            printf(" address %s", addr?addr:"unknown");
            printf(" port %d", ntohs(sinp->sin_port));
        }
        printf("\n");
    }
    exit(0);
}
```

这个程序说明了 `getaddrinfo` 函数的使用方法。如果有多个协议为指定的主机提供给定的服务，程序会打印出多条信息。本实例仅打印了与 IPv4 一起工作的那些协议（`ai_family` 为 `AF_INET`）的地址信息。

### 将套接字与地址关联

将一个客户端的套接字关联上一个地址没有多少新意，可以让系统选一个默认的地址。然而，对于服务器，需要给一个接收客户端请求的服务器套接字关联上一个众所周知的地址。客户端应有一种方法来发现连接服务器所需要的地址，最简单的方法就是服务器保留一个地址并且注册在 `/etc/services` 或者某个名字服务中。使用 `bind` 函数来关联地址和套接字。

```c
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr *addr, socklen_t len);
```

返回值：

- 若成功，返回 0；
- 若出错，返回 −1。

参数：

- `sockfd`：表示要绑定的套接字的文件描述符。
- `addr`：指向一个 `sockaddr` 结构体的指针，包含了要绑定的网络地址信息。
- `len`：表示 `addr` 结构体的大小。

对于使用的地址有以下一些限制。

- 在进程正在运行的计算机上，指定的地址必须有效；不能指定一个其他机器的地址。
- 地址必须和创建套接字时的地址族所支持的格式相匹配。
- 地址中的端口号必须不小于 1024，除非该进程具有相应的特权（即超级用户）。
- 一般只能将一个套接字端点绑定到一个给定地址上，尽管有些协议允许多重绑定。

对于因特网域，如果指定 IP 地址为 `INADDR_ANY`（`<netinet/in.h>`中定义的），套接字端点可以被绑定到所有的系统网络接口上。这意味着可以接收这个系统所安装的任何一个网卡的数据包。如果调用 `connect` 或 `listen`，但没有将地址绑定到套接字上，系统会选一个地址绑定到套接字上。

可以调用 `getsockname` 函数来发现绑定到套接字上的地址。

```c
#include <sys/socket.h>
int getsockname(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict alenp);
```

返回值：

- 若成功，返回 0；
- 若出错，返回 −1。

调用 `getsockname` 之前，将 `alenp` 设置为一个指向整数的指针，该整数指定缓冲区 `sockaddr` 的长度。返回时，该整数会被设置成返回地址的大小。如果地址和提供的缓冲区长度不匹配，地址会被自动截断而不报错。如果当前没有地址绑定到该套接字，则其结果是未定义的。

如果套接字已经和对等方连接，可以调用 `getpeername` 函数来找到对方的地址。

```c
#include <sys/socket.h>
int getpeername(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict alenp);
```

返回值：

- 若成功，返回 0；
- 若出错，返回 −1。

除了返回对等方的地址，函数 `getpeername` 和 `getsockname` 一样。

## 建立连接

如果要处理一个面向连接的网络服务（`SOCK_STREAM` 或 `SOCK_SEQPACKET`），那么在开始交换数据以前，需要在请求服务的进程套接字（客户端）和提供服务的进程套接字（服务器）之间建立一个连接。使用 `connect` 函数来建立连接。

```c
#include <sys/socket.h>
int connect(int sockfd, const struct sockaddr *addr, socklen_t len);
```

返回值：

- 若成功，返回 0；
- 若出错，返回 −1。

参数：

- `sockfd`：表示要连接的套接字的文件描述符。
- `addr`：指向一个 `sockaddr` 结构体的指针，包含了要连接的远程主机的网络地址信息。
- `len`：表示 `addr` 结构体的大小。

在 `connect` 中指定的地址是我们想与之通信的服务器地址。如果 `sockfd` 没有绑定到一个地址，`connect` 会给调用者绑定一个默认地址。

当尝试连接服务器时，出于一些原因，连接可能会失败。要想一个连接请求成功，要连接的计算机必须是开启的，并且正在运行，服务器必须绑定到一个想与之连接的地址上，并且服务器的等待连接队列要有足够的空间。因此，应用程序必须能够处理 `connect` 返回的错误，这些错误可能是由一些瞬时条件引起的。

例子，支持重试的 `connect`。

```c
#include <sys/socket.h>
#include <unistd.h>

#define MAXSLEEP 128

int connect_retry(int sockfd, const struct sockaddr *addr, socklen_t alen){
	int numsec;
	// 尝试使用指数补偿算法进行 connect 操作
	for(numsec = 1; numsec <= MAXSLEEP; numsec <<=1){
		if(connect(sockfd, addr, alen) == 0){
			return 0;
		}
		// 如果 connect 失败，再次尝试之前延迟。
		if(numsec <= MAXSLEEP/2)
			sleep(numsec);
	}
	return -1;
}
```

这个函数展示了指数补偿（exponential backoff）算法。如果调用 `connect` 失败，进程会休眠一小段时间，然后进入下次循环再次尝试，每次循环休眠时间会以指数级增加，直到最大延迟为 2 分钟左右。

例子，可迁移的支持重试的连接代码。

```c
#include <sys/socket.h>
#include <unistd.h>

#define MAXSLEEP 128

int connect_retry(int domain, int type, int protocol,
				const struct sockaddr *addr, socklen_t alen){
	int numsec, fd;
	// 尝试使用指数退避算法进行 connect 操作
	for(numsec = 1; numsec <= MAXSLEEP; numsec <<=1){
		if((fd = socket(domain, type, protocol)) < 0)
			return -1;
		if(connect(fd, addr, alen) == 0){
			return fd;
		}
		close(fd);
		// 如果 connect 失败，再次尝试之前延迟。
		if(numsec <= MAXSLEEP/2)
			sleep(numsec);
	}
	return -1;
}
```

需要注意的是，因为可能要建立一个新的套接字，给 `connect_retry` 函数传递一个套接字描述符参数是没有意义。我们现在返回一个已连接的套接字描述符给调用者，而并非返回一个表示调用成功的值。

如果套接字描述符处于非阻塞模式，那么在连接不能马上建立时，`connect` 将会返回 −1 并且将 `errno` 设置为特殊的错误码 `EINPROGRESS`。应用程序可以使用 `poll` 或者 `select` 来判断文件描述符何时可写，如果可写，连接完成。

`connect` 函数还可以用于无连接的网络服务（`SOCK_DGRAM`）。这看起来有点矛盾，实际上却是一个不错的选择。如果用 `SOCK_DGRAM` 套接字调用 `connect`，传送的报文的目标地址会设置成 `connect` 调用中所指定的地址，这样每次传送报文时就不需要再提供地址。另外，仅能接收来自指定地址的报文。

服务器调用 `listen` 函数来宣告它愿意接受连接请求。

```c
#include <sys/socket.h>
int listen(int sockfd, int backlog);
```

返回值：

- 若成功，返回 0；
- 若出错，返回 −1。

参数 `backlog` 提供了一个提示，提示系统该进程所要入队的未完成连接请求数量。其实际值由系统决定，但上限由 `<sys/socket.h>` 中的 `SOMAXCONN` 指定。

一旦服务器调用了 `listen`，所用的套接字就能接收连接请求。使用 `accept` 函数获得连接请求并建立连接。

```c
#include <sys/socket.h>
int accept(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict len);
```

返回值：

- 若成功，返回文件（套接字）描述符；
- 若出错，返回 −1。

参数：

- `sockfd`：服务器套接字的文件描述符，表示服务器正在监听的套接字。
- `addr`：指向用于存储客户端地址信息的结构体指针，通常是 `struct sockaddr` 类型。
- `len`：指向 `addr` 结构体长度的指针。在调用 `accept` 之前，`len` 应该初始化为 `struct sockaddr` 结构体的大小。

如果不关心客户端标识，可以将参数 `addr` 和 `len` 设为 `NULL`。

函数 `accept` 所返回的文件描述符是套接字描述符，该描述符连接到调用 `connect` 的客户端。这个新的套接字描述符和原始套接字（`sockfd`）具有相同的套接字类型和地址族。传给 `accept` 的原始套接字没有关联到这个连接，而是继续保持可用状态并接收其他连接请求。

如果没有连接请求在等待，`accept` 会阻塞直到一个请求到来。如果 `sockfd` 处于非阻塞模式，`accept` 会返回 −1，并将 `errno` 设置为 `EAGAIN` 或 `EWOULDBLOCK`。

如果服务器调用 `accept`，并且当前没有连接请求，服务器会阻塞直到一个请求到来。另外，服务器可以使用 `poll` 或 `select` 来等待一个请求的到来。在这种情况下，一个带有等待连接请求的套接字会以可读的方式出现。

例子，初始化一个套接字端点供服务器进程使用。

```c
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

int initserver(int type, const struct sockaddr *addr, socklen_t alen, int qlen){
	int fd;
	int err = 0;

	if((fd = socket(addr->sa_family, type, 0)) < 0)
		return -1;
	if(bind(fd, addr, alen) < 0)
		goto errout;
	if(type == SOCK_STREAM || type == SOCK_SEQPACKET){
		if(listen(fd, qlen) < 0)
			goto errout;
	}
	return fd;
errout:
	err = errno;
	close(fd);
	errno = err;
	return -1;
}
```

## 数据传输

既然一个套接字端点表示为一个文件描述符，那么只要建立连接，就可以使用 `read` 和 `write` 来通过套接字通信。在套接字描述符上使用 `read` 和 `write` 是非常有意义的，因为这意味着可以将套接字描述符传递给那些原先为处理本地文件而设计的函数。而且还可以安排将套接字描述符传递给子进程，而该子进程执行的程序并不了解套接字。

最简单的是 `send`，它和 `write` 很像，但是可以指定标志来改变处理传输数据的方式。

```c
#include <sys/socket.h>
ssize_t send(int sockfd, const void *buf, size_t nbytes, int flags);
```

返回值：

- 若成功，返回发送的字节数；
- 若出错，返回 −1。

类似 `write`，使用 `send` 时套接字必须已经连接。参数 `buf` 和 `nbytes` 的含义与 `write` 中的一致。与 `write` 不同的是，`send` 支持第 4 个参数 `flags`。

`send` 套接字调用标志：

| 标志            | 描述                                    | POSIX.1 | FreeBSD8.0 | Linux3.2.0 | Mac OSX 10.6.8 |
| --------------- | --------------------------------------- | ------- | ---------- | ---------- | -------------- |
| `MSG_CONFIRM`   | 提供链路层反馈以保持地址映射有效        |         |            | •          |                |
| `MSG_DONTROUTE` | 勿将数据包路由出本地网络                |         | •          | •          | •              |
| `MSG_DONTWAIT`  | 允许非阻塞操作(等价于使用 O_NONBLOCK)   |         | •          | •          | •              |
| `MSG_EOF`       | 发送数据后关闭套接字的发送端            |         | •          |            | •              |
| `MSG_EOR`       | 如果协议支持，标记记录结束              | •       | •          | •          | •              |
| `MSG_MORE`      | 延迟发送数据包允许写更多数据            |         |            | •          |                |
| `MSG_NOSIGNAL`  | 在写无连接的套接字时不产生 SIGPIPE 信号 | •       | •          | •          |                |
| `MSG_OOB`       | 如果协议支持，发送带外数据              | •       | •          | •          | •              |

即使 `send` 成功返回，也并不表示连接的另一端的进程就一定接收了数据。我们所能保证的只是当 `send` 成功返回时，数据已经被无错误地发送到网络驱动程序上。

对于支持报文边界的协议，如果尝试发送的单个报文的长度超过协议所支持的最大长度，那么 `send` 会失败，并将 `errno` 设为 `EMSGSIZE`。对于字节流协议，`send` 会阻塞直到整个数据传输完成。函数 `sendto` 和 `send` 很类似。区别在于 `sendto` 可以在无连接的套接字上指定一个目标地址。

```c
#include <sys/socket.h>
ssize_t sendto(int sockfd, const void *buf, size_t nbytes, int flags,
                const struct sockaddr *destaddr, socklen_t destlen);
```

返回值：

- 若成功，返回发送的字节数；
- 若出错，返回 −1。

参数：

- `sockfd`：发送数据的套接字文件描述符。
- `buf`：指向包含要发送数据的缓冲区的指针。
- `nbytes`：要发送的数据的字节数。
- `flags`：控制发送操作的行为的标志，通常设置为 0。
- `destaddr`：指向目标地址信息的结构体指针，通常是 `struct sockaddr` 类型。对于 UDP，通常是 `struct sockaddr_in` 类型。
- `destlen`：destaddr 结构体的长度。

对于面向连接的套接字，目标地址是被忽略的，因为连接中隐含了目标地址。对于无连接的套接字，除非先调用 `connect` 设置了目标地址，否则不能使用 `send`。`sendto` 提供了发送报文的另一种方式。

通过套接字发送数据时，还有一个选择。可以调用带有 `msghdr` 结构的 `sendmsg` 来指定多重缓冲区传输数据，这和 `writev` 函数很相似。

```c
#include <sys/socket.h>
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
```

返回值：

- 若成功，返回发送的字节数；
- 若出错，返回 −1。

参数：

- `sockfd`：发送数据的套接字文件描述符。
- `msg`：指向 `struct msghdr` 结构体的指针，该结构体包含了要发送的消息的相关信息，包括缓冲区数组和控制信息等。
- `flags`：控制发送操作的行为的标志，通常设置为 0。

POSIX.1 定义了 `msghdr` 结构，它至少有以下成员：

```c
struct msghdr {
    void *msg_name;             /* optional address */
    socklen_t msg_namelen;      /* address size in bytes */
    struct iovec *msg_iov;      /* array of I/O buffers */
    int msg_iovlen;             /* number of elements in array */
    void *msg_control;          /* ancillary data */
    socklen_t msg_controllen;   /* number of ancillary bytes */
    int msg_flags;              /* flags for received message */
};
```

函数 `recv` 和 `read` 相似，但是 `recv` 可以指定标志来控制如何接收数据。

```c
#include <sys/socket.h>
ssize_t recv(int sockfd, void *buf, size_t nbytes, int flags);
```

返回值：

- 返回数据的字节长度；若无可用数据或对等方已经按序结束，返回 0；
- 若出错，返回 −1。

参数：

- `sockfd`：接收数据的套接字文件描述符。
- `buf`：指向接收数据的缓冲区的指针。
- `nbytes`：接收缓冲区的大小。
- `flags`：控制接收操作的行为的标志，通常设置为 0。

`recv` 套接字调用标志：

| 标志               | 描述                                                 | POSIX.1 | FreeBSD8.0 | Linux3.2.0 | Mac OSX 10.6.8 |
| ------------------ | ---------------------------------------------------- | ------- | ---------- | ---------- | -------------- |
| `MSG_CMSG_CLOEXEC` | 为 UNIX 域套接字上接收的文件描述符设置执行时关闭标志 |         |            |            |                |
| `MSG_DONTWAIT`     | 启用非阻塞操作(相当于使用 `O_NONBLOCK`)              |         | •          | •          |                |
| `MSG_ERRQUEUE`     | 接收错误信息作为辅助数据                             |         |            | •          |                |
| `MSG_OOB`          | 如果协议支持，获取带外数据                           | •       | •          | •          | •              |
| `MSG_PEEK`         | 返回数据包内容而不真正取走数据包                     | •       | •          | •          | •              |
| `MSG_TRUNC`        | 即使数据包被截断，也返回数据包的实际长度             |         |            | •          |                |
| `MSG_WAITALL`      | 等待直到所有的数据可用(仅 `SOCK_STREAM`)             | •       | •          | •          | •              |

当指定 `MSG_PEEK` 标志时，可以查看下一个要读取的数据但不真正取走它。当再次调用 `read` 或其中一个 `recv` 函数时，会返回刚才查看的数据。

对于 `SOCK_STREAM` 套接字，接收的数据可以比预期的少。`MSG_WAITALL` 标志会阻止这种行为，直到所请求的数据全部返回，`recv` 函数才会返回。对于 `SOCK_DGRAM` 和 `SOCK_SEQPACKET` 套接字，`MSG_WAITALL` 标志没有改变什么行为，因为这些基于报文的套接字类型一次读取就返回整个报文。

如果发送者已经调用 `shutdown` 来结束传输，或者网络协议支持按默认的顺序关闭并且发送端已经关闭，那么当所有的数据接收完毕后，`recv` 会返回 0。

如果有兴趣定位发送者，可以使用 `recvfrom` 来得到数据发送者的源地址。

```c
#include <sys/socket.h>
ssize_t recvfrom(int sockfd, void *restrict buf, size_t len, int flags,
                struct sockaddr *restrict addr, socklen_t *restrict addrlen);
```

返回值：

- 返回数据的字节长度；若无可用数据或对等方已经按序结束，返回 0；
- 若出错，返回 −1

如果 `addr` 非空，它将包含数据发送者的套接字端点地址。当调用 `recvfrom` 时，需要设置 `addrlen` 参数指向一个整数，该整数包含 `addr` 所指向的套接字缓冲区的字节长度。返回时，该整数设为该地址的实际字节长度。

因为可以获得发送者的地址，`recvfrom` 通常用于无连接的套接字。否则，recvfrom 等同于 recv。

为了将接收到的数据送入多个缓冲区，类似于 `readv` ，或者想接收辅助数据，可以使用 `recvmsg`。

```c
#include <sys/socket.h>
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
```

返回值：

- 返回数据的字节长度；若无可用数据或对等方已经按序结束，返回 0；
- 若出错，返回 −1。

`recvmsg` 用 `msghdr` 结构（在 `sendmsg` 中见到过）指定接收数据的输入缓冲区。可以设置参数 `flags` 来改变 `recvmsg` 的默认行为。返回时，`msghdr` 结构中的 `msg_flags` 字段被设为所接收数据的各种特征。（进入 `recvmsg` 时 `msg_flags` 被忽略。）

从 `recvmsg` 中返回的 `msg_flags` 标志：

| 标志           | 描述                     | POSIX.1 | FreeBSD8.0 | Linux3.2.0 | Mac OSX 10.6.8 |
| -------------- | ------------------------ | ------- | ---------- | ---------- | -------------- |
| `MSG_CTRUNC`   | 控制数据被截断           | •       | •          | •          | •              |
| `MSG_EOR`      | 接收记录结束符           | •       | •          | •          | •              |
| `MSG_ERROUEUE` | 接收错误信息作为辅助数据 |         |            | •          |                |
| `MSG_OOB`      | 接收带外数据             | •       | •          | •          | •              |
| `MSG_TRUNC`    | 一般数据被截断           | •       | •          | •          | •              |

例子，用于从服务器获取正常运行时间的客户端命令。

```c
#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>

#define BUFLEN		128

extern int connect_retry(int, int, int, const struct sockaddr *,
	socklen_t);

void
print_uptime(int sockfd)
{
	int		n;
	char	buf[BUFLEN];

	while ((n = recv(sockfd, buf, BUFLEN, 0)) > 0)
		write(STDOUT_FILENO, buf, n);
	if (n < 0)
		err_sys("recv error");
}

int
main(int argc, char *argv[])
{
	struct addrinfo	*ailist, *aip;
	struct addrinfo	hint;
	int				sockfd, err;

	if (argc != 2)
		err_quit("usage: ruptime hostname");
	memset(&hint, 0, sizeof(hint));
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	if ((err = getaddrinfo(argv[1], "ruptime", &hint, &ailist)) != 0)
		err_quit("getaddrinfo error: %s", gai_strerror(err));
	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		if ((sockfd = connect_retry(aip->ai_family, SOCK_STREAM, 0,
		  aip->ai_addr, aip->ai_addrlen)) < 0) {
			err = errno;
		} else {
			print_uptime(sockfd);
			exit(0);
		}
	}
	err_exit(err, "can't connect to %s", argv[1]);
}

```

这个程序连接服务器，读取服务器发送过来的字符串并将其打印到标准输出。因为使用的是 `SOCK_STREAM` 套接字，所以不能保证调用一次 `recv` 就会读取整个字符串，因此需要重复调用直到它返回 0。

如果服务器支持多重网络接口或多重网络协议，函数 `getaddrinfo` 可能会返回多个候选地址供使用。轮流尝试每个地址，当找到一个允许连接到服务的地址时便可停止。使用图 16-11 中的 connect_retry 函数来与服务器建立一个连接。

例子， 提供系统正常运行时间的服务器程序。

```c
#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <sys/socket.h>

#define BUFLEN	128
#define QLEN 10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

extern int initserver(int, const struct sockaddr *, socklen_t, int);

void
serve(int sockfd)
{
	int		clfd;
	FILE	*fp;
	char	buf[BUFLEN];

	set_cloexec(sockfd);
	for (;;) {
		if ((clfd = accept(sockfd, NULL, NULL)) < 0) {
			syslog(LOG_ERR, "ruptimed: accept error: %s",
			  strerror(errno));
			exit(1);
		}
		set_cloexec(clfd);
		if ((fp = popen("/usr/bin/uptime", "r")) == NULL) {
			sprintf(buf, "error: %s\n", strerror(errno));
			send(clfd, buf, strlen(buf), 0);
		} else {
			while (fgets(buf, BUFLEN, fp) != NULL)
				send(clfd, buf, strlen(buf), 0);
			pclose(fp);
		}
		close(clfd);
	}
}

int
main(int argc, char *argv[])
{
	struct addrinfo	*ailist, *aip;
	struct addrinfo	hint;
	int				sockfd, err, n;
	char			*host;

	if (argc != 1)
		err_quit("usage: ruptimed");
	if ((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
		n = HOST_NAME_MAX;	/* best guess */
	if ((host = malloc(n)) == NULL)
		err_sys("malloc error");
	if (gethostname(host, n) < 0)
		err_sys("gethostname error");
	daemonize("ruptimed");
	memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_CANONNAME;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	if ((err = getaddrinfo(host, "ruptime", &hint, &ailist)) != 0) {
		syslog(LOG_ERR, "ruptimed: getaddrinfo error: %s",
		  gai_strerror(err));
		exit(1);
	}
	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		if ((sockfd = initserver(SOCK_STREAM, aip->ai_addr,
		  aip->ai_addrlen, QLEN)) >= 0) {
			serve(sockfd);
			exit(0);
		}
	}
	exit(1);
}

```

为了找到它的地址，服务器需要获得其运行时的主机名。如果主机名的最大长度不确定，可以使用 HOST_NAME_MAX 代替。如果系统没定义 HOST_NAME_MAX，可以自己定义。POSIX.1 要求主机名的最大长度至少为 255 字节，不包括终止 null 字符，因此定义 HOST_NAME_MAX 为 256 来包括终止 null 字符。

服务器调用 gethostname 获得主机名，查看远程正常运行时间服务的地址。可能会有多个地址返回，但我们简单地选择第一个来建立被动套接字端点（即一个只用于监听连接请求的地址）。处理多个地址作为习题留给读者。

例子，用于说明命令直接写到套接字的服务器程序。

```c
#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define QLEN 10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

extern int initserver(int, const struct sockaddr *, socklen_t, int);

void
serve(int sockfd)
{
	int		clfd, status;
	pid_t	pid;

	set_cloexec(sockfd);
	for (;;) {
		if ((clfd = accept(sockfd, NULL, NULL)) < 0) {
			syslog(LOG_ERR, "ruptimed: accept error: %s",
			  strerror(errno));
			exit(1);
		}
		if ((pid = fork()) < 0) {
			syslog(LOG_ERR, "ruptimed: fork error: %s",
			  strerror(errno));
			exit(1);
		} else if (pid == 0) {	/* child */
			/*
			 * The parent called daemonize ({Prog daemoninit}), so
			 * STDIN_FILENO, STDOUT_FILENO, and STDERR_FILENO
			 * are already open to /dev/null.  Thus, the call to
			 * close doesn't need to be protected by checks that
			 * clfd isn't already equal to one of these values.
			 */
			if (dup2(clfd, STDOUT_FILENO) != STDOUT_FILENO ||
			  dup2(clfd, STDERR_FILENO) != STDERR_FILENO) {
				syslog(LOG_ERR, "ruptimed: unexpected error");
				exit(1);
			}
			close(clfd);
			execl("/usr/bin/uptime", "uptime", (char *)0);
			syslog(LOG_ERR, "ruptimed: unexpected return from exec: %s",
			  strerror(errno));
		} else {		/* parent */
			close(clfd);
			waitpid(pid, &status, 0);
		}
	}
}

int
main(int argc, char *argv[])
{
	struct addrinfo	*ailist, *aip;
	struct addrinfo	hint;
	int				sockfd, err, n;
	char			*host;

	if (argc != 1)
		err_quit("usage: ruptimed");
	if ((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
		n = HOST_NAME_MAX;	/* best guess */
	if ((host = malloc(n)) == NULL)
		err_sys("malloc error");
	if (gethostname(host, n) < 0)
		err_sys("gethostname error");
	daemonize("ruptimed");
	memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_CANONNAME;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	if ((err = getaddrinfo(host, "ruptime", &hint, &ailist)) != 0) {
		syslog(LOG_ERR, "ruptimed: getaddrinfo error: %s",
		  gai_strerror(err));
		exit(1);
	}
	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		if ((sockfd = initserver(SOCK_STREAM, aip->ai_addr,
		  aip->ai_addrlen, QLEN)) >= 0) {
			serve(sockfd);
			exit(0);
		}
	}
	exit(1);
}

```

我们没有采用 popen 来运行 uptime 命令，并从连接到命令标准输出的管道读取输出，而是采用 fork 创建了一个子进程，然后使用 dup2 使 STDIN_FILENO 的子进程副本对/dev/null 开放，使 STDOUT_FILENO 和 STDERR_FILENO 的子进程副本对套接字端点开放。当执行 uptime 时，命令将结果写到它的标准输出，该标准输出是连接到套接字的，所以数据被送到 ruptime 客户端命令。

父进程可以安全地关闭连接到客户端的文件描述符，因为子进程仍旧让它打开着。父进程会等待子进程处理完毕再继续，所以子进程不会变成僵死进程。由于运行 uptime 命令不会花费太长的时间，所以父进程在接受下一个连接请求之前，可以等待子进程退出。然而，如果子进程运行的时间比较长的话，这种策略就未必适合了。

前面的实例采用的都是面向连接的套接字。但如何选择合适的套接字类型呢？何时采用面向连接的套接字，何时采用无连接的套接字呢？答案取决于我们要做的工作量和能够容忍的出错程度。

对于无连接的套接字，数据包到达时可能已经没有次序，因此如果不能将所有的数据放在一个数据包里，则在应用程序中就必须关心数据包的次序。数据包的最大尺寸是通信协议的特征。另外，对于无连接的套接字，数据包可能会丢失。如果应用程序不能容忍这种丢失，必须使用面向连接的套接字。

容忍数据包丢失意味着两种选择。一种选择是，如果想和对等方可靠通信，就必须对数据包编号，并且在发现数据包丢失时，请求对等应用程序重传，还必须标识重复数据包并丢弃它们，因为数据包可能会延迟或疑似丢失，可能请求重传之后，它们又出现了。

另一种选择是，通过让用户再次尝试那个命令来处理错误。对于简单的应用程序，这可能就足够了，但对于复杂的应用程序，这种选择通常不可行。因此，一般在这种情况下使用面向连接的套接字比较好。面向连接的套接字的缺陷在于需要更多的时间和工作来建立一个连接，并且每个连接都需要消耗较多的操作系统资源。

例子， 采用数据报套接字接口的 uptime 客户端命令版本。

```c
#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>

#define BUFLEN		128
#define TIMEOUT		20

void
sigalrm(int signo)
{
}

void
print_uptime(int sockfd, struct addrinfo *aip)
{
	int		n;
	char	buf[BUFLEN];

	buf[0] = 0;
	if (sendto(sockfd, buf, 1, 0, aip->ai_addr, aip->ai_addrlen) < 0)
		err_sys("sendto error");
	alarm(TIMEOUT);
	if ((n = recvfrom(sockfd, buf, BUFLEN, 0, NULL, NULL)) < 0) {
		if (errno != EINTR)
			alarm(0);
		err_sys("recv error");
	}
	alarm(0);
	write(STDOUT_FILENO, buf, n);
}

int
main(int argc, char *argv[])
{
	struct addrinfo		*ailist, *aip;
	struct addrinfo		hint;
	int					sockfd, err;
	struct sigaction	sa;

	if (argc != 2)
		err_quit("usage: ruptime hostname");
	sa.sa_handler = sigalrm;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGALRM, &sa, NULL) < 0)
		err_sys("sigaction error");
	memset(&hint, 0, sizeof(hint));
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	if ((err = getaddrinfo(argv[1], "ruptime", &hint, &ailist)) != 0)
		err_quit("getaddrinfo error: %s", gai_strerror(err));

	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		if ((sockfd = socket(aip->ai_family, SOCK_DGRAM, 0)) < 0) {
			err = errno;
		} else {
			print_uptime(sockfd, aip);
			exit(0);
		}
	}

	fprintf(stderr, "can't contact %s: %s\n", argv[1], strerror(err));
	exit(1);
}

```

除了增加安装一个 SIGALRM 的信号处理程序以外，基于数据报的客户端中的 main 函数和面向连接的客户端中的类似。使用 alarm 函数来避免调用 recvfrom 时的无限期阻塞。

对于面向连接的协议，需要在交换数据之前连接到服务器。对于服务器来说，到来的连接请求已经足够判断出所需提供给客户端的服务。但是对于基于数据报的协议，需要有一种方法通知服务器来执行服务。本例中，只是简单地向服务器发送了 1 字节的数据。服务器将接收它，从数据包中得到地址，并使用这个地址来传送它的响应。如果服务器提供多个服务，可以使用这个请求数据来表示需要的服务，但由于服务器只做一件事情，1 字节数据的内容是无关紧要的。如果服务器不在运行状态，客户端调用 recvfrom 便会无限期阻塞。对于这个面向连接的实例，如果服务器不运行，connect 调用会失败。为了避免无限期阻塞，可以在调用 recvfrom 之前设置警告时钟。

例子，基于数据报提供系统正常运行时间的服务器。

```c
#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <sys/socket.h>

#define BUFLEN		128
#define MAXADDRLEN	256

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

extern int initserver(int, const struct sockaddr *, socklen_t, int);

void
serve(int sockfd)
{
	int				n;
	socklen_t		alen;
	FILE			*fp;
	char			buf[BUFLEN];
	char			abuf[MAXADDRLEN];
	struct sockaddr	*addr = (struct sockaddr *)abuf;

	set_cloexec(sockfd);
	for (;;) {
		alen = MAXADDRLEN;
		if ((n = recvfrom(sockfd, buf, BUFLEN, 0, addr, &alen)) < 0) {
			syslog(LOG_ERR, "ruptimed: recvfrom error: %s",
			  strerror(errno));
			exit(1);
		}
		if ((fp = popen("/usr/bin/uptime", "r")) == NULL) {
			sprintf(buf, "error: %s\n", strerror(errno));
			sendto(sockfd, buf, strlen(buf), 0, addr, alen);
		} else {
			if (fgets(buf, BUFLEN, fp) != NULL)
				sendto(sockfd, buf, strlen(buf), 0, addr, alen);
			pclose(fp);
		}
	}
}

int
main(int argc, char *argv[])
{
	struct addrinfo	*ailist, *aip;
	struct addrinfo	hint;
	int				sockfd, err, n;
	char			*host;

	if (argc != 1)
		err_quit("usage: ruptimed");
	if ((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
		n = HOST_NAME_MAX;	/* best guess */
	if ((host = malloc(n)) == NULL)
		err_sys("malloc error");
	if (gethostname(host, n) < 0)
		err_sys("gethostname error");
	daemonize("ruptimed");
	memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_CANONNAME;
	hint.ai_socktype = SOCK_DGRAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	if ((err = getaddrinfo(host, "ruptime", &hint, &ailist)) != 0) {
		syslog(LOG_ERR, "ruptimed: getaddrinfo error: %s",
		  gai_strerror(err));
		exit(1);
	}
	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		if ((sockfd = initserver(SOCK_DGRAM, aip->ai_addr,
		  aip->ai_addrlen, 0)) >= 0) {
			serve(sockfd);
			exit(0);
		}
	}
	exit(1);
}
```

服务器在 recvfrom 阻塞等待服务请求。当一个请求到达时，保存请求者地址并使用 popen 来运行 uptime 命令。使用 sendto 函数将输出发送到客户端，将目标地址设置成刚才的请求者地址。

## 套接字选项

套接字机制提供了两个套接字选项接口来控制套接字行为。一个接口用来设置选项，另一个接口可以查询选项的状态。可以获取或设置以下 3 种选项。

1. 通用选项，工作在所有套接字类型上。
2. 在套接字层次管理的选项，但是依赖于下层协议的支持。
3. 特定于某协议的选项，每个协议独有的。

Single UNIX Specification 定义了套接字层的选项（上述选项中的前两个选项类型）。可以使用 setsockopt 函数来设置套接字选项。

```c
#include <sys/socket.h>
int setsockopt(int sockfd, int level, int option, const void
*val, socklen_t len);
```

返回值：

- 若成功，返回 0；
- 若出错，返回 −1。

参数 level 标识了选项应用的协议。如果选项是通用的套接字层次选项，则 level 设置成 SOL_SOCKET。否则，level 设置成控制这个选项的协议编号。对于 TCP 选项，level 是 IPPROTO_TCP，对于 IP，level 是 IPPROTO_IP。

套接字选项

| 选项            | 参数 val 的类型 | 描述                                            |
| --------------- | --------------- | ----------------------------------------------- |
| `SO_ACCEPTCONN` | int             | 返回信息指示该套接字是否能被监听(仅 getsockopt) |
| `SO_BROADCAST`  | int             | 如果 `*val` 非 0，广播数据报                    |
| `SO_DEBUG`      | int             | 如果 `*val` 非 0，启用网络驱动调试功能          |
| `SO_DONTROUTE`  | int             | 如果 `*val` 非 0，绕过通常路由                  |
| `SO_ERROR`      | int             | 返回挂起的套接字错误并清除(仅 getsockopt)       |
| `SO_KEEPALIVE`  | int             | 如果 `*val` 非 0，启用周期性 ecp-alive 报文     |
| `SO_LINGER`     | struct linger   | 当还有未发报文而套接字已关闭时，延迟时间        |
| `SO_OOBINLINE`  | int             | 如果 `*val` 非 0，将带外数据放在普通数据中      |
| `SO_RCVBUF`     | int             | 接收缓冲区的字节长度                            |
| `SO_RCVLOWAT`   | int             | 接收调用中返回的最小数据字节数                  |
| `SO_RCVTIMEO`   | struct timeval  | 套接字接收调用的超时值                          |
| `SO_REUSEADDR`  | int             | 如果 `*val` 非 0，重用 bind 中的地址            |
| `SO_SNDBUF`     | int             | 发送缓冲区的字节长度                            |
| `SO_SNDLOWAT`   | int             | 发送调用中传送的最小数据字节数                  |
| `SO_SNDTIMEO`   | struct timeval  | 套接字发送调用的超时值                          |
| `SO_TYPE`       | int             | 标识套接字类型(仅 getsockopt)                   |

参数 val 根据选项的不同指向一个数据结构或者一个整数。一些选项是 on/off 开关。如果整数非 0，则启用选项。如果整数为 0，则禁止选项。参数 len 指定了 val 指向的对象的大小。

可以使用 getsockopt 函数来查看选项的当前值。

```c
#include <sys/socket.h>
int getsockopt(int sockfd, int level, int option, void
*restrict val,
socklen_t *restrict lenp);
```

返回值：

- 若成功，返回 0；
- 若出错，返回 −1。

参数 lenp 是一个指向整数的指针。在调用 getsockopt 之前，设置该整数为复制选项缓冲区的长度。如果选项的实际长度大于此值，则选项会被截断。如果实际长度正好小于此值，那么返回时将此值更新为实际长度。

例子，采用地址复用初始化套接字端点供服务器使用。

```c
#include "apue.h"
#include <errno.h>
#include <sys/socket.h>

int
initserver(int type, const struct sockaddr *addr, socklen_t alen,
  int qlen)
{
	int fd, err;
	int reuse = 1;

	if ((fd = socket(addr->sa_family, type, 0)) < 0)
		return(-1);
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse,
	  sizeof(int)) < 0)
		goto errout;
	if (bind(fd, addr, alen) < 0)
		goto errout;
	if (type == SOCK_STREAM || type == SOCK_SEQPACKET)
		if (listen(fd, qlen) < 0)
			goto errout;
	return(fd);

errout:
	err = errno;
	close(fd);
	errno = err;
	return(-1);
}
```

为了启用 SO_REUSEADDR 选项，设置了一个非 0 值的整数，并把这个整数地址作为 val 参数传递给了 setsockopt。将 len 参数设置成了一个整数大小来表明 val 所指的对象的大小。

## 带外数据

带外数据（out-of-band data）是一些通信协议所支持的可选功能，与普通数据相比，它允许更高优先级的数据传输。带外数据先行传输，即使传输队列已经有数据。TCP 支持带外数据，但是 UDP 不支持。套接字接口对带外数据的支持很大程度上受 TCP 带外数据具体实现的影响。TCP 将带外数据称为紧急数据（urgent data）。TCP 仅支持一个字节的紧急数据，但是允许紧急数据在普通数据传递机制数据流之外传输。为了产生紧急数据，可以在 3 个 send 函数中的任何一个里指定 MSG_OOB 标志。

如果带 MSG_OOB 标志发送的字节数超过一个时，最后一个字节将被视为紧急数据字节。

如果通过套接字安排了信号的产生，那么紧急数据被接收时，会发送 SIGURG 信号。在 3.14 节和 14.5.2 节中可以看到，在 fcntl 中使用 F_SETOWN 命令来设置一个套接字的所有权。如果 fcntl 中的第三个参数为正值，那么它指定的就是进程 ID。如果为非-1 的负值，那么它代表的就是进程组 ID。

因此，可以通过调用以下函数安排进程接收套接字的信号：

`fcntl(sockfd, F_SETOWN, pid);`

F_GETOWN 命令可以用来获得当前套接字所有权。对于 F_SETOWN 命令，负值代表进程组 ID，正值代表进程 ID。因此，调用

`owner = fcntl(sockfd, F_GETOWN, 0);`

将返回 owner，如果 owner 为正值，则等于配置为接收套接字信号的进程的 ID。如果 owner 为负值，其绝对值为接收套接字信号的进程组的 ID。

TCP 支持紧急标记（urgent mark）的概念，即在普通数据流中紧急数据所在的位置。如果采用套接字选项 SO_OOBINLINE，那么可以在普通数据中接收紧急数据。为帮助判断是否已经到达紧急标记，可以使用函数 sockatmark。

```c
#include <sys/socket.h>
int sockatmark(int sockfd);
```

返回值：

- 若在标记处，返回 1；
- 若没在标记处，返回 0；
- 若出错，返回 −1。

当下一个要读取的字节在紧急标志处时，sockatmark 返回 1。

当带外数据出现在套接字读取队列时，select 函数（见 14.4.1 节）会返回一个文件描述符并且有一个待处理的异常条件。可以在普通数据流上接收紧急数据，也可以在其中一个 recv 函数中采用 MSG_OOB 标志在其他队列数据之前接收紧急数据。TCP 队列仅用一个字节的紧急数据。如果在接收当前的紧急数据字节之前又有新的紧急数据到来，那么已有的字节会被丢弃。

## 非阻塞和异步 I/O

通常，recv 函数没有数据可用时会阻塞等待。同样地，当套接字输出队列没有足够空间来发送消息时，send 函数会阻塞。在套接字非阻塞模式下，行为会改变。在这种情况下，这些函数不会阻塞而是会失败，将 errno 设置为 EWOULDBLOCK 或者 EAGAIN。当这种情况发生时，可以使用 poll 或 select 来判断能否接收或者传输数据。Single UNIX Specification 包含通用异步 I/O 机制（见 14.5 节）的支持。套接字机制有其自己的处理异步 I/O 的方式，但是这在 SingleUNIXSpecification 中没有标准化。一些文献把经典的基于套接字的异步 I/O 机制称为“基于信号的 I/O”，区别于 Single UNIX Specification 中的通用异步 I/O 机制。

在基于套接字的异步 I/O 中，当从套接字中读取数据时，或者当套接字写队列中空间变得可用时，可以安排要发送的信号 SIGIO。启用异步 I/O 是一个两步骤的过程。

1. 建立套接字所有权，这样信号可以被传递到合适的进程。
2. 通知套接字当 I/O 操作不会阻塞时发信号。

可以使用 3 种方式来完成第一个步骤。

1. 在 fcntl 中使用 F_SETOWN 命令。
2. 在 ioctl 中使用 FIOSETOWN 命令。
3. 在 ioctl 中使用 SIOCSPGRP 命令。

要完成第二个步骤，有两个选择。

1. 在 fcntl 中使用 F_SETFL 命令并且启用文件标志 O_ASYNC。
2. 在 ioctl 中使用 FIOASYNC 命令。

虽然有多种选项，但它们没有得到普遍支持。图 16-23 总结了本文讨论的平台支持这些选项的情况。

套接字异步 I/O 管理命令

| 机制                                 | POSIX.1 | FreeBSD8.0 | Linux3.2.0 | Mac OSX 10.6.8 |
| ------------------------------------ | ------- | ---------- | ---------- | -------------- |
| `fcntl(fd, F_SETOWN,pid)`            | •       | •          | •          | •              |
| `ioctl(fd, FIOSETOWN,pid)`           |         | •          | •          | •              |
| `ioctl(fd, SIOCSPGRP,pid)`           |         | •          | •          | •              |
| `fcntl(fd, F_SETFL, flags\|O_ASYNC)` |         | •          | •          | •              |
| `ioctl(fd, FIOASYNC, &n);`           |         | •          | •          | •              |
