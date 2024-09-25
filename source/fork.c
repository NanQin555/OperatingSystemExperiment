#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
int main(void)
{
	pid_t pid;
	pid=fork();
	if(pid==-1)
		printf("fork error\n");
	else if(pid==0)
		printf("in the child process\n");
	else
		printf("in the parent process,the child process id is %d\n",pid);
	return 0;
}

