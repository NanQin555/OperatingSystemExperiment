#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main (void) 
{
	pid_t childpid;
	childpid = fork();

	if (childpid == -1) 
	{
		return 1;
	}

	if (childpid == 0)
		printf("I am child %d\n", (long)getpid());

	while(wait(NULL) != childpid)
		printf("A signal must have interrupted the wait!\n");

	printf("I am parent %d with child %d\n", (long)getpid(), (long)childpid);
	return 0;
}

