# 遇到的问题
[toc]
## 虚拟地址范围确定

- [x]  每个vma的起始和结束地址好像都不是对齐4KB的，这是什么原理？我记得VMA是由许多page组成的，也当然要求与page_size对齐；

![Untitled](%E9%81%87%E5%88%B0%E7%9A%84%E9%97%AE%E9%A2%98%20ede18ec20c16432fa15bb094dd4ebbfd/Untitled.png)

- 解决：上面是同学写的，不知道是怎么回事，先跳过
- 备注：他写的代码如下：

    ![Untitled](%E9%81%87%E5%88%B0%E7%9A%84%E9%97%AE%E9%A2%98%20ede18ec20c16432fa15bb094dd4ebbfd/Untitled%201.png)

- 测试方法：
    - 编写模块输出指定进程信息

        ```c
        // sclab.c
        #include <linux/module.h>     // 模块的结构定义和版本控制
        #include <linux/kernel.h>     // 常用内核函数
        #include <linux/init.h>       // 宏 __init和__exit
        #include <linux/sched.h>      // 进程描述符定义
        #include <linux/pid.h>        // pid相关操作函数

        MODULE_LICENSE("GPL");                                 // 模块许可证，这里设为GNU公共许可证
        MODULE_AUTHOR("huomax");                               // 模块作者
        MODULE_DESCRIPTION("A simple example Linux module.");  // 模块简单描述
        MODULE_VERSION("0.01");                                // 模块版本

        module_param(pid,int,S_IRUGO);                         // 模块传参，指定进程进程号
        static int __init sclab_init(void){

        	struct task_struct *sctask;
        	struct mm_struct *scmm;
        	struct vm_area_struct *scmmap;

        	/* 获取当前操作系统页表级数 */
        	printk("本机目前采用 %d 级页表",CONFIG_PGTABLE_LEVELS);

        	/* 获取指定PID的任务描述符 */
        	sctask = pid_task(find_get_pid(pid),PIDTYPE_PID);

        	/* 打印进程的每一个VMA的地址范围 */
        	scmm = sctask->mm;
        	scmmap = scmm->mmap;
        	while(scmmap != NULL){
        		printk("VMA_start:%lx	VMA_end:%lx",scmmap->vm_start,scmmap->vm_end);
        		scmmap = scmmap->vm_next;
        	}

        	printk("end input !\n");
        	return 0;
        }

        static void __exit sclab_exit(void){
        	printk("sclab mod uninstall!\n");
        }

        module_init(sclab_init);   // 注册模块初始化函数
        module_exit(sclab_exit);   // 注册模块注销函数

        // Makefile
        obj-m :=sclab.o # 使用目标文件sclab.o建立一个模块，模块名为sclab.ko
        all:
                make -C /lib/modules/$(shell uname -r)/build SUBDIRS=$(PWD) modules    # 编译模块
        clean: 
                make -C /lib/modules/$(shell uname -r)/build SUBDIRS=$(PWD) clean      # 清理模块
        ```

    - 输出并使用proc验证【这里通过内核配置选项CONFIG_PGTABLE_LEVELS查询当前页表结构】

        ```bash
        $ make
        $ insmod sclab.ko pid=5319
        $ dmesg
        [ 4292.391826] 本机目前采用 4 级页表
        [ 4292.391828] VMA_start:556005fb7000	VMA_end:556005fd0000
        [ 4292.391829] VMA_start:5560061cf000	VMA_end:5560061d0000
        [ 4292.391829] VMA_start:5560061d0000	VMA_end:5560061d1000
        [ 4292.391830] VMA_start:5560061d1000	VMA_end:5560061f9000
        [ 4292.391830] VMA_start:5560077a8000	VMA_end:55600782e000
        [ 4292.391830] VMA_start:7f826d423000	VMA_end:7f826d45e000
        [ 4292.391831] VMA_start:7f826d45e000	VMA_end:7f826d65d000
        [ 4292.391831] VMA_start:7f826d65d000	VMA_end:7f826d660000
        [ 4292.391831] VMA_start:7f826d660000	VMA_end:7f826d661000
        [ 4292.391832] VMA_start:7f826d661000	VMA_end:7f826d66c000
        [ 4292.391832] VMA_start:7f826d66c000	VMA_end:7f826d86b000
        [ 4292.391833] VMA_start:7f826d86b000	VMA_end:7f826d86c000
        [ 4292.391833] VMA_start:7f826d86c000	VMA_end:7f826d86d000
        [ 4292.391833] VMA_start:7f826d86d000	VMA_end:7f826d873000
        [ 4292.391834] VMA_start:7f826d873000	VMA_end:7f826d88a000
        [ 4292.391834] VMA_start:7f826d88a000	VMA_end:7f826da89000
        [ 4292.391834] VMA_start:7f826da89000	VMA_end:7f826da8a000
        [ 4292.391834] VMA_start:7f826da8a000	VMA_end:7f826da8b000
        [ 4292.391835] VMA_start:7f826da8b000	VMA_end:7f826da8d000
        ..............

        $ cat proc/5319/maps
        556005fb7000-556005fd0000 r-xp 00000000 08:01 1181214                    /usr/bin/top
        5560061cf000-5560061d0000 r--p 00018000 08:01 1181214                    /usr/bin/top
        5560061d0000-5560061d1000 rw-p 00019000 08:01 1181214                    /usr/bin/top
        5560061d1000-5560061f9000 rw-p 00000000 00:00 0 
        5560077a8000-55600782e000 rw-p 00000000 00:00 0                          [heap]
        7f826d423000-7f826d45e000 r-xp 00000000 08:01 268253                     /lib/x86_64-linux-gnu/libnss_systemd.so.2
        7f826d45e000-7f826d65d000 ---p 0003b000 08:01 268253                     /lib/x86_64-linux-gnu/libnss_systemd.so.2
        7f826d65d000-7f826d660000 r--p 0003a000 08:01 268253                     /lib/x86_64-linux-gnu/libnss_systemd.so.2
        7f826d660000-7f826d661000 rw-p 0003d000 08:01 268253                     /lib/x86_64-linux-gnu/libnss_systemd.so.2
        7f826d661000-7f826d66c000 r-xp 00000000 08:01 275459                     /lib/x86_64-linux-gnu/libnss_files-2.27.so
        7f826d66c000-7f826d86b000 ---p 0000b000 08:01 275459                     /lib/x86_64-linux-gnu/libnss_files-2.27.so
        7f826d86b000-7f826d86c000 r--p 0000a000 08:01 275459                     /lib/x86_64-linux-gnu/libnss_files-2.27.so
        7f826d86c000-7f826d86d000 rw-p 0000b000 08:01 275459                     /lib/x86_64-linux-gnu/libnss_files-2.27.so
        7f826d86d000-7f826d873000 rw-p 00000000 00:00 0 
        7f826d873000-7f826d88a000 r-xp 00000000 08:01 275456                     /lib/x86_64-linux-gnu/libnsl-2.27.so
        7f826d88a000-7f826da89000 ---p 00017000 08:01 275456                     /lib/x86_64-linux-gnu/libnsl-2.27.so
        7f826da89000-7f826da8a000 r--p 00016000 08:01 275456                     /lib/x86_64-linux-gnu/libnsl-2.27.so
        7f826da8a000-7f826da8b000 rw-p 00017000 08:01 275456                     /lib/x86_64-linux-gnu/libnsl-2.27.so
        7f826da8b000-7f826da8d000 rw-p 00000000 00:00 0 
        7f826da8d000-7f826da98000 r-xp 00000000 08:01 275461                     /lib/x86_64-linux-gnu/libnss_nis-2.27.so
        7f826da98000-7f826dc97000 ---p 0000b000 08:01 275461                     /lib/x86_64-linux-gnu/libnss_nis-2.27.so
        7f826dc97000-7f826dc98000 r--p 0000a000 08:01 275461                     /lib/x86_64-linux-gnu/libnss_nis-2.27.so
        7f826dc98000-7f826dc99000 rw-p 0000b000 08:01 275461                     /lib/x86_64-linux-gnu/libnss_nis-2.27.so
        7f826dc99000-7f826dca1000 r-xp 00000000 08:01 275457                     /lib/x86_64-linux-gnu/libnss_compat-2.27.so
        7f826dca1000-7f826dea1000 ---p 00008000 08:01 275457                     /lib/x86_64-linux-gnu/libnss_compat-2.27.so
        7f826dea1000-7f826dea2000 r--p 00008000 08:01 275457                     /lib/x86_64-linux-gnu/libnss_compat-2.27.so
        ..............
        ```

## 页表机制确定

- [x]  怎么知道当前的操作系统采用了什么页表机制：
- 先查询当前操作系统体系

```bash
$ uname -a
Linux huomax-virtual-machine 4.17.12+ #1 SMP Fri Aug 27 14:13:14 CST 2021 x86_64 x86_64 x86_64 GNU/Linux
```

- 可以看出当前为x86_64 体系，查看linux4.17.12源码/arch/x86/...，查找页表级数定义
- 但是我在看别人的源码解析时提到：若当前为4级页表，则p4d_offset返回pgd，所以这里的查看页表机制没在代码里面用上

```c
/* 
可以发现页表级数由 CONFIG_PGTABLE_LEVELS 定义
但未找到 CONFIG_PGTABLE_LEVELS 最初定义
*/

// arch\x86\entry\vdso\vdso32\vclock_gettime.c

#ifdef CONFIG_X86_64
/*
 * in case of a 32 bit VDSO for a 64 bit kernel fake a 32 bit kernel
 * configuration
 */
#undef CONFIG_64BIT
#undef CONFIG_X86_64
#undef CONFIG_PGTABLE_LEVELS
#undef CONFIG_ILLEGAL_POINTER_VALUE
#undef CONFIG_SPARSEMEM_VMEMMAP
#undef CONFIG_NR_CPUS

#define CONFIG_X86_32 1
#define CONFIG_PGTABLE_LEVELS 2
#define CONFIG_PAGE_OFFSET 0
#define CONFIG_ILLEGAL_POINTER_VALUE 0
#define CONFIG_NR_CPUS 1

#define BUILD_VDSO32_64

#endif

/* 
以上似乎并不是 CONFIG_PGTABLE_LEVELS 的定义 , 是否是由硬件、或者其他什么定义？
于是查找内核配置选项
未查到 CONFIG_PGTABLE_LEVELS，只查到 PGTABLE_LEVELS
emmm。。。是不是在这里配置的选项在被外界引用都要加个CONFIG_前缀？还没开始查，先默认是
// arch/x86/Kconfig

config PGTABLE_LEVELS
	int
	default 5 if X86_5LEVEL
	default 4 if X86_64
	default 3 if X86_PAE
	default 2
*/
```

## 明确需要查找的目标

- 实验要求：

![Untitled](%E9%81%87%E5%88%B0%E7%9A%84%E9%97%AE%E9%A2%98%20ede18ec20c16432fa15bb094dd4ebbfd/Untitled%202.png)

- [x]  我语文不好，这里是指？
    - [x]  当前进程的所有已分配的【虚拟内存对应的物理内存页框号】？
    - [x]  当前进程的所有【已分配的虚拟内存】对应的物理页框号？

```c
/*
每个进程的物理页框都是有限的，有些虚地址都没建立映射。那这就不可能是情况2，因为我不能也不好去
影响当前进程(正在在CPU上运行的进程)啊
总结：我傻逼了。
*/
```