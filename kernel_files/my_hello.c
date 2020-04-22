#include <linux/kernel.h>

asmlinkage long sys_my_hello(void) {
    printk("Hello world\n");
    return 0;
}
