#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// 定义消息结构
struct message{
	long mtype;
	char mtext[100];
};

int main(){
	key_t key;
	int msgid;
	struct message msg;
	// 生成一个唯一键
	if((key = ftok("/tmp", 'A')) == -1){
		perror("fork");
		exit(1);
	}
	// 创建消息队列，如果队列已经存在则获取队列 ID
	if((msgid = msgget(key, 0666|IPC_CREAT)) == -1){
		perror("msgget error");
		exit(1);
	}
	// 准备消息
	msg.mtype = 1;
	strcpy(msg.mtext, "hello, this is a message frome msg_send!");
	// 发送消息
	if(msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1){
		perror("msgsnd error");
		exit(1);
	}
	printf("Message sent: %s\n", msg.mtext);
	return 0;
}
