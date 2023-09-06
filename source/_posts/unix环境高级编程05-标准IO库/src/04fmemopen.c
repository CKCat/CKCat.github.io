#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BSZ 48

int main(){
	FILE *fp;
	char buf[BSZ];

	memset(buf, 'a', BSZ-2);
	buf[BSZ-2] = '\0';
	buf[BSZ-1] = 'x';
	if((fp = fmemopen(buf, BSZ, "w+")) == NULL){
		perror("fmemopen failed");
		return 1;
	}
	printf("initial buffer contens: %s\n", buf);
	fprintf(fp, "hello, world");
	printf("before flush:%s\n", buf);

	fflush(fp);
	printf("after fflush: %s\n", buf);
	printf("len of string in buf = %ld\n", (long)strlen(buf));

	memset(buf, 'b', BSZ-2);
	buf[BSZ-2] = '\0';
	buf[BSZ-1] = 'X';
	fprintf(fp, "Hello, World");
	// fseek 引起缓冲区冲洗
	fseek(fp, 0, SEEK_SET);
	printf("after fseek: %s\n", buf);
	printf("len of string in buf = %ld\n", (long)strlen(buf));

	memset(buf, 'c', BSZ-2);
	buf[BSZ-2] = '\0';
	buf[BSZ-1] = 'X';
	fprintf(fp, "Hello, World");
	// fclose 引起缓冲区冲洗
	fclose(fp);
	printf("after fseek: %s\n", buf);
	printf("len of string in buf = %ld\n", (long)strlen(buf));
	return 0;
}

