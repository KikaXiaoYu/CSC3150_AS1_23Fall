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

	struct waitid_info *wo_info;
	int wo_stat;
	struct rusage *wo_rusage;

	wait_queue_entry_t child_wait;
};

/* exported functions from kernel */
extern pid_t kernel_clone(struct kernel_clone_args *args);

extern int do_execve(struct filename *filename,
					 const char __user *const __user *__argv,
					 const char __user *const __user *__envp);

extern struct filename *getname_kernel(const char *filename);

extern long do_wait(struct wait_opts *wo);

// to texecute the test program
int my_execve(void)
{
	int exe_res;
	const char path[] = "/tmp/test";;

	struct filename *file_name = getname_kernel(path);

	/* execute a test program in child process */
	exe_res = do_execve(file_name, NULL, NULL);

	// return 0;

	if (exe_res == 0) // execute properly
	{
		return 0;
	}
	do_exit(exe_res); // forcely exit process if not proper
}

// implement fork function
int my_fork(void *argc)
{
	int pid;

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

	/* for kernel_clone parameter usage */
	struct kernel_clone_args clone_args = {
		.flags = ((lower_32_bits(SIGCHLD) | CLONE_VM | CLONE_UNTRACED) & ~CSIGNAL),
		// A series of flag bits can be set through the flags parameter.
		// For example, CLONE_VM indicates the shared process address space,
		// CLONE_UNTRACED indicates that there is no need to track the child process,
		// and CSIGNAL indicates the signal when the child process exits.
		.pidfd = NULL,		// pidfd is a flag used to indicate whether to create a pidfd file descriptor when cloning a new process
		.child_tid = NULL,	// used for clone() to point to user space mem. in child process address space
		.parent_tid = NULL, // usef for clone() to point to user space mem. in parent process address
		.exit_signal = (lower_32_bits(SIGCHLD) & CSIGNAL),
		.stack = (unsigned long)&my_execve, // the location of the functio to execute
		.stack_size = 0,					// normally set as 0 because it is unused
		.tls = 0
		// tls stands for Thread Local Storage, which is a
		// thread-private memory area used to store thread-specific variables.
		/* other paramters or struct no need to set */
	};

	/* fork a process using kernel_clone or kernel_thread */
	pid = kernel_clone(&clone_args);
	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n", (int)current->pid);

	/* wait_opts paramters */
	int status;

	/* wait_opts settings */
	struct wait_opts do_wo = {
		.wo_type = PIDTYPE_PID,
		.wo_flags = WEXITED | WUNTRACED,
		.wo_pid = find_get_pid(pid),
		.wo_info = NULL,
		.wo_stat = status,
		.wo_rusage = NULL,
	};

	/* wait until child process terminates */
	printk("[program2] : child process\n");
	do_wait(&do_wo);

	status = do_wo.wo_stat; // get the status

	int signal = status & 0x7f; // to do WTERMSIG

	/* output signal information */
	char signal_array[32][15] = {
		"normal", "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL",
		"SIGTRAP", "SIGABRT", "SIGBUS", "SIGFPE", "SIGKILL",
		"SIGUSR1", "SIGSEGV", "SIGUSR2", "SIGPIPE", "SIGALRM",
		"SIGTERM", "unused", "SIGCHLD", "SIGCONT", "SIGSTOP",
		"SIGTSTP", "SIGTTIN", "SIGTTOU", "SIGURG", "SIGXCPU",
		"SIGXFSG", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGIO",
		"SIGPWR", "SIGSYS"};

	switch (signal)
	{
	case 0:
		printk("[program2] : Normal termination with EXIT STATUS = 0\n");
		break;
	case 127:
		printk("[program2] : child process get stop signal\n");
		break;
	default:
		printk("[program2] : get %s signal\n", signal_array[signal]);
		break;
	}

	// /* stopped or terminated */
	if (signal == 17 || signal == 19 || signal == 127)
	{
		printk("[program2] : child process stopped\n");
	}
	else if (signal != 0)
	{
		printk("[program2] : child process terminated\n");
	}

	printk("[program2] : The return signal is %d\n", signal);

	return 0;
	do_exit(0);
}

static int __init program2_init(void)
{

	printk("[program2] : module_init\n");

	// /* create a kernel thread to run my_fork */
	printk("[program2] : module_init create kthread start\n");
	task = kthread_create(&my_fork, NULL, "MyThread");

	/* wake up new thread if ok */
	if (!IS_ERR(task))
	{
		printk("[program2] : module_init kthread start\n");
		wake_up_process(task);
	}

	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
