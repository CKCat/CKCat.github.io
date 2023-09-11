#include <stdio.h>
#include <unistd.h>

int main() {

    uid_t original_ruid, original_euid, original_suid;
    // 获取进程的 实际UID 有效UID 和 设置UID
    if (getresuid(&original_ruid, &original_euid, &original_suid) == -1) {
        perror("getresuid");
        return 1;
    }
    printf("Real UID (RUID): %d\n", original_ruid);
    printf("Effective UID (EUID): %d\n", original_euid);
    printf("Saved UID (SUID): %d\n", original_suid);
    // 尝试使用 setresuid 将 RUID、EUID 和 SUID 切换到其他用户
    if (setresuid(1000, 1000, 0) != 0) { 
        perror("setresuid");
        return 1;
    }
    if (getresuid(&original_ruid, &original_euid, &original_suid) == -1) {
        perror("getresuid");
        return 1;
    }
    printf("Real UID (RUID): %d\n", original_ruid);
    printf("Effective UID (EUID): %d\n", original_euid);
    printf("Saved UID (SUID): %d\n", original_suid);
    return 0;
}
