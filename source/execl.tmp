

#include <unistd.h>
#include <stdio.h>

int main(void)
{
	printf("entering main process---\n");
	if(fork()==0)
	{
		execl("/bin/ls","ls","-l",NULL); 
		printf("exiting main process ----\n");
	}
	return 0; 
}

