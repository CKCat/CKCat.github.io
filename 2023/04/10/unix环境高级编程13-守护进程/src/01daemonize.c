#include <stdio.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>

void daemonize(const char *cmd){
	int i, fd0, fd1, fd2;
	pid_t pid;
	struct rlimit rl;
	struct sigaction sa;

	// 清除文件模式创建掩码
	umask(0);

	// 获取文件描述符的最大数量
	if(getrlimit(RLIMIT_NOFILE, &rl) < 0){
		perror("can't get file limit");
		exit(1);
	}
	// 父进程退出，保证了子进程不是一个进程组的组长进程
	if((pid = fork()) < 0){
		perror("can't fork");
		exit(1);
	}else if(pid != 0)
		exit(0);
	// 创建一个新会话
	setsid();

	// 成为新会话的首进程以解除对控制终端的控制
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(SIGHUP, &sa, NULL) < 0){
		perror("can't ignore SIGHUP");
		exit(1);
	}
	if((pid = fork()) < 0){
		perror("can't fork");
		exit(1);
	}else if(pid != 0)
		exit(0);

	// 将当前工作目录更改为根目录
	if(chdir("/") < 0){
		perror("can't change directory to /");
		exit(1);
	}

	// 关闭不再需要的文件描述符
	if(rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for(i = 0; i < rl.rlim_max; i++)
		close(i);
	// 打开 `/dev/null` 使其具有文件描述符 0、1 和 2
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	// 初始化 log 文件
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if(fd0 != 0 || fd1 != 1 || fd2 != 2){
		syslog(LOG_ERR,"unexpected file descriptors %d %d %d", fd0, fd1, fd2);
		exit(1);
	}
}

// int main(int argc, char const *argv[]){
// 	daemonize(argv[0]);
// 	pause();
// 	return 0;
// }
