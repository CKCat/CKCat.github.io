#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

int system(const char *cmdstring){
    pid_t pid;
    int status;
    if(cmdstring == NULL)
        return (-1);
    if((pid = fork()) < 0)
        status = -1;
    else if(pid == 0){
        // shell的-c选项告诉shell程序取下一个命令行参数作为命令输入
        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        // _exit 防止任一标准I/O缓冲在子进程中被冲洗。
        _exit(127);
    }else{
        while (waitpid(pid, &status, 0) < 0){
            if(errno != EINTR){
                status = -1;
                break;
            }
        }
    }
    return (status);
}

void pr_exit(int status)
{

    if (WIFEXITED(status))
        printf("normal termination , exut status = %d\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("normal termination , exut status = %d%s\n", WTERMSIG(status),
#ifdef WCOREDUMP                                            
               WCOREDUMP(status) ? " (core file generated)" : "");
#else
        "");
#endif
    else if (WIFSTOPPED(status))
        printf("child stopped, signal number = %d\n", WSTOPSIG(status));
}

int main(int argc, char const *argv[])
{
    int status;
    if((status = system("date")) < 0){
        perror("system(date) error");
        return 1;
    }   
    pr_exit(status);
    if((status = system("nosuchcommand")) < 0){
        perror("system(nosuchcommand) error");
        return 1;
    }
    pr_exit(status);
    if ((status = system("who; exit 44")) < 0){
        perror("system(who; exit 44) error");
        return 1;
    }
    pr_exit(status);
    return 0;
}
