#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	pid_t pid;
	int status;
	int the_signal;

	/* fork a child process */
	printf("Process start to fork\n");
	pid = fork();

	if (pid == -1)
	{
		perror("error");
		exit(1);
	}
	else
	{
		/* child process */
		if (pid == 0)
		{
			int i;
			char *arg[argc];

			for (i = 0; i < argc - 1; i++)
			{
				arg[i] = argv[i + 1];
			}
			arg[argc - 1] = NULL;
			/* get the test program path and name */

			printf("I'm the Child Process, my pid = %d\n", getpid());
			printf("Child process start to execute test program:\n");

			/* execute test program */
			execve(arg[0], arg, NULL);
		}
		/* parent process */
		else
		{
			printf("I'm the Parent Process, my pid = %d\n", getpid());

			/* wait for child process terminates */
			waitpid(pid, &status, WUNTRACED);
			printf("Parent process receives SIGCHLD signal\n");

			/* check child process' termination status and signal */
			int signal = status & 0x7F; // get signal from lowest 7 bits

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
				printf("Normal termination with EXIT STATUS = 0\n");
				break;
			case 127:
				printf("child process get stop signal\n");
				break;
			default:
				printf("child process get %s signal\n", signal_array[signal]);
				break;
			}

			if (signal == 17 || signal == 19 || signal == 127)
			{
				printf("child process stopped\n");
			}
			else if (signal != 0)
			{
				printf("child process terminated by %s signal with signal = %d\n", signal_array[signal], signal);
			}
		}
	}
}
