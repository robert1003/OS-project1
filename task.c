#define _GNU_SOURCE

#define SHM_NAME "shm_name"
#define SHM_FLAG O_RDWR
#define SHM_MODE S_IRWXU
#define MMAP_FLAG PROT_READ | PROT_WRITE
#define MMAP_MODE MAP_SHARED
#define SYSCALL_PRINT_ID 333
#define SYSCALL_TIME_ID 334

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

typedef struct my_time {
    unsigned long sec, nsec;
} time;

int main(int argc, char *argv[]) {
    /* setup environment */
    // share memory
    int fd = shm_open(SHM_NAME, SHM_FLAG, SHM_MODE);
    assert(fd >= 0 && "main shm failed");
    int *_share_int = (int *)mmap(NULL, sizeof(int), MMAP_FLAG, MMAP_MODE, fd, 0);

    /* start */
    int total = atoi(argv[2]);
    time st, ed;
    assert(syscall(SYSCALL_TIME_ID, &st.sec, &st.nsec) != -1 && "task get start time failed");
    while(total > 0) {
        total -= *_share_int;
        for(int t = 0 ; t < *_share_int ; ++t) one_time_unit();
        if(total == 0) {
            assert(syscall(SYSCALL_TIME_ID, &ed.sec, &ed.nsec) != -1 && "task get end time failed");
            char mes[505];
            sprintf(mes, "[Project1] %d %lu.%09lu, %lu.%09lu", getpid(), st.sec, st.nsec, ed.sec, ed.nsec);
            assert(syscall(SYSCALL_PRINT_ID, mes) != -1 && "task print message failed");
        }
        /* reset priority, give control back to main */
        struct sched_param param;
        param.sched_priority = 1;
        assert(sched_setparam(0, &param) >= 0 && "child change priority failed");
        sched_yield();
    }
}
