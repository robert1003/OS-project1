# OS project 1

## execute main program

Note that new system calls need to be added first.

```
cd [OS-project1_dir_path]
make
./main < [input_file_path]
```

## add new system call

### spec

I added three custom system call:
+ `asmlinkage long sys_my_print(char *s)`, id=333
+ `asmlinkage long sys_my_time(unsigned long *sec, unsigned long *nsec)`, id=334
+ `asmlinkage long sys_my_hello(void)`, id=335
The last system call is just for testing, and the `id` represent the system call id.

### step

```
# enter super user session
sudo -s

# install required packages
apt-get update
apt-get install wget gcc libncurses5-dev bison flex libssl-dev libelf-dev vim git
apt-get upgrade

# get kernel source 4.15.4
wget https://www.kernel.org/pub/linux/kernel/v4.x/linux-4.15.4.tar.xz

# extrace kernel source code and move our system call into it
sudo tar -xvf linux-4.15.4.tar.xz -C /usr/src/
cp -r [OS-project1_dir_path/kernel_files] /usr/src/linux-4.15.4/kernel_files
cd /usr/src/linux-4.15.4/

# edit linux-4.15.4/Makefile
# add kernel_files/ to the end of this line: core-y += kernel/ mm/ fs/ ipc/ security/ crypto/ block/

# edit linux-4.15.4/arch/x86/entry/syscalls/syscall_64.tbl
# add the following three lines
# 333     64      my_print          sys_my_print
# 334     64      my_time           sys_my_time
# 335     64      my_hello          sys_my_hello

# edit linux-4.15.4/include/linux/syscalls.h
# add the following three lines to the end of the files
# asmlinkage long sys_my_print(char *s);
# asmlinkage long sys_my_time(unsigned long *sec, unsigned long *nsec);
# asmlinkage long sys_my_hello(void);

# configure linux kernel
make menuconfig

# compile files
make -j[core_number]

# install kernel
make modules_install install

# reboot
shutdown -r now

# check kernel version
uname
```
