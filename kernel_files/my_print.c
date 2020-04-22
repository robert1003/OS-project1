#include <linux/kernel.h>
#include <linux/linkage.h>

asmlinkage long sys_my_print(char *s) {
    printk("%s\n", s);
    return 1;
}
