#include <pthread.h>
#include <syslog.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sigset_t mask;

extern void daemonize(const char *cmd);
extern int already_running();

void reread(){
	// 重新读取配置文件
}

void * thr_fn(void *arg){
	int err, signo;
	for(;;){
		err = sigwait(&mask, &signo);
		if(err != 0){
			syslog(LOG_ERR, "sigwait failed");
			exit(1);
		}

		switch(signo){
		case SIGHUP:
			syslog(LOG_INFO, "Re-reading configuration file");
			reread();
			break;
		case SIGTERM:
			syslog(LOG_INFO, "got SIGTERM; exiting");
			exit(0);
		default:
			syslog(LOG_INFO, "unexpected signal %d\n", signo);
		}
	}
	return 0;
}

int main(int argc, char *argv[]){
	int err;
	pthread_t tid;
	char *cmd;
	struct sigaction sa;

	if((cmd = strrchr(argv[0], '/')) == NULL)
		cmd = argv[0];
	else
		cmd++;
	// 成为一个守护进程
	daemonize(cmd);
	// 确保只有一个守护进程运行
	if(already_running()){
		syslog(LOG_ERR, "daemon already running");
		exit(1);
	}
	// 恢复 SIGHUP 的默认行为，并且阻塞所有信号
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(SIGHUP, &sa, NULL) < 0){
		perror("can't restore SIGHUP default");
		exit(1);
	}
	sigfillset(&mask);
	if((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0){
		perror("SIG_BLOCK error");
		exit(1);
	}
	// 创建一个线程处理 SIGHUB 和 SIGTERM
	err = pthread_create(&tid, NULL, thr_fn, 0);
	if(err != 0){
		perror("can't create thread");
		exit(1);
	}
	// 继续进行后续的守护进程操作
	
	exit(0);
}
