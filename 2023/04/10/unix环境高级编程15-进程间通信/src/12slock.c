#include "12slock.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

struct slock *s_alloc(){
    struct slock *sp;
    static int cnt;

    if ((sp = malloc(sizeof(struct slock))) == NULL)
        return (NULL);
    do{
        // 根据进程 ID 和计数器来创建名字。
        snprintf(sp->name, sizeof(sp->name), "/%ld.%d", (long)getpid(), cnt++);
        sp->semp = sem_open(sp->name, O_CREAT | O_EXCL, S_IRWXU, 1);
        // 两个竞争的线程同时调用 s_alloc 并以同一个名字结束时，会导致 EEXIST 错误。
    } while ((sp->semp == SEM_FAILED) && (errno == EEXIST));

    if (sp->semp == SEM_FAILED){
        free(sp);
        return (NULL);
    }
    // 销毁名字，其他进程不能再次访问它。
    sem_unlink(sp->name);
    return (sp);
}

void s_free(struct slock *sp){
    sem_close(sp->semp);
    free(sp);
}

int s_lock(struct slock *sp){
    return (sem_wait(sp->semp));
}

int s_trylock(struct slock *sp){
    return (sem_trywait(sp->semp));
}

int s_unlock(struct slock *sp){
    return (sem_post(sp->semp));
}
