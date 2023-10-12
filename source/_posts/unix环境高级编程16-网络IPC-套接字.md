---
title: unix环境高级编程16-网络IPC-套接字
date: 2023-04-10 17:23:26
tags: linux 编程
category: unix环境高级编程
---

## 套接字描述符
套接字是通信端点的抽象。正如使用文件描述符访问文件，应用程序用套接字描述符访问套接字。套接字描述符在UNIX系统中被当作是一种文件描述符。事实上，许多处理文件描述符的函数（如read和write）可以用于处理套接字描述符。

为创建一个套接字，调用socket函数。
```c
#include <sys/socket.h>
int socket (int domain, int type, int protocol);
```

返回值：
若成功，返回文件（套接字）描述符；
若出错，返回−1

参数domain（域）确定通信的特性，包括地址格式。各个域都有自己表示地址的格式，而表示各个域的常数都以AF_开头，意指地址族（address family）。

| 域        | 描述         |
| --------- | ------------ |
| AF_INET   | IPv4因特网城 |
| AF_INET6  | IPv6因特网域 |
| AF UNIX   | UNIX域       |
| AF_UPSPEC | 未指定       |

大多数系统还定义了AF_LOCAL域，这是AF_UNIX的别名。AF_UNSPEC 域可以代表“任何”域。

参数type确定套接字的类型，进一步确定通信特征。下表总结了由POSIX.1定义的套接字类型，但在实现中可以自由增加其他类型的支持。

| 类型           | 描述                                           |
| -------------- | ---------------------------------------------- |
| SOCK_DGRAM     | 固定长度的、无连接的、不可靠的报文传递         |
| SOCK_RAW       | IP协议的数据报接口(在POSIX1中为可选)           |
| SOCK_SEOPACKET | 固定长度的、有序的、可靠的、面向连接的报文传递 |
| SOCK_STREAM    | 有序的、可靠的、双向的、面向连接的字节流       |

参数protocol通常是 0，表示为给定的域和套接字类型选择默认协议。当对同一域和套接字类型支持多个协议时，可以使用protocol 选择一个特定协议。在 AF_INET 通信域中，套接字类型SOCK_STREAM的默认协议是传输控制协议（Transmission Control Protocol，TCP）。在AF_INET通信域中，套接字类型SOCK_DGRAM的默认协议是UDP。图16-3列出了为因特网域套接字定义的协议。

| 协议         | 描述                                                 |
| ------------ | ---------------------------------------------------- |
| IPPROTO_IP   | IPv4网际协议                                         |
| IPPROTO_PV6  | IPv6网际协议(在POSX1中为可选)                        |
| IPPROTO_ICMP | 因特网控制报文协议(Internet ControlMessage Protocol) |
| IPPROTO_RAW  | 原始IP 数据包协议(在POSX1中为可选)                   |
| IPPROTO_TCP  | 传输控制协议                                         |
| IPPROTO_UDP  | 用户数据报协议(User Datagram Protocol)               |

SOCK_STREAM套接字提供字节流服务，所以应用程序分辨不出报文的界限。这意味着从SOCK_STREAM
套接字读数据时，它也许不会返回所有由发送进程所写的字节数。最终可以获得发送过来的所有数据，但也许要通过若干次函数调用才能得到。

SOCK_SEQPACKET 套接字和 SOCK_STREAM 套接字很类似，只是从该套接字得到的是基于报文的服务而不是字节流服务。这意味着从SOCK_SEQPACKET套接字接收的数据量与对方所发送的一致。流控制传输协议（Stream Control Transmission Protocol，SCTP）提供了因特网域上的顺序数据包服务。

SOCK_RAW 套接字提供一个数据报接口，用于直接访问下面的网络层（即因特网域中的 IP层）。使用这个接口时，应用程序负责构造自己的协议头部，这是因为传输协议（如TCP和UDP）被绕过了。当创建一个原始套接字时，需要有超级用户特权，这样可以防止恶意应用程序绕过内建安全机制来创建报文。

虽然套接字描述符本质上是一个文件描述符，但不是所有参数为文件描述符的函数都可以接受套接字描述符。

文件描述符函数使用套接字时的行为

| 函数              | 使用套接字时的行为                                                                                 |
| ----------------- | -------------------------------------------------------------------------------------------------- |
| close             | 释放套接字                                                                                         |
| dup和dup2         | 和一般文件描述符一样复制                                                                           |
| fchdir           | 失败，并且将errno 设置为 ENOTDIR                                                                   |
| fchomod           | 未指定                                                                                             |
| fchown            | 由实现定义                                                                                         |
| fcntl             | 支持一些命令，包括F_DUPFD、FDUPFD_CLOEXEC、F_GETED、F_GETFL、F_GETOWN、F_SETFD、F_SETFL 和F_SETOWN |
| fdatasync和 fsync | 由实现定义                                                                                         |
| fstat            | 支持一些 stat 结构成员，但如何支持由实现定义                                                       |
| ftruncate        | 未指定                                                                                             |
| ioctl             | 支持部分命令，依赖于底层设备驱动                                                                   |
| lseek             | 由实现定义(通常失败时会将 errno设为ESPIPE)                                                         |
| mmap              | 未指定                                                                                             |
| poll              | 正常工作                                                                                           |
| pread和pwrite     | 失败时会将 errno 设为 ESPIPE                                                                       |
| read              | 与没有任何标志位的 recv等价                                                                        |
| select            | 正常工作                                                                                           |
| write             | 与没有任何标志位的 send 等价                                                                       |

套接字通信是双向的。可以采用shutdown函数来禁止一个套接字的I/O。
```c
#include <sys/socket.h>
int shutdown (int sockfd, int how);
```

返回值：
- 若成功，返回0；
- 若出错，返回−1

如果how是SHUT_RD（关闭读端），那么无法从套接字读取数据。如果how是SHUT_WR（关闭写端），那么无法使用套接字发送数据。如果how是SHUT_RDWR，则既无法读取数据，又无法发送数据。

shutdown 允许使一个套接字处于不活动状态，和引用它的文件描述符数目无关。其次，有时可以很方便地关闭套接字双向传输中的一个方向。

## 寻址

进程标识由两部分组成。一部分是计算机的网络地址，它可以帮助标识网络上我们想与之通信的计算机；另一部分是该计算机上用端口号表示的服务，它可以帮助标识特定的进程。

### 字节序
与同一台计算机上的进程进行通信时，一般不用考虑字节序。网络协议指定了字节序，因此异构计算机系统能够交换协议信息而不会被字节序所混淆。TCP/IP协议栈使用大端字节序。应用程序交换格式化数据时，字节序问题就会出现。对于TCP/IP，地址用网络字节序来表示，所以应用程序有时需要在处理器的字节序与网络字节序之间转换它们。

对于TCP/IP应用程序，有4个用来在处理器字节序和网络字节序之间实施转换的函数。
```c
#include <arpa/inet.h>
uint32_t htonl(uint32_t hostint32);
uint16_t htons(uint16_t hostint16);
uint32_t ntohl(uint32_t netint32);
uint16_t ntohs(uint16_t netint16);
```

4个函数返回值：
- 以网络字节序表示的32位整数
- 以网络字节序表示的16位整数
- 以主机字节序表示的32位整数
- 以主机字节序表示的16位整数

h表示“主机”字节序，n表示“网络”字节序。l表示“长”（即4字节）整数，s表示“短”（即4字节）整数。对于系统来说，把这些函数实现为宏也是很常见的。

### 地址格式
一个地址标识一个特定通信域的套接字端点，地址格式与这个特定的通信域相关。为使不同格式地址能够传入到套接字函数，地址会被强制转换成一个通用的地址结构sockaddr：
```c
struct sockaddr {
    sa_family_t sa_family; /* address family */
    char sa_data[];        /* variable-length address */
    ...
};
```
套接字实现可以自由地添加额外的成员并且定义sa_data成员的大小。在 Linux 中，该结构定义如下：
```c
struct sockaddr {
    sa_family_t sa_family;   /* address family */
    char sa_data[14];        /* variable-length address */
};
```

因特网地址定义在 `<netinet/in.h>` 头文件中。在IPv4因特网域（AF_INET）中，套接字地址用结构sockaddr_in表示：
```c
struct in_addr {
    in_addr_t s_addr; /* IPv4 address */
};
struct sockaddr_in {
    sa_family_t sin_family; /* address family */
    in_port_t sin_port; /* port number */
    struct in_addr sin_addr; /* IPv4 address */
};
```
数据类型in_port_t定义成uint16_t。数据类型in_addr_t定义成uint32_t。这些整数类型在 `<stdint.h>` 中定义并指定了相应的位数。

与AF_INET域相比较，IPv6因特网域（AF_INET6）套接字地址用结构sockaddr_in6表示：
```c
struct_in6_addr {
    uint8_t s6_addr[16]; /* IPv6 address */
};
struct sockaddr_in6 {
    sa_family_t sin6_family; /* address family */
    in_port_t sin6_port; /* port number */
    uint32_t sin6_flowinfo; /* traffic class and flow info */
    struct in6_addr sin6_addr; /* IPv6 address*/
    uint32_t sin6_scope_id; /* set of interfaces for scope */
};
```

这些都是Single UNIX Specification要求的定义。每个实现可以自由添加更多的字段。在Linux中，sockaddr_in定义如下：
```c
struct sockaddr_in {
    sa_family_t sin_family; /* address family */
    in_port_t sin_port; /* port number */
    struct in6_addr sin6_addr; /* IPv4 address */
    unsigned char sin_zero[8]; /* filler */
};
```
尽管 sockaddr_in 与 sockaddr_in6 结构相差比较大，但它们均被强制转换成sockaddr结构输入到套接字例程中。

有时，需要打印出能被人理解而不是计算机所理解的地址格式。有两个新函数inet_ntop和inet_pton具有相似的功能，而且同时支持 IPv4地址和IPv6地址。
```c
#include <arpa/inet.h>
const char *inet_ntop(int domain, const void *restrict addr, char *restrict str, socklen_t size);
```
返回值：
- 若成功，返回地址字符串指针；
- 若出错，返回NULL

```c
int inet_pton(int domain, const char * restrict str, void *restrict addr);
```
返回值：
- 若成功，返回1；若格式无效，返回0；
- 若出错，返回−1

函数 inet_ntop 将网络字节序的二进制地址转换成文本字符串格式。inet_pton 将文本字符串格式转换成网络字节序的二进制地址。参数domain仅支持两个值：AF_INET和AF_INET6。

对于 inet_ntop，参数size指定了保存文本字符串的缓冲区（str）的大小。两个常数用于简化工作：INET_ADDRSTRLEN 定义了足够大的空间来存放一个表示 IPv4 地址的文本字符串；INET6_ADDRSTRLEN 定义了足够大的空间来存放一个表示 IPv6 地址的文本字符串。对于inet_pton，如果 domain是AF_INET，则缓冲区addr需要足够大的空间来存放一个32位地址，如果domain是AF_INET6，则需要足够大的空间来存放一个128位地址。

### 地址查询
理想情况下，应用程序不需要了解一个套接字地址的内部结构。如果一个程序简单地传递一个类似于sockaddr结构的套接字地址，并且不依赖于任何协议相关的特性，那么可以与提供相同类型服务的许多不同协议协作。

通过调用gethostent，可以找到给定计算机系统的主机信息。
```c
#include <netdb.h>
struct hostent *gethostent(void);
```

返回值：
- 若成功，返回指针；
- 若出错，返回NULL
```c
void sethostent(int stayopen);
void endhostent(void);
```

如果主机数据库文件没有打开，gethostent会打开它。函数gethostent返回文件中的下一个条目。函数sethostent会打开文件，如果文件已经被打开，那么将其回绕。当stayopen参数设置成非0值时，调用gethostent之后，文件将依然是打开的。函数endhostent可以关闭文件。

当gethostent返回时，会得到一个指向hostent结构的指针，该结构可能包含一个静态的数据缓冲区，每次调用gethostent，缓冲区都会被覆盖。hostent结构至少包含以下成员：
```c
struct hostent{
    char *h_name; /* name of host */
    char **h_aliases; /* pointer to alternate host name array */
    int h_addrtype; /* address type */
    int h_length; /* length in bytes of address */
    char **h_addr_list; /* pointer to array of network addresses */
    ...
};
```
返回的地址采用网络字节序。另外两个函数gethostbyname和gethostbyaddr，原来包含在hostent函数中，现在则被认为是过时的。能够采用一套相似的接口来获得网络名字和网络编号。
```c
#include <netdb.h>
struct netent *getnetbyaddr (uint32_t net, int type);
struct netent *getnetbyname(const char *name);
struct netent *getnetent(void);
```
3个函数的返回值：
- 若成功，返回指针；
- 若出错，返回NULL

```c
void setnetent(int stayopen);
void endnetent(void);
```
netent结构至少包含以下字段：
```c
struct netent {
    char *n_name; /* network name */
    char **n_aliases; /* alternate network name array pointer */
    int n_addrtype; /* address type */
    uint32_t n_net; /* network number */
    ...
};
```

网络编号按照网络字节序返回。地址类型是地址族常量之一（如AF_INET）。

我们可以用以下函数在协议名字和协议编号之间进行映射。
```c
#include <netdb.h>
struct protoent *getprotobyname(const char *name);
struct protoent *getprotobynumber(int proto);
struct protoent *getprotoent(void);
```

3个函数的返回值：
- 若成功，返回指针；
- 若出错，返回NULL

```c
void setprotoent(int stayopen);
void endprotoent(void);
```

POSIX.1定义的protoent结构至少包含以下成员：
```c
struct protoent {
    char *p_name; /* protocol name */
    char **p_ aliases; /* pointer to altername protocol name array */
    int p_proto; /* protocol number */
    ...
};
```

服务是由地址的端口号部分表示的。每个服务由一个唯一的众所周知的端口号来支持。可以使用函数getservbyname将一个服务名映射到一个端口号，使用函数getservbyport将一个端口号映射到一个服务名，使用函数getservent顺序扫描服务数据库。
```c
#include <netdb.h>
struct servent *getservbyname(const char *name, const char
*proto);
struct servent *getserbyport(int port, const char *proto);
struct servent *getservent(void);

```
3个函数的返回值：
若成功，返回指针，
若出错，返回NULL

```c
void setservent(int stayopen);
void endservent(void);
```

servent结构至少包含以下成员：
```c
struct servent{
char *s_name; /* service name */
char **s_aliases; /* pointer to alternate service name array */
int s_port; /* port number */
char *s_proto; /* name of protocol */
...
};
```

POSIX.1定义了若干新的函数，允许一个应用程序将一个主机名和一个服务名映射到一个地址，或者反之。这些函数代替了较老的函数gethostbyname和gethostbyaddr。

getaddrinfo函数允许将一个主机名和一个服务名映射到一个地址。
```c
#include <sys/socket.h>
#include <netdb.h>
int getaddrinfo(const char *restrict host, const char *restrict service,
                const struct addrinfo *restrict hint, struct addrinfo **restrict res);
```

返回值：
- 若成功，返回0；
- 若出错，返回非0错误码

```c
void freeaddrinfo(struct addrinfo *ai);
```

需要提供主机名、服务名，或者两者都提供。如果仅仅提供一个名字，另外一个必须是一个空指针。主机名可以是一个节点名或点分格式的主机地址。

getaddrinfo函数返回一个链表结构addrinfo。可以用freeaddrinfo 来释放一个或多个这种结构，这取决于用ai_next字段链接起来的结构有多少。

addrinfo结构的定义至少包含以下成员：
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
可以提供一个可选的hint来选择符合特定条件的地址。hint是一个用于过滤地址的模板，包括ai_family、ai_flags、ai_protocol和ai_socktype字段。剩余的整数字段必须设置为0，指针字段必须为空。

 addrinfo结构的标志

| 标志           | 描述                                                |
| -------------- | --------------------------------------------------- |
| AI_ADDRCONFIG  | 查询配置的地址类型(IPv4 或IPv6)                     |
| AI_ALL         | 查找IPv4和IPv6地址(仅用于AI_V4MAPPED)               |
| AI_CANONNAME   | 需要一个规范的名字(与别名相对)                      |
| AI_NUMERICHOST | 以数字格式指定主机地址，不翻译                      |
| AI_NUMERICSERV | 将服务指定为数字端口号，不翻译                      |
| AI_PASSIVE     | 套接字地址用于监听绑定                              |
| AI_V4MAPPED    | 如没有找到 IPv6 地址，返回映射到 IPv6格式的IP4 地址 |

如果getaddrinfo失败，不能使用perror或strerror来生成错误消息，而是要调用gai_strerror将返回的错误码转换成错误消息。
```c
#include <netdb.h>
const char *gai_strerror(int error);
```

返回值：
指向描述错误的字符串的指针

getnameinfo函数将一个地址转换成一个主机名和一个服务名。
```c
#include <sys/socket.h>
#include <netdb.h>
int getnameinfo(const struct sockaddr *restrict addr, socklen_t alen, char *restrict host, 
                socklen_t hostlen, char *restrict service, socklen_t servlen, int flags);
```

返回值：
- 若成功，返回0；
- 若出错，返回非0值

套接字地址（addr）被翻译成一个主机名和一个服务名。如果host非空，则指向一个长度为hostlen字节的缓冲区用于存放返回的主机名。同样，如果service非空，则指向一个长度为servlen字节的缓冲区用于存放返回的主机名。

flags参数提供了一些控制翻译的方式。
| 标志            | 描述                                       |
| --------------- | ------------------------------------------ |
| NI_DGRAM        | 服务基于数据报而非基于流                   |
| NI_NAMEREQD     | 如果找不到主机名，将其作为一个错误对待     |
| NI_NOFODN       | 对于本地主机，仅返回全限定域名的节点名部分 |
| NI_NUMERICHOST  | 返回主机地址的数字形式，而非主机名         |
| NI_NUMERICSCOPE | 对于IPv6，返回范围I的数字形式，而非名字    |
| NI_NUMERICSERV  | 返回服务地址的数字形式(即端口号)，而非名字 |

例子，打印主机和服务信息。
```c
#include "apue.h"
#if defined(SOLARIS)
#include <netinet/in.h>
#endif
#include <netdb.h>
#include <arpa/inet.h>
#if defined(BSD)
#include <sys/socket.h>
#include <netinet/in.h>
#endif

void
print_family(struct addrinfo *aip)
{
	printf(" family ");
	switch (aip->ai_family) {
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

void
print_type(struct addrinfo *aip)
{
	printf(" type ");
	switch (aip->ai_socktype) {
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

void
print_protocol(struct addrinfo *aip)
{
	printf(" protocol ");
	switch (aip->ai_protocol) {
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
		printf("raw");
		break;
	default:
		printf("unknown (%d)", aip->ai_protocol);
	}
}

void
print_flags(struct addrinfo *aip)
{
	printf("flags");
	if (aip->ai_flags == 0) {
		printf(" 0");
	} else {
		if (aip->ai_flags & AI_PASSIVE)
			printf(" passive");
		if (aip->ai_flags & AI_CANONNAME)
			printf(" canon");
		if (aip->ai_flags & AI_NUMERICHOST)
			printf(" numhost");
		if (aip->ai_flags & AI_NUMERICSERV)
			printf(" numserv");
		if (aip->ai_flags & AI_V4MAPPED)
			printf(" v4mapped");
		if (aip->ai_flags & AI_ALL)
			printf(" all");
	}
}

int
main(int argc, char *argv[])
{
	struct addrinfo		*ailist, *aip;
	struct addrinfo		hint;
	struct sockaddr_in	*sinp;
	const char 			*addr;
	int 				err;
	char 				abuf[INET_ADDRSTRLEN];

	if (argc != 3)
		err_quit("usage: %s nodename service", argv[0]);
	hint.ai_flags = AI_CANONNAME;
	hint.ai_family = 0;
	hint.ai_socktype = 0;
	hint.ai_protocol = 0;
	hint.ai_addrlen = 0;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	if ((err = getaddrinfo(argv[1], argv[2], &hint, &ailist)) != 0)
		err_quit("getaddrinfo error: %s", gai_strerror(err));
	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		print_flags(aip);
		print_family(aip);
		print_type(aip);
		print_protocol(aip);
		printf("\n\thost %s", aip->ai_canonname?aip->ai_canonname:"-");
		if (aip->ai_family == AF_INET) {
			sinp = (struct sockaddr_in *)aip->ai_addr;
			addr = inet_ntop(AF_INET, &sinp->sin_addr, abuf,
			    INET_ADDRSTRLEN);
			printf(" address %s", addr?addr:"unknown");
			printf(" port %d", ntohs(sinp->sin_port));
		}
		printf("\n");
	}
	exit(0);
}
```
这个程序说明了 getaddrinfo 函数的使用方法。如果有多个协议为指定的主机提供给定的服务，程序会打印出多条信息。本实例仅打印了与IPv4一起工作的那些协议（ai_family为AF_INET）的地址信息。如果想将输出限制在AF_INET协议族，可以在提示中设置ai_family字段。

### 将套接字与地址关联

将一个客户端的套接字关联上一个地址没有多少新意，可以让系统选
一个默认的地址。然而，对于服务器，需要给一个接收客户端请求的服务
器套接字关联上一个众所周知的地址。客户端应有一种方法来发现连接服
务器所需要的地址，最简单的方法就是服务器保留一个地址并且注册在/
etc/services或者某个名字服务中。
使用bind函数来关联地址和套接字。、
```c
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr *addr, socklen_t
len);
```

返回值：若成功，返回0；若出错，返回−1

对于使用的地址有以下一些限制。
•在进程正在运行的计算机上，指定的地址必须有效；不能指定一个
其他机器的地址。
•地址必须和创建套接字时的地址族所支持的格式相匹配。
•地址中的端口号必须不小于1 024，除非该进程具有相应的特权（即
超级用户）。
•一般只能将一个套接字端点绑定到一个给定地址上，尽管有些协议
允许多重绑定。

对于因特网域，如果指定IP地址为INADDR_ANY（<netinet/in.h>中
定义的），套接字端点可以被绑定到所有的系统网络接口上。这意味着可
以接收这个系统所安装的任何一个网卡的数据包。在下一节中可以看到，
如果调用 connect 或 listen，但没有将地址绑定到套接字上，系统会选一个
地址绑定到套接字上。

可以调用getsockname函数来发现绑定到套接字上的地址。
```c
#include <sys/socket.h>
int getsockname(int sockfd, struct sockaddr *restrict addr,
socklen_t *restrict alenp);
```

返回值：若成功，返回0；若出错，返回−1

调用 getsockname 之前，将 alenp 设置为一个指向整数的指针，该整数
指定缓冲区sockaddr
的长度。返回时，该整数会被设置成返回地址的大
小。如果地址和提供的缓冲区长度不匹配，地址会被自动截断而不报错。
如果当前没有地址绑定到该套接字，则其结果是未定义的。

如果套接字已经和对等方连接，可以调用getpeername函数来找到对方
的地址。
```c
#include <sys/socket.h>
int getpeername(int sockfd, struct sockaddr *restrict addr,
socklen_t *restrict alenp);
```

返回值：若成功，返回0；若出错，返回−1

除了返回对等方的地址，函数getpeername和getsockname一样。

## 建立连接
如果要处理一个面向连接的网络服务（SOCK_STREAM或SOCK_SEQPACKET），那么在开始交换数据以前，需要在请求服务的进程套接字（客户端）和提供服务的进程套接字（服务器）之间建立一个连接。使用connect函数来建立连接。

```c
#include <sys/socket.h>
int connect(int sockfd, const struct sockaddr *addr, socklen_t len);
```
返回值：
- 若成功，返回0；
- 若出错，返回−1

在connect中指定的地址是我们想与之通信的服务器地址。如果sockfd没有绑定到一个地址，connect会给调用者绑定一个默认地址。

当尝试连接服务器时，出于一些原因，连接可能会失败。要想一个连接请求成功，要连接的计算机必须是开启的，并且正在运行，服务器必须绑定到一个想与之连接的地址上，并且服务器的等待连接队列要有足够的空间（后面会有更详细的介绍）。因此，应用程序必须能够处理connect返回的错误，这些错误可能是由一些瞬时条件引起的。

例子，支持重试的connect。
```c
#include "apue.h"
#include <sys/socket.h>

#define MAXSLEEP 128

int
connect_retry(int sockfd, const struct sockaddr *addr, socklen_t alen)
{
	int numsec;

	/*
	 * Try to connect with exponential backoff.
	 */
	for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) {
		if (connect(sockfd, addr, alen) == 0) {
			/*
			 * Connection accepted.
			 */
			return(0);
		}

		/*
		 * Delay before trying again.
		 */
		if (numsec <= MAXSLEEP/2)
			sleep(numsec);
	}
	return(-1);
}

```
这个函数展示了指数补偿（exponential backoff）算法。如果调用 connect失败，进程会休眠一小段时间，然后进入下次循环再次尝试，每次循环休眠时间会以指数级增加，直到最大延迟为2分钟左右。

例子，可迁移的支持重试的连接代码。
```c
#include "apue.h"
#include <sys/socket.h>

#define MAXSLEEP 128

int
connect_retry(int domain, int type, int protocol,
              const struct sockaddr *addr, socklen_t alen)
{
	int numsec, fd;

	/*
	 * Try to connect with exponential backoff.
	 */
	for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) {
		if ((fd = socket(domain, type, protocol)) < 0)
			return(-1);
		if (connect(fd, addr, alen) == 0) {
			/*
			 * Connection accepted.
			 */
			return(fd);
		}
		close(fd);

		/*
		 * Delay before trying again.
		 */
		if (numsec <= MAXSLEEP/2)
			sleep(numsec);
	}
	return(-1);
}

```
需要注意的是，因为可能要建立一个新的套接字，给connect_retry函数传递一个套接字描述符参数是没有意义。我们现在返回一个已连接的套接字描述符给调用者，而并非返回一个表示调用成功的值。

如果套接字描述符处于非阻塞模式（该模式将在 16.8 节中进一步讨论），那么在连接不能马上建立时，connect将会返回−1并且将errno设置为特殊的错误码EINPROGRESS。应用程序可以使用poll或者select来判断文件描述符何时可写。如果可写，连接完成。

connect函数还可以用于无连接的网络服务（SOCK_DGRAM）。这看起来有点矛盾，实际上却是一个不错的选择。如果用SOCK_DGRAM套接字调用connect，传送的报文的目标地址会设置成connect调用中所指定的地址，这样每次传送报文时就不需要再提供地址。另外，仅能接收来自指定地址的报文。

服务器调用listen函数来宣告它愿意接受连接请求。
```c
#include <sys/socket.h>
int listen(int sockfd, int backlog);
```

返回值：
若成功，返回0；
若出错，返回−1

参数backlog提供了一个提示，提示系统该进程所要入队的未完成连接请求数量。其实际值由系统决定，但上限由 `<sys/socket.h>` 中的SOMAXCONN指定。

一旦服务器调用了listen，所用的套接字就能接收连接请求。使用accept函数获得连接请求并建立连接。
```c
#include <sys/socket.h>
int accept(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict len);
```
返回值：若成功，返回文件（套接字）描述符；若出错，返回−1

函数accept所返回的文件描述符是套接字描述符，该描述符连接到调用connect的客户端。这个新的套接字描述符和原始套接字（sockfd）具有相同的套接字类型和地址族。传给accept的原始套接字没有关联到这个连接，而是继续保持可用状态并接收其他连接请求。

如果不关心客户端标识，可以将参数addr和len设为NULL。否则，在调用accept之前，将addr参数设为足够大的缓冲区来存放地址，并且将len指向的整数设为这个缓冲区的字节大小。返回时，accept会在缓冲区填充客户端的地址，并且更新指向len的整数来反映该地址的大小。

如果没有连接请求在等待，accept会阻塞直到一个请求到来。如果sockfd处于非阻塞模式，accept会返回−1，并将errno设置为EAGAIN或EWOULDBLOCK。

如果服务器调用accept，并且当前没有连接请求，服务器会阻塞直到一个请求到来。另外，服务器可以使用poll或select来等待一个请求的到来。在这种情况下，一个带有等待连接请求的套接字会以可读的方式出现。

例子，初始化一个套接字端点供服务器进程使用。
```c
#include "apue.h"
#include <errno.h>
#include <sys/socket.h>

int
initserver(int type, const struct sockaddr *addr, socklen_t alen,
  int qlen)
{
	int fd;
	int err = 0;

	if ((fd = socket(addr->sa_family, type, 0)) < 0)
		return(-1);
	if (bind(fd, addr, alen) < 0)
		goto errout;
	if (type == SOCK_STREAM || type == SOCK_SEQPACKET) {
		if (listen(fd, qlen) < 0)
			goto errout;
	}
	return(fd);

errout:
	err = errno;
	close(fd);
	errno = err;
	return(-1);
}
```
可以看到，TCP有一些奇怪的地址复用规则，这使得这个例子不完备。

## 数据传输
既然一个套接字端点表示为一个文件描述符，那么只要建立连接，就可以使用read和write来通过套接字通信。在套接字描述符上使用read和write是非常有意义的，因为这意味着可以将套接字描述符传递给那些原先为处理本地文件而设计的函数。而且还可以安排将套接字描述符传递给子进程，而该子进程执行的程序并不了解套接字。

最简单的是send，它和write很像，但是可以指定标志来改变处理传输数据的方式。
```c
#include <sys/socket.h>
ssize_t send(int sockfd, const void *buf, size_t nbytes, int flags);
```
返回值：
- 若成功，返回发送的字节数；
- 若出错，返回−1

类似write，使用send时套接字必须已经连接。参数buf和nbytes的含义与write中的一致。与write不同的是，send支持第4个参数flags。 

send套接字调用标志

| 标志          | 描述                                   | POSIX.1 | FreeBSD8.0 | Linux3.2.0 | Mac OSX 10.6.8 |
| ------------- | -------------------------------------- | ------- | ---------- | ---------- | -------------- |
| MSG_CONFIRM   | 提供链路层反馈以保持地址映射有效       |         |            | •          |                |
| MSG_DONTROUTE | 勿将数据包路由出本地网络               |         | •          | •          | •              |
| MSG_DONTWAIT  | 允许非阻塞操作(等价于使用O_NONBLOCK)   |         | •          | •          | •              |
| MSG_EOF       | 发送数据后关闭套接字的发送端           |         | •          |            | •              |
| MSG_EOR       | 如果协议支持，标记记录结束             | •       | •          | •          | •              |
| MSG_MORE      | 延迟发送数据包允许写更多数据           |         |            | •          |                |
| MSG_NOSIGNAL  | 在写无连接的套接字时不产生SIGPIPE 信号 | •       | •          | •          |                |
| MSG_OOB       | 如果协议支持，发送带外数据             | •       | •          | •          | •              |

即使send成功返回，也并不表示连接的另一端的进程就一定接收了数据。我们所能保证的只是当send成功返回时，数据已经被无错误地发送到网络驱动程序上。

对于支持报文边界的协议，如果尝试发送的单个报文的长度超过协议所支持的最大长度，那么send会失败，并将errno设为EMSGSIZE。对于字节流协议，send会阻塞直到整个数据传输完成。函数sendto和send很类似。区别在于sendto可以在无连接的套接字上指定一个目标地址。
```c
#include <sys/socket.h>
ssize_t sendto(int sockfd, const void *buf, size_t nbytes, int flags,
                const struct sockaddr *destaddr, socklen_t destlen);
```
返回值：若成功，返回发送的字节数；若出错，返回−1

对于面向连接的套接字，目标地址是被忽略的，因为连接中隐含了目
标地址。对于无连接的套接字，除非先调用connect设置了目标地址，否则
不能使用send。sendto提供了发送报文的另一种方式。

通过套接字发送数据时，还有一个选择。可以调用带有msghdr结构的
sendmsg来指定多重缓冲区传输数据，这和writev函数很相似

```c
#include <sys/socket.h>
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
```
返回值：若成功，返回发送的字节数；若出错，返回−1

POSIX.1定义了msghdr结构，它至少有以下成员：
```c
struct msghdr {
void *msg_name; /* optional address */
socklen_t msg_namelen; /* address size in bytes
*/
struct iovec *msg_iov; /* array of I/O buffers
*/
int msg_iovlen; /* number of elements in
array */
void *msg_control; /* ancillary data */
socklen_t msg_controllen; /* number of ancillary
bytes */
int msg_flags; /* flags for received
message */
};
```

函数recv和read相似，但是recv可以指定标志来控制如何接收数据。
```c
#include <sys/socket.h>
ssize_t recv(int sockfd, void *buf, size_t nbytes, int flags);
```
返回值：返回数据的字节长度；若无可用数据或对等方已经按序结束，返回0；若出错，返回−1

recv套接字调用标志

| 标志             | 描述                                                           | POSIX.1 | FreeBSD8.0 | Linux3.2.0 | Mac OSX 10.6.8 |
| ---------------- | -------------------------------------------------------------- | ------- | ---------- | ---------- | -------------- |
| MSG_CMSG_CLOEXEC | 为UNIX 域套接字上接收的文件描述符设置执行时关闭标志(见17.4 节) |         |            |            |                |
| MSG_DONTWAIT     | 启用非阻塞操作(相当于使用O_NONBLOCK)                           |         | •          | •          |                |
| MSG_ERRQUEUE     | 接收错误信息作为辅助数据                                       |         |            | •          |                |
| MSG_OOB          | 如果协议支持，获取带外数据(见16.7 节)                          | •       | •          | •          | •              |
| MSG_PEEK         | 返回数据包内容而不真正取走数据包                               | •       | •          | •          | •              |
| MSG_TRUNC        | 即使数据包被截断，也返回数据包的实际长度                       |         |            | •          |                |
| MSG_WAITALL      | 等待直到所有的数据可用(仅SOCK_STREAM)                          | •       | •          | •          | •              |

当指定MSG_PEEK标志时，可以查看下一个要读取的数据但不真正
取走它。当再次调用read或其中一个recv函数时，会返回刚才查看的数
据。

对于SOCK_STREAM套接字，接收的数据可以比预期的少。
MSG_WAITALL标志会阻止这种行为，直到所请求的数据全部返回，recv
函数才会返回。对于SOCK_DGRAM和SOCK_SEQPACKET套接字，
MSG_WAITALL
标志没有改变什么行为，因为这些基于报文的套接字类
型一次读取就返回整个报文。

如果发送者已经调用shutdown（见16.2节）来结束传输，或者网络协
议支持按默认的顺序关闭并且发送端已经关闭，那么当所有的数据接收完
毕后，recv会返回0。

如果有兴趣定位发送者，可以使用recvfrom来得到数据发送者的源地址。
```c
#include <sys/socket.h>
ssize_t recvfrom(int sockfd, void *restrict buf, size_t len, int flags,
                struct sockaddr *restrict addr, socklen_t *restrict addrlen);
```
返回值：
- 返回数据的字节长度；若无可用数据或对等方已经按序结束，返回0；
- 若出错，返回−1

如果addr非空，它将包含数据发送者的套接字端点地址。当调用
recvfrom时，需要设置addrlen参数指向一个整数，该整数包含addr所指向
的套接字缓冲区的字节长度。返回时，该整数设为该地址的实际字节长
度。

因为可以获得发送者的地址，recvfrom通常用于无连接的套接字。否
则，recvfrom等同于recv。

为了将接收到的数据送入多个缓冲区，类似于readv（见14.6节），或
者想接收辅助数据（见17.4节），可以使用recvmsg。
```c
#include <sys/socket.h>
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
```

返回值：返回数据的字节长度；若无可用数据或对等方已经按序结束，返回0；若出错，返回−1

recvmsg用msghdr结构（在sendmsg中见到过）指定接收数据的输入缓冲区。可以设置参数flags来改变recvmsg的默认行为。返回时，msghdr结构中的msg_flags字段被设为所接收数据的各种特征。（进入recvmsg时msg_flags被忽略。）recvmsg中返回的各种可能值总结在图16-15中。

 从recvmsg中返回的msg_flags标志

| 标志         | 描述                     | POSIX.1 | FreeBSD8.0 | Linux3.2.0 | Mac OSX 10.6.8 |
| ------------ | ------------------------ | ------- | ---------- | ---------- | -------------- |
| MSG_CTRUNC   | 控制数据被截断           | •       | •          | •          | •              |
| MSG_EOR      | 接收记录结束符           | •       | •          | •          | •              |
| MSG_ERROUEUE | 接收错误信息作为辅助数据 |         |            | •          |                |
| MSG_OOB      | 接收带外数据             | •       | •          | •          | •              |
| MSG_TRUNC    | 一般数据被截断           | •       | •          | •          | •              |

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
这个程序连接服务器，读取服务器发送过来的字符串并将其打印到标
准输出。因为使用的是SOCK_STREAM
套接字，所以不能保证调用一次
recv 就会读取整个字符串，因此需要重复调用直到它返回0。

如果服务器支持多重网络接口或多重网络协议，函数getaddrinfo可能
会返回多个候选地址供使用。轮流尝试每个地址，当找到一个允许连接到
服务的地址时便可停止。使用图16-11中的connect_retry函数来与服务器建
立一个连接。

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
为了找到它的地址，服务器需要获得其运行时的主机名。如果主机名
的最大长度不确定，可以使用HOST_NAME_MAX代替。如果系统没定义
HOST_NAME_MAX，可以自己定义。POSIX.1要求主机名的最大长度至
少为255字节，不包括终止null字符，因此定义HOST_NAME_MAX为256来
包括终止null字符。

服务器调用gethostname获得主机名，查看远程正常运行时间服务的地
址。可能会有多个地址返回，但我们简单地选择第一个来建立被动套接字
端点（即一个只用于监听连接请求的地址）。处理多个地址作为习题留给
读者。


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
我们没有采用popen来运行uptime命令，并从连接到命令标准输出的管
道读取输出，而是采用fork创建了一个子进程，然后使用dup2使
STDIN_FILENO的子进程副本对/dev/null开放，使STDOUT_FILENO 和
STDERR_FILENO 的子进程副本对套接字端点开放。当执行uptime时，命
令将结果写到它的标准输出，该标准输出是连接到套接字的，所以数据被
送到ruptime客户端命令。

父进程可以安全地关闭连接到客户端的文件描述符，因为子进程仍旧
让它打开着。父进程会等待子进程处理完毕再继续，所以子进程不会变成
僵死进程。由于运行uptime命令不会花费太长的时间，所以父进程在接受
下一个连接请求之前，可以等待子进程退出。然而，如果子进程运行的时
间比较长的话，这种策略就未必适合了。

前面的实例采用的都是面向连接的套接字。但如何选择合适的套接字
类型呢？何时采用面向连接的套接字，何时采用无连接的套接字呢？答案
取决于我们要做的工作量和能够容忍的出错程度。

对于无连接的套接字，数据包到达时可能已经没有次序，因此如果不
能将所有的数据放在一个数据包里，则在应用程序中就必须关心数据包的
次序。数据包的最大尺寸是通信协议的特征。另外，对于无连接的套接
字，数据包可能会丢失。如果应用程序不能容忍这种丢失，必须使用面向
连接的套接字。

容忍数据包丢失意味着两种选择。一种选择是，如果想和对等方可靠
通信，就必须对数据包编号，并且在发现数据包丢失时，请求对等应用程
序重传，还必须标识重复数据包并丢弃它们，因为数据包可能会延迟或疑
似丢失，可能请求重传之后，它们又出现了。

另一种选择是，通过让用户再次尝试那个命令来处理错误。对于简单
的应用程序，这可能就足够了，但对于复杂的应用程序，这种选择通常不
可行。因此，一般在这种情况下使用面向连接的套接字比较好。
面向连接的套接字的缺陷在于需要更多的时间和工作来建立一个连
接，并且每个连接都需要消耗较多的操作系统资源。

例子， 采用数据报套接字接口的uptime客户端命令版本。
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

除了增加安装一个SIGALRM的信号处理程序以外，基于数据报的客
户端中的main函数和面向连接的客户端中的类似。使用alarm函数来避免调
用recvfrom时的无限期阻塞。
对于面向连接的协议，需要在交换数据之前连接到服务器。对于服务
器来说，到来的连接请求已经足够判断出所需提供给客户端的服务。但是
对于基于数据报的协议，需要有一种方法通知服务器来执行服务。本例
中，只是简单地向服务器发送了 1 字节的数据。服务器将接收它，从数据
包中得到地址，并使用这个地址来传送它的响应。如果服务器提供多个服
务，可以使用这个请求数据来表示需要的服务，但由于服务器只做一件事
情，1字节数据的内容是无关紧要的。
如果服务器不在运行状态，客户端调用recvfrom便会无限期阻塞。对
于这个面向连接的实例，如果服务器不运行，connect
调用会失败。为了
避免无限期阻塞，可以在调用 recvfrom之前设置警告时钟。

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
服务器在recvfrom阻塞等待服务请求。当一个请求到达时，保存请求
者地址并使用popen来运行uptime命令。使用sendto函数将输出发送到客户
端，将目标地址设置成刚才的请求者地址。


## 套接字选项
套接字机制提供了两个套接字选项接口来控制套接字行为。一个接口
用来设置选项，另一个接口可以查询选项的状态。可以获取或设置以下3
种选项。
（1）通用选项，工作在所有套接字类型上。
（2）在套接字层次管理的选项，但是依赖于下层协议的支持。
（3）特定于某协议的选项，每个协议独有的。
Single UNIX Specification定义了套接字层的选项（上述选项中的前两
个选项类型）。
可以使用setsockopt函数来设置套接字选项。
```c
#include <sys/socket.h>
int setsockopt(int sockfd, int level, int option, const void
*val, socklen_t len);
```

返回值：若成功，返回0；若出错，返回−1

参数 level 标识了选项应用的协议。如果选项是通用的套接字层次选
项，则 level 设置成SOL_SOCKET。否则，level设置成控制这个选项的协
议编号。对于TCP选项，level是IPPROTO_TCP，对于IP，level是
IPPROTO_IP。

套接字选项
| 选项          | 参数val的类型  | 描述                                            |
| ------------- | -------------- | ----------------------------------------------- |
| SO_ACCEPTCONN | int            | 返回信息指示该套接字是否能被监听(仅 getsockopt) |
| SO_BROADCAST  | int            | 如果*val非0，广播数据报                         |
| SO_DEBUG      | int            | 如果*val非0，启用网络驱动调试功能               |
| SO_DONTROUTE  | int            | 如果*val非0，绕过通常路由                       |
| SO_ERROR      | int            | 返回挂起的套接字错误并清除(仅 getsockopt)       |
| SO_KEEPALIVE  | int            | 如果*va1非0，启用周期性 ecp-alive 报文          |
| SO_LINGER     | struct linger  | 当还有未发报文而套接字已关闭时，延迟时间        |
| SO_OOBINLINE  | int            | 如果*val非0，将带外数据放在普通数据中           |
| SO_RCVBUF     | int            | 接收缓冲区的字节长度                            |
| SO_RCVLOWAT   | int            | 接收调用中返回的最小数据字节数                  |
| SO_RCVTIMEO   | struct timeval | 套接字接收调用的超时值                          |
| SO_REUSEADDR  | int            | 如果*val非0，重用bind 中的地址                  |
| SO_SNDBUF     | int            | 发送缓冲区的字节长度                            |
| SO_SNDLOWAT   | int            | 发送调用中传送的最小数据字节数                  |
| SO_SNDTIMEO   | struct timeval | 套接字发送调用的超时值                          |
| SO_TYPE       | int            | 标识套接字类型(仅 getsockopt)                   |


参数val根据选项的不同指向一个数据结构或者一个整数。一些选项是
on/off开关。如果整数非0，则启用选项。如果整数为0，则禁止选项。参
数len指定了val指向的对象的大小。
可以使用getsockopt函数来查看选项的当前值。

```c
#include <sys/socket.h>
int getsockopt(int sockfd, int level, int option, void
*restrict val,
socklen_t *restrict lenp);
```

返回值：若成功，返回0；若出错，返回−1

参数lenp是一个指向整数的指针。在调用getsockopt之前，设置该整数
为复制选项缓冲区的长度。如果选项的实际长度大于此值，则选项会被截
断。如果实际长度正好小于此值，那么返回时将此值更新为实际长度。

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
为了启用SO_REUSEADDR选项，设置了一个非0值的整数，并把这
个整数地址作为val参数传递给了setsockopt。将len参数设置成了一个整数
大小来表明val所指的对象的大小。
## 带外数据

带外数据（out-of-band data）是一些通信协议所支持的可选功能，与
普通数据相比，它允许更高优先级的数据传输。带外数据先行传输，即使
传输队列已经有数据。TCP 支持带外数据，但是UDP不支持。套接字接口
对带外数据的支持很大程度上受TCP带外数据具体实现的影响。
TCP将带外数据称为紧急数据（urgent data）。TCP仅支持一个字节的
紧急数据，但是允许紧急数据在普通数据传递机制数据流之外传输。为了
产生紧急数据，可以在3个send函数中的任何一个里指定MSG_OOB标志。
如果带MSG_OOB标志发送的字节数超过一个时，最后一个字节将被视为
紧急数据字节。
如果通过套接字安排了信号的产生，那么紧急数据被接收时，会发送
SIGURG信号。在3.14节和14.5.2节中可以看到，在fcntl中使用F_SETOWN
命令来设置一个套接字的所有权。如果fcntl中的第三个参数为正值，那么
它指定的就是进程ID。如果为非-1的负值，那么它代表的就是进程组ID。
因此，可以通过调用以下函数安排进程接收套接字的信号：
fcntl(sockfd, F_SETOWN, pid);
F_GETOWN命令可以用来获得当前套接字所有权。对于F_SETOWN
命令，负值代表进程组ID，正值代表进程ID。因此，调用
owner = fcntl(sockfd, F_GETOWN, 0);
将返回owner，如果owner为正值，则等于配置为接收套接字信号的进
程的ID。如果owner为负值，其绝对值为接收套接字信号的进程组的ID。
TCP支持紧急标记（urgent mark）的概念，即在普通数据流中紧急数
据所在的位置。如果采用套接字选项SO_OOBINLINE，那么可以在普通
数据中接收紧急数据。为帮助判断是否已经到达紧急标记，可以使用函数
sockatmark。
```c
#include <sys/socket.h>
int sockatmark(int sockfd);
```
返回值：若在标记处，返回1；若没在标记处，返回0；若出错，返回−1
当下一个要读取的字节在紧急标志处时，sockatmark返回1。
当带外数据出现在套接字读取队列时，select函数（见14.4.1节）会返
回一个文件描述符并且有一个待处理的异常条件。可以在普通数据流上接
收紧急数据，也可以在其中一个recv函数中采用MSG_OOB标志在其他队
列数据之前接收紧急数据。TCP队列仅用一个字节的紧急数据。如果在接
收当前的紧急数据字节之前又有新的紧急数据到来，那么已有的字节会被
丢弃。

## 非阻塞和异步I/O

通常，recv 函数没有数据可用时会阻塞等待。同样地，当套接字输出
队列没有足够空间来发送消息时，send 函数会阻塞。在套接字非阻塞模式
下，行为会改变。在这种情况下，这些函数不会阻塞而是会失败，将errno
设置为EWOULDBLOCK或者EAGAIN。当这种情况发生时，可以使用poll
或select来判断能否接收或者传输数据。
Single UNIX Specification包含通用异步I/O机制（见14.5节）的支持。
套接字机制有其自己的处理异步I/O的方式，但是这在Single
UNIX
Specification中没有标准化。一些文献把经典的基于套接字的异步I/O机制
称为“基于信号的I/O”，区别于Single UNIX Specification中的通用异步I/
O机制。
在基于套接字的异步I/O中，当从套接字中读取数据时，或者当套接
字写队列中空间变得可用时，可以安排要发送的信号SIGIO。启用异步I/
O是一个两步骤的过程。
（1）建立套接字所有权，这样信号可以被传递到合适的进程。
（2）通知套接字当I/O操作不会阻塞时发信号。
可以使用3种方式来完成第一个步骤。
（1）在fcntl中使用F_SETOWN命令。
（2）在ioctl中使用FIOSETOWN命令。
（3）在ioctl中使用SIOCSPGRP命令。
要完成第二个步骤，有两个选择。
（1）在fcntl中使用F_SETFL命令并且启用文件标志O_ASYNC。
（2）在ioctl中使用FIOASYNC命令。
虽然有多种选项，但它们没有得到普遍支持。图16-23总结了本文讨
论的平台支持这些选项的情况。

套接字异步I/O管理命令

| 机制                               | POSIX.1 | FreeBSD8.0 | Linux3.2.0 | Mac OSX 10.6.8 |
| ---------------------------------- | ------- | ---------- | ---------- | -------------- |
| fcntl(fd, F_SETOWN,pid)            | •       | •          | •          | •              |
| ioctl(fd, FIOSETOWN,pid)           |         | •          | •          | •              |
| ioctl(fd, SIOCSPGRP,pid)           |         | •          | •          | •              |
| fcntl(fd, F_SETFL, flags\|O_ASYNC) |         | •          | •          | •              |
| ioctl(fd, FIOASYNC, &n);           |         | •          | •          | •              |