#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main (void)
{
	pid_t childpid;
	childpid = fork();
	if (childpid == -1) {
		return 1;   
	}
	if (childpid == 0)
		printf("I am child %ld\n", (long)getpid());
	else if (wait(NULL) != childpid)
		printf("A signal must have interrupted the wait!\n");
	else
		printf("I am parent %ld with child %ld\n", (long)getpid(), (long)childpid);
	return 0;
}

