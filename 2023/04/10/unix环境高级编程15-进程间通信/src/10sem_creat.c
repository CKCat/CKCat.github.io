#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main(){
	key_t key;
	int semid;
	// 生成一个唯一的key
	if((key = ftok("/tmp", 'A')) == -1){
		perror("ftok error");
		exit(1);
	}
	// 创建一个信号集合，包含一个信号量
	if((semid = semget(key, 1, 0666|IPC_CREAT)) == -1){
		perror("semget error");
		exit(1);
	}
	printf("Semaphore created with ID: %d\n", semid);
	return 0;
}
