#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(){
	struct timespec delay;
	delay.tv_sec = 2;
	delay.tv_nsec = 500000000;  // 休眠500毫秒
	printf("sleep for 2.5 s.\n");
	if(nanosleep(&delay, NULL) == 0){
		printf("Woke up after sleeping.\n");
	}else{
		perror("nanosleep");
	}
	
	printf("sleep for 2.5 s.\n");
	if(clock_nanosleep(CLOCK_REALTIME, 0, &delay, NULL) == 0){
		printf("Woke up after sleeping.\n");
	}else{
		perror("clock_nanosleep");
	}
	return 0;
}
