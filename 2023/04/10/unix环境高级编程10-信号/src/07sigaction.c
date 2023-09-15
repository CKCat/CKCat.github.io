#include <signal.h>

typedef void Sigfunc(int); // 定义一个函数类型

Sigfunc *signal1(int signo, Sigfunc *func){
	struct sigaction act, oact;
	act.sa_handler = func;
	// 必须使用 sigemptyset 函数初始化 act 结构的 sa_mask 成员
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	// 除了 SIGALRM 以外的所有信号，我们都有意尝试设置 SA_RESTART 标志，
	// 于是被这些信号中断的系统调用都能自动重启。
	if(signo == SIGALRM){
		#ifdef SA_INTERRUPT
			act.sa_flags |= SA_INTERRUPT;
		#endif
	}else{
		act.sa_flags |= SA_RESTART;
	}
	if(sigaction(signo, &act, &oact) < 0)
		return SIG_ERR;
	return oact.sa_handler;
}
