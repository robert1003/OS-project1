#ifndef UTILS_H
#define UTILS_H

extern int *_share_int;

typedef struct process {
    char name[40];
    int arrive_time, remain_time, pid, idx;
} Task;

void one_time_unit();
void set_priority(Task *task, int priority);
void create(Task *task);
void execute(Task *task, int duration);
void clear(Task *task);

#endif
