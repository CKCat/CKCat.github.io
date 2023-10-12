#include <stdio.h>
#include <sys/wait.h>

#define PAGER "${PAGER:-more}"	// 使用环境变量 PAGER 的值，如果没有设置，则使用 more

int main(int argc, char *argv[]){
	char line[4096];
	FILE *fpin, *fpout;
	if(argc != 2){
		printf("usage: a.out <pathname>");
		return 0;
	}
	if((fpin = fopen(argv[1], "r")) == NULL){
		perror("can open file");
		return 1;
	}
	if((fpout = popen(PAGER, "w")) == NULL){
		perror("popen error");
		return 1;
	}
	// 复制 argv[1] 的内容到 pager
	while(fgets(line, 4096, fpin) != NULL){
		if(fputs(line, fpout) == EOF){
			perror("fputs error to pipe");
			return 1;
		}
	}
	if(ferror(fpin)){
		perror("fgets error");
		return 1;
	}
	if(pclose(fpout) == -1){
		perror("pclose error");
		return 1;
	}
	return 0;
}
