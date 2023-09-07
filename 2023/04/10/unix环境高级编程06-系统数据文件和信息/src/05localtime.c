#include <stdio.h>
#include <time.h>
#include <locale.h>

int main() {
    char buffer[80]; // 存储格式化后的字符串
    struct tm timeinfo;
    time_t rawtime;
    locale_t locale;
    // 获取当前时间
    time(&rawtime);
    localtime_r(&rawtime, &timeinfo);
    // 设置本地化环境为英文美国
    locale = newlocale(LC_TIME, "en_US.UTF-8", (locale_t)0);
    if (locale == (locale_t)0) {
        perror("设置本地化环境时出错");
        return 1;
    }
    // 使用 strftime_l 格式化时间和日期
    size_t len = strftime_l(buffer, sizeof(buffer), "%A, %B %d, %Y %I:%M%p", &timeinfo, locale);
    if (len > 0) {
        printf("格式化后的时间和日期：%s\n", buffer);
    } else {
        perror("格式化时间和日期时出错");
    }
    // 释放本地化环境
    freelocale(locale);
}  
