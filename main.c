#define _GNU_SOURCE

#define N 10000
#define SHM_NAME "shm_name"
#define SHM_FLAG O_CREAT | O_RDWR
#define SHM_MODE S_IRWXU
#define MMAP_FLAG PROT_READ | PROT_WRITE
#define MMAP_MODE MAP_SHARED

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
#include "sjf.h"
#include "psjf.h"
#include "rr.h"

int main() {
    /* setup environment */
    // share memory
    int fd = shm_open(SHM_NAME, SHM_FLAG, SHM_MODE);
    assert(fd >= 0 && "main shm failed");
    ftruncate(fd, sizeof(int));
    _share_int = (int *)mmap(NULL, sizeof(int), MMAP_FLAG, MMAP_MODE, fd, 0);
    *_share_int = 0;
    // cpu setup
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set); CPU_SET(0, &cpu_set);
    assert(sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set) != -1 && "main set visible cpu failed");
    // priority & scheduler setup
    struct sched_param param;
    param.sched_priority = 2;
    assert(sched_setscheduler(0, SCHED_FIFO, &param) != -1 && "main set scheduler type failed");
    assert(sched_setparam(0, &param) != -1 && "main set priority failed");

    /* read input */
    int n;
    char op[20];
    Task task[N];
    scanf("%s %d", op, &n);
    for(int i = 0 ; i < n ; ++i) {
        scanf("%s %d %d", task[i].name, &task[i].arrive_time, &task[i].remain_time);
        task[i].idx = i;
    }
    
    /* run! */
    if(op[0] == 'R') rr(task, n);
    if(op[0] == 'F') fifo(task, n);
    if(op[0] == 'S') sjf(task, n);
    if(op[0] == 'P') psjf(task, n);

    /* print result */
    for(int i = 0 ; i < n ; ++i) {
        clear(&task[i]);
        assert(task[i].remain_time == 0 && "task not done");
        printf("%s %d\n", task[i].name, task[i].pid);
        fflush(stdout);
    }

    /* release shared memory */
    shm_unlink(SHM_NAME);
}

