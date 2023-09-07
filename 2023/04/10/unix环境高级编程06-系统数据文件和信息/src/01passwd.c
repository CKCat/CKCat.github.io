#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>

int main() {
    struct passwd *user_info = getpwuid(1000);
    getpwuid(0); // user_info 被重写了
    if (user_info != NULL) {
        printf("用户名: %s\n", user_info->pw_name);
        printf("用户主目录: %s\n", user_info->pw_dir);
        printf("用户ID: %d\n", user_info->pw_uid);
        printf("组ID: %d\n", user_info->pw_gid);
        
        endpwent();  // 关闭 passwd 文件，释放资源
    } else {
        perror("获取用户信息时出错");
    }
    return 0;
}
