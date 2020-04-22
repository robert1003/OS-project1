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

void rr(Task *task, int n) {
    // copy task
    Task task_c[N];
    for(int i = 0 ; i < n ; ++i) {
        memcpy(&task_c[i], &task[i], sizeof(task[i]));
    }
    // sort task
    qsort(task_c, n, sizeof(task_c[0]), cmp_rr);

    int id[N], t = 0, pt = 0, lef = 0, rig = n - 1, cnt = 0;
	while(pt != n || cnt) {
		if(!cnt) {
			int tt = task_c[pt].arrive_time - t;
			t = task_c[pt].arrive_time;
			for(int i = 0; i < tt; i++) one_time_unit();
			create(&task[task_c[pt].idx]);
			rig++; if(rig == n) rig = 0;
			id[rig] = pt;
			pt++;
			cnt++;
		}
		int p = id[lef], la = task[task_c[p].idx].remain_time;
		if(la > TQ) la = TQ;
		while(pt != n && t + la >= task[task_c[pt].idx].arrive_time) {
			int used = task[task_c[pt].idx].arrive_time - t;
			if(used) {
				execute(&task[task_c[p].idx], used);
				la -= used;
				t += used;
			}
			create(&task[task_c[pt].idx]);
			rig++; if(rig == n) rig = 0;
			id[rig] = pt;
			pt++;
			cnt++;
		}
		if(la) { 
			execute(&task[task_c[p].idx], la);
			t += la;
		}
		lef++; if(lef == n) lef = 0;
		cnt--;
		if(task[task_c[p].idx].remain_time) {
			rig++; if(rig == n) rig = 0;
			id[rig] = p;
			cnt++;
		}
	}
}
