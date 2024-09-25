#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
	int ret;
	char *argv[] = {"ls","-l",NULL};
	printf("entering main process---\n");

	if(fork()==0) 
	{
    		ret = execv("/bin/ls",argv);
		if(ret == -1) 
			perror("execl error");
		printf("exiting main process ----\n");
	}
	return 0;
}


