#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>

void TELL_WAIT();
void TELL_PARENT(pid_t pid);
void WAIT_PARENT();
void TELL_CHILD(pid_t pid);
void WAIT_CHILD();
