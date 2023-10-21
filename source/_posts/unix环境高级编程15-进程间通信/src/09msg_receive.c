#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// 定义消息结构
struct message {
    long mtype;       // 消息类型
    char mtext[100];  // 消息内容
};

int main(){
	key_t key;
	int msgid;
	struct message msg;
	// 生成一个唯一的键
	if((key = ftok("/tmp", 'A')) == -1){
		perror("ftok error");
		exit(1);
	}
	// 获取消息队列 ID
	if((msgid = msgget(key, 0666)) == -1){
		perror("msgget error");
		exit(1);
	}
	// 接收消息
	if(msgrcv(msgid, &msg, sizeof(msg.mtext), 1, MSG_NOERROR) == -1){
		perror("msgrcv");
		exit(1);
	}
	printf("Received message: %s\n", msg.mtext);
	return 0;
}
