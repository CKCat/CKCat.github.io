#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <grp.h>

int main() {
	int max_groups = 32;
	gid_t group_list[max_groups];
	int num_groups;
	num_groups = getgroups(max_groups, group_list);

	if (num_groups == -1) {
		perror("获取组ID列表时出错");
	} else {
		printf("当前进程所属的组ID列表：\n");
		for (int i = 0; i < num_groups; i++) {
			printf("%d\n", group_list[i]);
		}
	}
	gid_t group_list_[] = {1001, 1002}; // 要设置的附加组ID列表
	num_groups = sizeof(group_list) / sizeof(group_list[0]);

	if (setgroups(num_groups, group_list) == -1) {
		perror("设置附加组ID列表时出错");
	} else {
		printf("附加组ID列表已设置。\n");
	}

	const char *username = "ckcat"; // 要初始化附加组列表的用户名
	gid_t main_group = 1000;	   // 用户的主组ID

	if (initgroups(username, main_group) == -1) {
		perror("初始化附加组列表时出错");
	} else {
		printf("附加组列表已初始化。\n");
	}
	return 0;
}
