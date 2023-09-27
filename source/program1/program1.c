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

			/* check child process'  termination status */
			switch (status)
			{
			case 0:
				printf("Normal termination with EXIT STATUS = 0\n");
				break;
			case 134:
				printf("child process get SIGABRT signal\n");
				break;
			case 14:
				printf("child process get SIGALRM signal\n");
				break;
			case 135:
				printf("child process get SIGBUS signal\n");
				break;
			case 136:
				printf("child process get SIGFPE signal\n");
				break;
			case 1:
				printf("child process get SIGHUP signal\n");
				break;
			case 132:
				printf("child process get SIGILL signal\n");
				break;
			case 2:
				printf("child process get SIGINT signal\n");
				break;
			case 9:
				printf("child process get SIGKILL signal\n");
				break;
			case 13:
				printf("child process get SIGPIPE signal\n");
				break;
			case 131:
				printf("child process get SIGQUIT signal\n");
				break;
			case 139:
				printf("child process get SIGSEGV signal\n");
				break;
			case 15:
				printf("child process get SIGTERM signal\n");
				break;
			case 133:
				printf("child process get SIGTRAP signal\n");
				break;
			case 4991:
				printf("child process get SIGSTOP signal\n");
				break;
			default:
				printf("child process get the signal with status = %d\n", status);
				break;
			}
		}
	}
}
