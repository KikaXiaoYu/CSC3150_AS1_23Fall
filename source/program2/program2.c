#include <linux/module.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/kthread.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <linux/jiffies.h>
#include <linux/kmod.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

/* for kthread_create */
static struct task_struct *task;


/* for do_wait parameters usage */
struct wait_opts
{
	enum pid_type wo_type;
	int wo_flags;
	struct pid *wo_pid;
	struct siginfo __user *wo_info;
	int __user *wo_stat;
	struct rusage __user *wo_rusage;
	wait_queue_t child_wait;
	int notask_error;
};

/* for kernel_clone parameters usage */
struct kernel_clone_args
{
	u64 flags;
	int __user *pidfd;
	int __user *child_tid;
	int __user *parent_tid;
	int exit_signal;
	unsigned long stack;
	unsigned long stack_size;
	unsigned long tls;
	pid_t *set_tid;
	/* Number of elements in *set_tid */
	size_t set_tid_size;
	int cgroup;
	int io_thread;
	struct cgroup *cgrp;
	struct css_set *cset;
};

extern pid_t kernel_clone(struct kernel_clone_args *args);

extern int do_execve(struct filename *filename,
					 const char __user *const __user *__argv,
					 const char __user *const __user *__envp);

extern struct filename *getname(const char __user *filename);

extern long do_wait(struct wait_opts *wo);

// implement fork function
int my_fork(void *argc)
{

	// set default sigaction for current process
	int i;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for (i = 0; i < _NSIG; i++)
	{
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}

	/* fork a process using kernel_clone or kernel_thread */

	/* execute a test program in child process */

	/* wait until child process terminates */

	return 0;
}

static int __init program2_init(void)
{

	printk("[program2] : module_init\n");

	/* create a kernel thread to run my_fork */
	printk("[program2] : module_init create kthread start");
	task = kthread_create();

	/* wake up new thread if ok */
	if ()


	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
