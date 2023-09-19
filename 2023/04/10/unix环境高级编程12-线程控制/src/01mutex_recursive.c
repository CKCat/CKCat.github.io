#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <malloc.h>

struct to_info {
	void	      (*to_fn)(void *);	/* function */
	void           *to_arg;			/* argument */
	struct timespec to_wait;		/* time to wait */
};

#define SECTONSEC  1000000000	/* seconds to nanoseconds */

int makethread(void *(*fn)(void *), void *arg){
	int err;
	pthread_t tid;
	pthread_attr_t attr;

	err = pthread_attr_init(&attr);
	if(err != 0)
		return err;
	err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(err == 0)
		err = pthread_create(&tid, &attr, fn, arg);
	pthread_attr_destroy(&attr);
	return err;
}

void *timeout_helper(void *arg){
	struct to_info *tip;
	tip = (struct to_info *)arg;
	clock_nanosleep(CLOCK_REALTIME, 0, &tip->to_wait, NULL);
	(*tip->to_fn)(tip->to_arg);
	free(arg);
	return 0;
}

void timeout(const struct timespec *when, void (*func)(void*), void *arg){
	struct timespec now;
	struct to_info *tip;
	int err;

	clock_gettime(CLOCK_REALTIME, &now);
	if((when->tv_sec > now.tv_sec) ||
		(when->tv_sec == now.tv_sec && when->tv_nsec > now.tv_nsec)){
		tip = malloc(sizeof(struct to_info));
		if(tip != NULL){
			tip->to_fn = func;
			tip->to_arg = arg;
			tip->to_wait.tv_nsec = when->tv_sec - now.tv_sec;
			if(when->tv_nsec >= now.tv_nsec){
				tip->to_wait.tv_nsec = when->tv_nsec - now.tv_nsec;
			}else{
				tip->to_wait.tv_sec--;
				tip->to_wait.tv_nsec = SECTONSEC - now.tv_nsec + when->tv_nsec;
			}
			err = makethread(timeout_helper, (void *)tip);
			if(err == 0)
				return;
			else
				free(tip);
		}
	}
	/**
	 * 发生下面几种情况才会执行到这里
	 * 1. when <= now
	 * 2. malloc 失败
	 * 3. 无法创建一个线程，所以现在就直接调用这个函数。
	 */
	(*func)(arg);
}

pthread_mutexattr_t attr;
pthread_mutex_t mutex;

void retry(void *arg){
	pthread_mutex_lock(&mutex);

	// 执行重试步骤......

	pthread_mutex_unlock(&mutex);
}

int main(){
	int err, condition, arg;
	struct timespec when;
	if((err = pthread_mutexattr_init(&attr)) != 0){
		perror("pthread_mutexattr_init failed");
		return 1;
	}
	if((err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)) != 0){
		perror("can't set recursive type");
		return 1;
	}
	if((err = pthread_mutex_init(&mutex, &attr)) != 0){
		perror("can't create recursive mutex");
		return 1;
	}
	// 继续处理......

	pthread_mutex_lock(&mutex);
	// 将条件检查和超时调用放在锁的保护下，以使检查和超时调用是原子操作。
	if(condition){
		// 计算我们希望重试的绝对时间。
		clock_gettime(CLOCK_REALTIME, &when);
		when.tv_sec += 10;
		timeout(&when, retry, (void *)((unsigned long)arg));
	}
	pthread_mutex_unlock(&mutex);
	// 继续处理......

	return 0;
}
