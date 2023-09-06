#include <stdio.h>

int main(){
	char name[L_tmpnam], line[4096];
	FILE *fp;
	printf("%s\n", tmpnam(NULL)); // 路径名存放在一个静态区中
	tmpnam(name);
	printf("%s\n", name);

	if((fp = tmpfile()) == NULL){
		printf("tempfile error");
		return 1;
	}
	fputs("one line of output\n", fp);
	rewind(fp);
	if(fgets(line, sizeof(line), fp) == NULL){
		printf("fgets error");
		return 1;
	}
	fputs(line, stdout);
	return 0;
}
