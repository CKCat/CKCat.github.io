#include <pthread.h>
#include <syslog.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int lockfile(int);
extern int already_running(void);

void reread(void){
	// 重新读取配置文件
}

void sigterm(int signo){
	syslog(LOG_INFO, "got SIGTERM; exiting");
	exit(0);
}

void sighup(int signo){
	syslog(LOG_INFO, "Re-reading configuration file");
	reread();
}

int main(int argc, char *argv[]){
	char				*cmd;
	struct sigaction	sa;

	if ((cmd = strrchr(argv[0], '/')) == NULL)
		cmd = argv[0];
	else
		cmd++;

	// 成为一个守护进程
	daemonize(cmd);

	// 确保只有一个守护进程运行
	if (already_running()) {
		syslog(LOG_ERR, "daemon already running");
		exit(1);
	}

	// 注册信号处理程序
	sa.sa_handler = sigterm;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGHUP);
	sa.sa_flags = 0;
	if (sigaction(SIGTERM, &sa, NULL) < 0) {
		syslog(LOG_ERR, "can't catch SIGTERM: %s", strerror(errno));
		exit(1);
	}
	sa.sa_handler = sighup;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGTERM);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0) {
		syslog(LOG_ERR, "can't catch SIGHUP: %s", strerror(errno));
		exit(1);
	}

	// 继续进行后续的守护进程操作
	// ......
	exit(0);
}