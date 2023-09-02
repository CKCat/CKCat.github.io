#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>

int main(int argc, char* argv[]){
	struct tms start_time, end_time;
	clock_t start_clock, end_clock;
	int sum = 0;
	long ticks_per_second = sysconf(_SC_CLK_TCK);
	start_clock = times(&start_time);
	for(int i = 0; i < 1000000; i++){
			sum += i;
			printf("%d\n", sum);
    }
	end_clock = times(&end_time);
	double user_time = (double)(end_time.tms_utime - start_time.tms_utime) / ticks_per_second;
    double system_time = (double)(end_time.tms_stime - start_time.tms_stime) / ticks_per_second;
	double cpu_time_used = (double)(end_time.tms_utime + end_time.tms_stime - start_time.tms_utime - start_time.tms_stime) / ticks_per_second;
	printf("ticks_per_second = %ld, cpu_time_used = %lf, sum = %d\n", ticks_per_second, cpu_time_used, sum);
	printf("用户模式时间：%f 秒\n", user_time);
    printf("系统模式时间：%f 秒\n", system_time);
	return 0;
}

