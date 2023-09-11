#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>

unsigned long long count;
struct timeval end;

void checktime(char *str){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	// 运行 10s 后输出计数器并终止
	if(tv.tv_sec >= end.tv_sec && tv.tv_usec >= end.tv_usec){
		printf("%s count = %lld\n", str, count);
		exit(0);
	}
}

int main(int argc, char *argv[]){
	pid_t pid;
	char *s;
	int nzero, ret;
	int adj = 0;
	setbuf(stdout, NULL);
	#if defined(_SC_NZERO)
		nzero = sysconf(_SC_NZERO);
	#else
		#error NZERO undefined
	#endif
	printf("NZERO = %d\n", nzero);
	if (argc == 2)
		adj = strtol(argv[1], NULL, 10);
	gettimeofday(&end, NULL);
	end.tv_sec += 10;
	
	if ((pid = fork()) < 0){
		perror("fork error");
		return 0;
	}else if(pid == 0){
		s = "child";
		printf("current nice value in child is %d, adjusting by %d\n", nice(0) + nzero, adj);
		if((ret = nice(adj)) == -1 && errno != 0){
			perror("child set scheduling priority");
			return 1;
		}
		printf("now child nice value is %d\n", ret + nzero);
	}else{
		s = "parent";
		printf("current nice value in parent is %d\n", nice(0) + nzero);
	}
	// 两个进程并行运行
	for(;;){
		if(++count == 0){
			printf("%s counter wrap", s);
			return 1;
		}
		checktime(s);
	}
	return 0;
}	
