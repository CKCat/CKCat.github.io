#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

void print_time(const time_t *timep, const char* type){
    struct tm atime_tm;
    localtime_r(timep, &atime_tm);
    char time_str[100];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &atime_tm);
    printf("%s 时间：%s\n", type, time_str);
}

int main(int argc, char* argv[]) {
    struct stat file_info;
    if (argc != 2){
        printf("usage: %s filename.\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // 使用 stat 函数获取文件信息
    if (stat(argv[1], &file_info) == -1) {
        perror("无法获取文件信息");
        exit(EXIT_FAILURE);
    }
    // 打印文件信息
    printf("st_dev: %lu\n", file_info.st_dev); // 设备ID，表示文件所在的设备。
    printf("st_ino: %lu\n", file_info.st_ino); // 文件的inode号，是文件在文件系统中的唯一标识。
    printf("st_mode: %o\n", file_info.st_mode & 0777); // 文件的类型和权限，使用掩码获取权限。
    printf("st_nlink: %lu\n", file_info.st_nlink); // 硬链接计数，表示指向该文件的硬链接数量。
    printf("st_uid: %d\n", file_info.st_uid);   // 文件的所有者的用户ID。
    printf("st_gid: %d\n", file_info.st_gid);  // 文件的所有者的组ID。
    printf("st_rdev: %lu\n", file_info.st_rdev); // 如果文件是特殊设备文件，则包含设备ID。
    printf("st_size: %lld byte\n", (long long)file_info.st_size); // 文件大小，以字节为单位。
    printf("st_blksize: %lu\n", file_info.st_blksize);  // 文件系统I/O缓冲区大小。
    printf("st_blocks: %ld\n", file_info.st_blocks);  // 文件所占的块数（以512字节为一块）。
    print_time(&file_info.st_atime, "st_atime");    // 最后访问时间。
    print_time(&file_info.st_mtime, "st_mtime");    // 最后修改时间。
    print_time(&file_info.st_ctime, "st_ctime");    // 最后状态变更时间。
    return 0;
}