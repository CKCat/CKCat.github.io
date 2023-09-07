#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	const char *name = "MY_NAME";
	const char *value = "Hello, World!";

	char *env_str = malloc(strlen(name) + strlen(value) + 2);
	sprintf(env_str, "%s=%s", name, value);
	// 使用 putenv 设置环境变量
	if(putenv(env_str) != 0)
		perror("putenv error");
	// 使用 getenv 获取环境变量
	char *env =getenv(name);
	if(env != NULL)
		printf("name = %s, value = %s\n",name, env);

	// 使用 setenv 设置环境变量
	if(setenv(name, "hello, world!", 1) != 0)
		perror("setenv error");

	env =getenv(name);
	if(env != NULL)
		printf("name = %s, value = %s\n",name, env);
	return 0;
}

