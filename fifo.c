#define _GNU_SOURCE

#define N 10000

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
#include "fifo.h"

int cmp_fifo(const void *a, const void *b) {
    return ((Task *)a)->arrive_time > ((Task *)b)->arrive_time ? 1 : -1;
}
void add_event_fifo(int (*event)[3], int event_cnt, int type, int value, int idx) {
    event[event_cnt][0] = type;
    event[event_cnt][1] = value;
    event[event_cnt][2] = idx;
}

void fifo(Task *task, int n) {
    // copy task_c
    Task task_c[N];
    for(int i = 0 ; i < n ; ++i) {
        memcpy(&task_c[i], &task[i], sizeof(task[i]));
    }
    // sort task_c
    qsort(task_c, n, sizeof(task_c[0]), cmp_fifo);

    int t = 0, start = 0, event_cnt = 0, event[N][3], created[N] = {0};
    for(int i = 0 ; i < n ; ++i) {
        // add new task_c
        int wt = task_c[i].arrive_time - t;
        if(wt < 0) wt = 0;
        else if(wt == 0) {
	    if(i == 0) {
            	add_event_fifo(event, event_cnt++, 1, -1, task_c[i].idx);
            	created[task_c[i].idx] = 1;
	    }
        }
        else {
            add_event_fifo(event, event_cnt++, 0, wt, -1);
            add_event_fifo(event, event_cnt++, 1, -1, task_c[i].idx);
            created[task_c[i].idx] = 1;
        }
        // update start and t
        start = t + wt;
        t += wt + task_c[i].remain_time;
        int pre = 0;
        // calculate collision
        for(int j = i + 1 ; j < n ; ++j) {
            if(task_c[j].arrive_time <= t && task_c[j].arrive_time >= start) pre++;
            else break;
        }
        // update
        for(int j = i + 1 ; j < i + 1 + pre ; ++j) {
            if(created[task_c[j].idx] == 0) {
                int rt = task_c[j].arrive_time - start;
                start = task_c[j].arrive_time;
                if(rt != 0) {
                    add_event_fifo(event, event_cnt++, 2, rt, task_c[i].idx);
                    task_c[i].remain_time -= rt;
                }
                add_event_fifo(event, event_cnt++, 1, -1, task_c[j].idx);
                created[task_c[j].idx] = 1;
            }
        }
        // resume operation
        if(task_c[i].remain_time != 0) {
            add_event_fifo(event, event_cnt++, 2, task_c[i].remain_time, task_c[i].idx);
            task_c[i].remain_time = 0;
        }
    }
#ifdef DEBUG
    for(int i = 0 ; i < event_cnt ; ++i) {
        fprintf(stderr, "%d %d %d\n", event[i][0], event[i][1], event[i][2]);
    }
#endif
    for(int i = 0 ; i < event_cnt ; ++i) {
        int op = event[i][0], val = event[i][1], idx = event[i][2];
        if(op == 0) {
            for(int t = 0 ; t < val ; ++t) one_time_unit();
        }
        else if(op == 1) {
            create(&task[idx]);
        }
        else if(op == 2) {
            execute(&task[idx], val);
        }
        else assert(0 && "op not in 0, 1, 2");
    }
}
