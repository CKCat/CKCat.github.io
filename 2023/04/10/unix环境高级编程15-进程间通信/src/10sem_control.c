#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
	
int main() {
	key_t key;
	int semid;
	int cmd;
	struct semid_ds seminfo;
	// 生成一个唯一的键
	if ((key = ftok("/tmp", 'A')) == -1) {
		perror("ftok");
		exit(1);
	}
	// 获取现有的信号量集合 ID
	if ((semid = semget(key, 0, 0666)) == -1) {
		perror("semget");
		exit(1);
	}
	// 获取信号量的信息
	cmd = IPC_STAT;
	if (semctl(semid, 0, cmd, &seminfo) == -1) {
		perror("semctl");
		exit(1);
	}
	printf("Semaphore key: 0x%x\n", key);
	printf("Semaphore ID: %d\n", semid);
	printf("Semaphore permissions: %o\n", seminfo.sem_perm.mode);
	printf("Semaphore value: %d\n", semctl(semid, 0, GETVAL));
	printf("Semaphore process owner: %d\n", seminfo.sem_perm.uid);
	return 0;
}
