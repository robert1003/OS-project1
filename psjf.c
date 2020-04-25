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
#include <limits.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/resource.h>

#include "utils.h"
#include "psjf.h"

int cmp_psjf(const void *a, const void *b) {
    int x = ((Task *)a)->arrive_time, y = ((Task *)b)->arrive_time;
    int p = ((Task *)a)->remain_time, q = ((Task *)b)->remain_time;
    if(x == y) return p > q ? 1 : -1;
    return x > y ? 1 : -1;
}
void add_event_psjf(int (*event)[3], int event_cnt, int type, int value, int idx) {
    event[event_cnt][0] = type;
    event[event_cnt][1] = value;
    event[event_cnt][2] = idx;
}

void psjf(Task *task, int n) {
    // copy task
    Task task_c[N];
    for(int i = 0 ; i < n ; ++i) {
        memcpy(&task_c[i], &task[i], sizeof(task[i]));
    }
    // sort task
    qsort(task_c, n, sizeof(task_c[0]), cmp_psjf);

    int cnt = 0, nxt = n, now = task_c[0].arrive_time, event_cnt = 0, event[N][3], created[N];
    for(int i = 1 ; i < n ; ++i) {
        if(task_c[i].arrive_time != task_c[0].arrive_time) {
            nxt = i;
            break;
        }
    }
    task_c[n].arrive_time = INT_MAX;
    if(now != 0) add_event_psjf(event, event_cnt++, 0, now, -1);
    while(1) {
        int min_idx = -1, min_time = INT_MAX;
        for(int i = 0 ; i < n ; ++i) {
            if(task_c[i].remain_time > 0) {
                if(task_c[i].arrive_time < task_c[nxt].arrive_time) {
                    if(min_time > task_c[i].remain_time) {
                        min_time = task_c[i].remain_time;
                        min_idx = i;
                    }
                }
            }
            if(now == task_c[i].arrive_time && !created[i]) {
                add_event_psjf(event, event_cnt++, 1, -1, task_c[i].idx);
                created[i] = 1;
            }
        }
        if(min_idx == -1) {
            add_event_psjf(event, event_cnt++, 0, task_c[nxt].arrive_time - now, -1);
            for(int i = nxt + 1 ; i <= n ; ++i) {
                if(task_c[i].arrive_time != task_c[nxt].arrive_time) {
                    nxt = i;
                    break;
                }
            }
            continue;
        }
        if(now + min_time >= task_c[nxt].arrive_time) {
            min_time = task_c[nxt].arrive_time - now;
            for(int i = nxt + 1 ; i <= n ; ++i) {
                if(task_c[i].arrive_time != task_c[nxt].arrive_time) {
                    nxt = i;
                    break;
                }
            }
        }
        add_event_psjf(event, event_cnt++, 2, min_time, task_c[min_idx].idx);
        task_c[min_idx].remain_time -= min_time;
        now += min_time;
        if(task_c[min_idx].remain_time == 0) cnt += 1;
        if(cnt == n) break;
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
