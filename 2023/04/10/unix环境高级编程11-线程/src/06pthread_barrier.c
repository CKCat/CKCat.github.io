#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include <sys/time.h>
#include <stdlib.h>

#define NTHR 8			// 线程数量
#define NUMNUM 8000000L	// 需要排序的数据数量
#define TNUM (NUMNUM / NTHR)

long random_nums[NUMNUM];
long sorted_nums[NUMNUM];

pthread_barrier_t b;

int complong(const void *arg1, const void *arg2){
	long l1 = *(long *)arg1;
	long l2 = *(long *)arg2;
	if(l1 == l2)
		return 1;
	else if(l1 < l2)
		return -1;
	else
		return 1;
}

void *thr_fn(void *arg){
	long idx = (long)arg;
	qsort(&random_nums[idx], TNUM, sizeof(long), complong);
	pthread_barrier_wait(&b);
	return (void*)0;
}

void merge(){
	long idx[NTHR];
	long i, minidx, sidx, num;
	for(i = 0; i < NTHR; i++)
		idx[i] = i * TNUM;
	for(sidx = 0; sidx < NUMNUM; sidx++){
		num = LONG_MAX;
		for(i = 0; i < NTHR; i++){
			if((idx[i] < ((i+1) * TNUM)) && (random_nums[idx[i]] < num)){
				num = random_nums[idx[i]];
				minidx = i;
			}
		}
		sorted_nums[sidx] = random_nums[idx[minidx]];
		idx[minidx]++;
	}
}

int main(){
	unsigned long i;
	struct timeval start, end;
	long long startusec, endusec;
	double elapsed;
	int err;
	pthread_t tid;

	srandom(1);
	for (i = 0; i < NUMNUM; i++)
		random_nums[i] = random();
	gettimeofday(&start, NULL);
	pthread_barrier_init(&b, NULL, NTHR + 1);
	for (i = 0; i < NTHR; i++){
		// 使用8个线程分解了800万个数的排序工作。每个线程用堆排序算法对100万个数进行排序
		err = pthread_create(&tid, NULL, thr_fn, (void *)(i * TNUM));
		if(err != 0){
			perror(" can't create thread");
			return 1;
		}
	}
	pthread_barrier_wait(&b);
	// 主线程调用一个函数对这些结果进行合并。
	merge();
	gettimeofday(&end, NULL);

	startusec = start.tv_sec * 1000000 + start.tv_usec;
	endusec = end.tv_sec * 1000000 + end.tv_usec;
	elapsed = (double)(endusec - startusec) / 1000000.0;
	printf("%d thread sort took %.4f seconds\n", NTHR, elapsed);
	// for (i = 0; i < NUMNUM; i++)
	// 	printf("%ld\n", sorted_nums[i]);

	gettimeofday(&start, NULL);

	// 主线程调用 qsort 进行排序
	qsort(&random_nums, NUMNUM, sizeof(long), complong);

	gettimeofday(&end, NULL);
	startusec = start.tv_sec * 1000000 + start.tv_usec;
	endusec = end.tv_sec * 1000000 + end.tv_usec;
	elapsed = (double)(endusec - startusec) / 1000000.0;
	printf("1 thread sort took %.4f seconds\n", elapsed);
	return 0;
}
	
