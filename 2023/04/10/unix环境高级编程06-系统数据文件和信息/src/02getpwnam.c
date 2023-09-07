#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <pwd.h>

struct passwd *my_getpwnam(const char *name){
	struct passwd *ptr;
	// passwd 文件可能被其他进程打开，确保定位到文件开始处。
	setpwent();
	while((ptr = getpwent()) != NULL){
		if(strcmp(name, ptr->pw_name) == 0)
			break;
	}
	endpwent(); // 关闭 passwd 文件
	return ptr;
}

int main(){
	struct passwd *user_info = my_getpwnam("ckcat");
	if (user_info != NULL) {
		printf("用户名: %s\n", user_info->pw_name);
		printf("用户主目录: %s\n", user_info->pw_dir);
		printf("用户ID: %d\n", user_info->pw_uid);
		printf("组ID: %d\n", user_info->pw_gid);
		// 其他用户信息字段也可以通过结构体访问
	} else {
		perror("获取用户信息时出错");
	}
	return 0;
}
