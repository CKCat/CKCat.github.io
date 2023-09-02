#include <stdio.h>
#include <time.h>

int main(int argc, char* argv[]){
	clock_t start_time, end_time;
	double cup_time_used;
	int sum = 0;
	start_time = clock();
	for(int i = 0; i < 1000000; i++){
		sum += i;
		printf("%d\n", sum);
	}
	end_time = clock();
	cup_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
	printf("CLOCKS_PER_SEC = %ld, cup_time_used = %lfs, sum = %d\n",CLOCKS_PER_SEC, cup_time_used, sum);
	return 0;
}
