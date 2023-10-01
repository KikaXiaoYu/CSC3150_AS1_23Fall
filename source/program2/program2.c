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
static struct task_struct *task;

struct wait_opts {
	enum pid_type wo_type;
	int wo_flags;
	struct pid *wo_pid;

	struct waitid_info *wo_info;
	int wo_stat;
	struct rusage *wo_rusage;

	wait_queue_entry_t child_wait;
	int notask_error;
};
extern pid_t kernel_clone(struct kernel_clone_args *args);
extern int do_execve(struct filename *filename,
		     const char __user *const __user *__argv,
		     const char __user *const __user *__envp);
extern long do_wait(struct wait_opts *wo);
extern struct filename *getname_kernel(const char *filename);

int my_exec(void)
{
	int retval;

	// the path of the test file
	const char path[] =
		"/home/vagrant/csc3150/source/program2/test"; // /tmp/test
	struct filename *my_filename = getname_kernel(path);

	printk("[program2] : child process");
	retval = do_execve(my_filename, NULL, NULL);

	// success
	if (!retval)
		return 0;
	do_exit(retval); // fail
}

long my_wait(pid_t pid)
{
	int status;
	int retval;
	struct wait_opts wo;
	enum pid_type type;
	type = PIDTYPE_PID;
	wo.wo_type = type;

	// wo.wo_flags
	wo.wo_flags = WEXITED | WUNTRACED;

	// wo.wo_pid
	struct pid *wo_pid = NULL;
	wo_pid = find_get_pid(pid);
	wo.wo_pid = wo_pid;

	// wo.wo_info
	wo.wo_info = NULL;

	// wo.wo_stat
	wo.wo_stat = status;

	// wo.wo_rusage
	wo.wo_rusage = NULL;

	do_wait(&wo);
	retval = wo.wo_stat;

	// decrease the count and free memory
	put_pid(wo_pid);
	return retval;
}

// implement fork function
int my_fork(void *argc)
{
	// set default sigaction for current process
	pid_t pid;
	int status;
	int i;
	struct k_sigaction *k_action = &current->sighand->action[0];
	for (i = 0; i < _NSIG; i++) {
		k_action->sa.sa_handler = SIG_DFL;
		k_action->sa.sa_flags = 0;
		k_action->sa.sa_restorer = NULL;
		sigemptyset(&k_action->sa.sa_mask);
		k_action++;
	}

	/* fork a process using kernel_clone or kernel_thread */
	struct kernel_clone_args args = {
		.flags = ((SIGCHLD | CLONE_VM | CLONE_UNTRACED) & ~CSIGNAL),
		.pidfd = NULL,
		.parent_tid = NULL,
		.child_tid = NULL,
		.exit_signal = (SIGCHLD & CSIGNAL),
		.stack = (unsigned long)&my_exec,
		.stack_size = 0,
		.tls = 0,
	};
	pid = kernel_clone(&args);
	printk("[program2] : The child process has pid = %d\n", pid);
	printk("[program2] : This is the parent process, pid = %d\n",
	       (int)current->pid);

	/* execute a test program in child process */
	// do this in my_execve function
	/* wait until child process terminates */
	status = my_wait(pid);
	// printk("[program2] : The return status is %d\n", status);
	int sign = status & 0x7f; // WTERMSIG
	int stop = status & 0xff; // WIFSTOPPED

	/* check child process'  termination status */
	if (sign == 0) {
		printk("[program2] : child process terminated\n");
		printk("[program2] : The return signal is %d", sign);
	}
	// child process stopped
	else if (stop == 0x7f) // WIFSTOPPED
	{
		int stop_sign = (status & 0xff00) >> 8; // WSTOPSIG, WEXITSTATUS
		if (stop_sign == SIGSTOP) {
			printk("[program2] : get SIGSTOP signal\n");
		} else {
			printk("[program2] : get a stop signal which is not supported\n");
		}
		printk("[program2] : The return signal is %d", stop_sign);
	}
	// child execution failed
	else if (((signed char)((sign + 1) >> 1)) > 0) // WIFSIGNALED
	{
		switch (sign) {
		case 6:
			printk("[program2] : get SIGABRT signal\n");
			printk("[program2] : child process is terminated by a abort call\n");
			break;
		case 14:
			printk("[program2] : get SIGALRM signal\n");
			printk("[program2] : child process is terminated by a alarm call\n");
			break;
		case 7:
			printk("[program2] : get SIGBUS signal\n");
			printk("[program2] : child process is terminated by a bus call\n");
			break;
		case 8:
			printk("[program2] : get SIGFPE signal\n");
			printk("[program2] : child process is terminated by a floating call\n");
			break;
		case 1:
			printk("[program2] : get SIGHUP signal\n");
			printk("[program2] : child process is terminated by a hangup call\n");
			break;
		case 4:
			printk("[program2] : get SIGILL signal\n");
			printk("[program2] : child process is terminated by a illegal_instr call\n");
			break;
		case 2:
			printk("[program2] : get SIGINT signal\n");
			printk("[program2] : child process is terminated by a interrupt call\n");
			break;
		case 9:
			printk("[program2] : get SIGKILL signal\n");
			printk("[program2] : child process is terminated by a kill call\n");
			break;
		case 13:
			printk("[program2] : get SIGPIPE signal\n");
			printk("[program2] : child process is terminated by a pipe call\n");
			break;
		case 3:
			printk("[program2] : get SIGQUIT signal\n");
			printk("[program2] : child process is terminated by a quit call\n");
			break;
		case 11:
			printk("[program2] : get SIGSEGV signal\n");
			printk("[program2] : child process is terminated by a segment_fault call\n");
			break;
		case 15:
			printk("[program2] : get SIGTERM signal\n");
			printk("[program2] : child process is terminated by a terminate call\n");
			break;
		case 5:
			printk("[program2] : get SIGTRAP signal\n");
			printk("[program2] : child process is terminated by a trap call\n");
			break;
		default:
			printk("[program2] : get a signal which is not supported\n");
			break;
		}
		printk("[program2] : The return signal is %d\n", sign);
	}
	// continued
	else {
		printk("[program2] : child process continued\n");
	}

	switch (status)
	{
	case 0:
		printk("[program2] : Normal termination\n");
		break;
	case 134:
		printk("[program2] : get SIGABRT signal\n");
		printk("[program2] : child process terminated\n");
		break;
	case 14:
		printk("[program2] : child process get SIGALRM signal\n");
		printk("[program2] : child process terminated\n");
		break;
	case 135:
		printk("[program2] : child process get SIGBUS signal\n");
		printk("[program2] : child process terminated\n");
		break;
	case 136:
		printk("[program2] : child process get SIGFPE signal\n");
		printk("[program2] : child process terminated\n");
		break;
	case 1:
		printk("[program2] : child process get SIGHUP signal\n");
		printk("[program2] : child process terminated\n");
		break;
	case 132:
		printk("[program2] : child process get SIGILL signal\n");
		printk("[program2] : child process terminated\n");
		break;
	case 2:
		printk("[program2] : child process get SIGINT signal\n");
		printk("[program2] : child process terminated\n");
		break;
	case 9:
		printk("[program2] : child process get SIGKILL signal\n");
		printk("[program2] : child process terminated\n");
		break;
	case 13:
		printk("[program2] : child process get SIGPIPE signal\n");
		printk("[program2] : child process terminated\n");
		break;
	case 131:
		printk("[program2] : child process get SIGQUIT signal\n");
		printk("[program2] : child process terminated\n");
		break;
	case 139:
		printk("[program2] : child process get SIGSEGV signal\n");
		printk("[program2] : child process terminated\n");
		break;
	case 15:
		printk("[program2] : child process get SIGTERM signal\n");
		printk("[program2] : child process terminated\n");
		break;
	case 133:
		printk("[program2] : child process get SIGTRAP signal\n");
		printk("[program2] : child process terminated\n");
		break;
	case 4991:
		printk("[program2] : child process get SIGSTOP signal\n");
		printk("[program2] : child process stoped\n");
		break;
	default:
		printk("[program2] : child process get the signal with status = %d\n", status);
		printk("[program2] : child process terminated\n");
		break;
	}
	printk("[program2] : the return signal is %d\n", status);

	do_exit(0);
	return 0;
}

static int __init program2_init(void)
{
	printk("[program2] : module_init Peng Qiaoyu 120090175\n");

	/* write your code here */

	/* create a kernel thread to run my_fork */
	printk("[program2] : module_init create kthread start\n");
	task = kthread_create(&my_fork, NULL, "MyThread");

	if (!IS_ERR(task)) {
		printk("[program2] : module_init kthread start\n");
		wake_up_process(task);
	}

	return 0;
}

static void __exit program2_exit(void)
{
	printk("[program2] : Module_exit\n");
}

module_init(program2_init);
module_exit(program2_exit);
