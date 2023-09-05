#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <stdarg.h>	
#include <string.h>


typedef int MyFunc(const char*, const struct stat*, int);

static MyFunc myfunc;
static int myftw(const char*, MyFunc*);
static int dopath(MyFunc*);
char* path_alloc(size_t *sizep);
void err_ret(const char *fmt, ...);
void err_quit(const char *fmt, ...);
void err_sys(const char *fmt, ...);
void err_dump(const char *fmt, ...);

static long nreg,ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int main(int argc, char const *argv[]){
    int ret;
    if(argc != 2)
        err_quit("realloc failed");("usage: a.out <starting-pathname>");

    ret = myftw(argv[1], myfunc);
    ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
    if(ntot == 0)
        ntot = 1;
    printf("regular files   = %7ld, %5.2f %%\n", nreg, nreg*100.0/ntot);
    printf("directories     = %7ld, %5.2f %%\n", ndir, ndir*100.0/ntot);
    printf("block special   = %7ld, %5.2f %%\n", nblk, nblk*100.0/ntot);
    printf("char special    = %7ld, %5.2f %%\n", nchr, nchr*100.0/ntot);
    printf("FIFOs           = %7ld, %5.2f %%\n", nfifo, nfifo*100.0/ntot);
    printf("symblic links   = %7ld, %5.2f %%\n", nslink, nslink*100.0/ntot);
    printf("sockets         = %7ld, %5.2f %%\n", nsock, nsock*100.0/ntot);

    return 0;
}

#define FTW_F   1   // 文件
#define FTW_D   2   // 目录
#define FTW_DNR 3   // 没有读权限的目录
#define FTW_NS  4   // 无法使用 stat 访问的文件

static char *fullpath;
static size_t pathlen;

static int myftw(const char *pathname, MyFunc *func){
    fullpath = path_alloc(&pathlen);
    if(pathlen <= strlen(pathname)){
        pathlen = strlen(pathname)*2;
        if((fullpath = realloc(fullpath, pathlen)) == NULL)
            err_sys("realloc failed");
    }
    strcpy(fullpath, pathname);
    return(dopath(func));
}

static int dopath(MyFunc* func){
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    int ret, n;
    if(lstat(fullpath, &statbuf) < 0) // stat 错误
        return (func(fullpath, &statbuf, FTW_NS));
    if(S_ISDIR(statbuf.st_mode) == 0) // 文件
        return (func(fullpath, &statbuf, FTW_F));
    // 目录
    if((ret = func(fullpath, &statbuf, FTW_D)) != 0)
        return (ret);
    n = strlen(fullpath);
    if (n + NAME_MAX + 2 > pathlen){
        pathlen *= 2;
        if((fullpath = realloc(fullpath, pathlen)) == NULL)
            err_sys("realloc faild");
    }

    // 将目录添加一个 /，防止后续拼接时出现问题
    fullpath[n++] = '/';
    fullpath[n] = 0;
    if((dp = opendir(fullpath)) == NULL)
        return (func(fullpath, &statbuf, FTW_DNR));
    // 开始遍历目录
    while ((dirp  = readdir(dp)) != NULL){
        if(strcmp(dirp->d_name, ".") == 0 ||
            strcmp(dirp->d_name, "..") == 0)
            continue;
        // 拼接完整的路径
        strcpy(&fullpath[n], dirp->d_name);
        if((ret = dopath(func)) != 0)
            break;
    }
    fullpath[n-1] = 0;
    if(closedir(dp) < 0)
        err_sys("can't close directory %s", fullpath);
    return (ret);
}

static int myfunc(const char *pathname, const struct stat *statptr, int type){
    switch (type)
    {
    case FTW_F:
        switch(statptr->st_mode & S_IFMT){
            case S_IFREG: nreg++;   break;
            case S_IFBLK: nblk++;   break;
            case S_IFCHR: nchr++;   break;
            case S_IFIFO: nfifo++;  break;
            case S_IFLNK: nslink++; break;
            case S_IFSOCK:nsock++;  break;
            case S_IFDIR: err_dump("for S_IFDIR for %s", pathname);
        }
        break;
    case FTW_D:
        ndir++;
        break;
    case FTW_DNR:
        err_ret("can't read directoy %s", pathname);
        break;
    case FTW_NS:
        err_ret("stat error for %s", pathname);
        break;
    default:
        err_dump("unknown type %d for pathname %s", type, pathname);
    }
    return (0);
}

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

static void err_doit(int errnoflag, int error, const char *fmt, va_list ap){
	char	buf[4096];
	vsnprintf(buf, 4096-1, fmt, ap);
	if (errnoflag)
		snprintf(buf+strlen(buf), 4096-strlen(buf)-1, ": %s",
		  strerror(error));
	strcat(buf, "\n");
	fflush(stdout);		/* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(NULL);		/* flushes all stdio output streams */
}

void err_ret(const char *fmt, ...){
	va_list		ap;
	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
}

void err_sys(const char *fmt, ...){
	va_list		ap;
	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	exit(1);
}

void err_exit(int error, const char *fmt, ...){
	va_list		ap;
	va_start(ap, fmt);
	err_doit(1, error, fmt, ap);
	va_end(ap);
	exit(1);
}

void err_quit(const char *fmt, ...){
	va_list		ap;
	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
	exit(1);
}

void err_dump(const char *fmt, ...){
	va_list		ap;
	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	abort();		/* dump core and terminate */
	exit(1);		/* shouldn't get here */
}
