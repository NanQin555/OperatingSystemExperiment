#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int show_return_status(void) 
{
	pid_t childpid;
	int status;   
	childpid = wait(&status);
	if (childpid == -1) 
		perror("Failed to wait for child");
	else if (WIFEXITED(status) && !WEXITSTATUS(status))
		printf("Child %ld terminated normally\n", (long)childpid);
	else if (WIFEXITED(status))
		printf("Child %ld terminated with return status %d\n", (long)childpid, WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		printf("Child %ld terminated due to uncaught signal %d\n", (long)childpid, WTERMSIG(status));
	else if (WIFSTOPPED(status))
		printf("Child %ld stopped due to signal %d\n", (long)childpid, WSTOPSIG(status));
	return childpid;
}

int main(int argc, char *argv[])
{
	pid_t childpid;
	int i, n;
	if (argc != 2) 
	{
		return 1;
	}
	n = atoi(argv[1]);
	for (i = 1; i < n; i++)
		if ((childpid = fork()) <= 0) 
			break;
	while(show_return_status() > 0) 
		printf("i:%d  process ID:%ld  parent ID:%ld  child ID:%ld\n", i, (long)getpid(), (long)getppid(), (long)childpid);
	return 0;
}

