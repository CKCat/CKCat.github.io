#include <stdio.h>
#include <pthread.h>

struct foo{
	int a, b, c, d;
};

void printfoo(const char *s, const struct foo *fp){
	printf("%s", s);
	printf("  structure at 0x%lx\n", (unsigned long)fp);
	printf("  foo.a = %d\n", fp->a);
	printf("  foo.b = %d\n", fp->b);
	printf("  foo.c = %d\n", fp->c);
	printf("  foo.d = %d\n", fp->d);
}

void *thr_fn(void *arg){
	struct foo fo = {1, 2, 3, 4};
	printfoo("in thread 1:\n", &fo);
	pthread_exit((void*)&fo);
}

int main(){
	int err;
	pthread_t tid;
	struct foo *fp;
	err = pthread_create(&tid, NULL, thr_fn, NULL);
	if(err != 0){
		perror("can't create thread");
		return 1;
	}
	err = pthread_join(tid, (void *)&fp);
	if(err != 0){
		perror(" can't join thread");
		return 1;
	}
	printfoo("in parent:\n", fp);
	return 0;
}
