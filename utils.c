#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sched.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/resource.h>

#include "utils.h"

int *_share_int = 0;

void one_time_unit() { volatile unsigned long i; for(i=0;i<1000000UL;i++); } 

void set_priority(Task *task, int priority) {
    struct sched_param param;
    param.sched_priority = priority;
    assert(sched_setparam(task->pid, &param) != -1 && "set priority failed");
}

void create(Task *task) {
    fflush(stdout);
    int pid = fork();
    assert(pid >= 0 && "utils create fork failed");
    task->pid = pid;
    if(pid == 0) {
        fflush(stdout);
        char tmp[105];
        sprintf(tmp, "%d", task->remain_time);
        execlp("./task", "./task", task->name, tmp, (char *)0);
    }
}

void execute(Task *task, int duration) {
    if(duration > task->remain_time) {
        puts("utils execute duration > task remain_time, exiting");
        return;
    }
    task->remain_time -= duration;
    *_share_int = duration;
    set_priority(task, 3);
}

void clear(Task *task) {
    waitpid(task->pid, NULL, 0);
}

