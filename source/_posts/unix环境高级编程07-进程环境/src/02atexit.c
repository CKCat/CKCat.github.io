#include <stdlib.h>
#include <stdio.h>

static void my_exit1(void);
static void my_exit2(void);

static int count = 0;

int main(){
	if(atexit(my_exit1) != 0)
		perror("can't register my_exit1");
	if(atexit(my_exit2) != 0)
		perror("can't register my_exit2");
	if(atexit(my_exit2) != 0)
		perror("can't register my_exit2");
	printf("main done.\n");
	return 0;
}

static void my_exit1(){
	count++;
	printf("first exit hander. count = %d\n", count);
}
static void my_exit2(){
	count++;
	printf("second exit hander. count = %d\n", count);
}

