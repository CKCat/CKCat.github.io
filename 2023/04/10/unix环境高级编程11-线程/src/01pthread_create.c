#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_t ntid;

void printids(const char *s){
	pid_t pid;
	pthread_t tid;
	pid = getpid();
	tid = pthread_self();
	printf("%s pid %lu tid %lu (0x%lx)\n", s, (unsigned long)pid,
			(unsigned long)tid, (unsigned long)tid);
}

void *thr_fn(void* arg){
	printids("new thread: ");
	return (void*)0;
}

int main(){
	int err;
	err = pthread_create(&ntid, NULL, thr_fn, NULL);
	if(err != 0){
		perror("pthread_create");
		return 1;
	}
	printids("main thread: ");
	sleep(1);
	return 0;
}
