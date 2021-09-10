#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/types.h>

static void printprocessusingpid(unsigned long pid)
{
	pr_info("the pid is:%ld",pid);
}

static int __init kernel_address(void)
{
	struct task_struct *tsk;
	unsigned long pid;
	tsk = current;
	pid = tsk ->pid;
	printprocessusingpid(pid);
	tsk = tsk ->stack;
	printk("the unk address is:%x",current);
	printk("the none-offset-address is:%x",tsk);
}

static void __exit kernel_exit()
{
	pr_info("oh yeh!");
}
module_init(kernel_address);
module_exit(kernel_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YMH");
MODULE_DESCRIPTION("THIS IS THE ADDRESS OF THE KERNEL");
