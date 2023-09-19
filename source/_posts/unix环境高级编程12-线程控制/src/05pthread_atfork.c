#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void prepare(){
	int err;
	printf("preparing locks...\n");
	if((err = pthread_mutex_lock(&lock1)) != 0)
		perror("can't lock lock1 in prepare handler");
	if((err = pthread_mutex_lock(&lock2)) != 0)
		perror("can't lock lock2 in prepare handler");
}

void parent(){
	int err;
	printf("parent unlocking locks...\n");
	if((err = pthread_mutex_unlock(&lock1)) != 0)
		perror("can't unlock lock1 in parent handler");
	if((err = pthread_mutex_unlock(&lock2)) != 0)
		perror("can't unlock lock2 in parent handler");
}

void child(){
	int err;
	printf("child unlocking locks...\n");
	if((err = pthread_mutex_unlock(&lock1)) != 0)
		perror("can't unlock lock1 in child handler");
	if((err = pthread_mutex_unlock(&lock2)) != 0)
		perror("can't unlock lock2 in child handler");
}

void *thr_fn(void *arg){
	printf("thread started...\n");
	pause();
	return 0;
}

int main(){
	int err;
	pid_t pid;
	pthread_t tid;

	if((err = pthread_atfork(prepare, parent,child)) != 0){
		perror("can't install fork handlers");
		exit(1);
	}
	if((err = pthread_create(&tid, NULL, thr_fn, 0)) != 0){
		perror("can't create thread");
		exit(1);
	}

	sleep(2);
	printf("parent about to fork...\n");

	if((pid = fork()) < 0){
		perror("fork failed");
		exit(1);
	}else if(pid == 0)
		printf("child returned from fork.\n");
	else
		printf("parent returned from fork.\n");
	return 0;
}
