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

/**
 * 主函数
 * @param argc 命令行参数的数量
 * @param argv 命令行参数的数组
 * @return int 返回状态码，0表示成功，1表示失败
 *
 * 该程序使用getaddrinfo函数获取指定主机名和服务名的地址信息，
 * 并打印出相关的信息，包括标志、地址族、套接字类型、协议类型、
 * 主机名、地址和端口号。
 * 
 * 使用方法：程序名 主机名 服务名
 * 如果参数数量不正确，程序将打印出使用方法并退出。
 * 如果getaddrinfo函数调用失败，程序将打印出错误信息并退出。
 */
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