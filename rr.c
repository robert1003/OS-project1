#define _GNU_SOURCE

#define N 10000
#define TQ 500

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
#include "rr.h"

int cmp_rr(const void *a, const void *b) {
    int x = ((Task *)a)->arrive_time, y = ((Task *)b)->arrive_time;
    int p = ((Task *)a)->idx, q = ((Task *)b)->idx;
    if(x == y) return p > q ? 1 : -1;
    return x > y ? 1 : -1;
}
void add_event_rr(int (*event)[3], int event_cnt, int type, int value, int idx) {
    event[event_cnt][0] = type;
    event[event_cnt][1] = value;
    event[event_cnt][2] = idx;
}

void rr(Task *task, int n) {
    // copy task
    Task task_c[N];
    for(int i = 0 ; i < n ; ++i) {
        memcpy(&task_c[i], &task[i], sizeof(task[i]));
    }
    // sort task
    qsort(task_c, n, sizeof(task_c[0]), cmp_rr);

    int id[N], t = 0, pt = 0, lef = 0, rig = n - 1, cnt = 0, event_cnt = 0, event[N][3];
    while(pt != n || cnt) {
        if(!cnt) {
            int tt = task_c[pt].arrive_time - t;
            t = task_c[pt].arrive_time;
            add_event_rr(event, event_cnt++, 0, tt, -1);
            add_event_rr(event, event_cnt++, 1, -1, task_c[pt].idx);
            rig++; if(rig == n) rig = 0;
            id[rig] = pt;
            pt++;
            cnt++;
        }
        int p = id[lef], la = task_c[p].remain_time;
        if(la > TQ) la = TQ;
        while(pt != n && t + la >= task_c[pt].arrive_time) {
            int used = task_c[pt].arrive_time - t;
            if(used) {
                add_event_rr(event, event_cnt++, 2, used, task_c[p].idx);
                task_c[p].remain_time -= used;
                la -= used;
                t += used;
            }
            add_event_rr(event, event_cnt++, 1, -1, task_c[pt].idx);
            rig++; if(rig == n) rig = 0;
            id[rig] = pt;
            pt++;
            cnt++;
        }
        if(la) {
            add_event_rr(event, event_cnt++, 2, la, task_c[p].idx);
            task_c[p].remain_time -= la;
            t += la;
        }
        lef++; if(lef == n) lef = 0;
        cnt--;
        if(task_c[p].remain_time) {
            rig++; if(rig == n) rig = 0;
            id[rig] = p;
            cnt++;
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

