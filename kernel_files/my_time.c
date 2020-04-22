#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/module.h>

asmlinkage long sys_my_time(unsigned long *sec, unsigned long *nsec) {
    struct timespec now;
    getnstimeofday(&now);
    *sec = now.tv_sec;
    *nsec = now.tv_nsec;
    return 1;
}

