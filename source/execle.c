#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int main()
{
	char *envp[]={"PATH=/tmp","USER=shan",NULL};

	if(fork()==0)
	{
		if(execle("/bin/ls", "ls", "-l", NULL,envp)<0)
			perror("execle error!");
	}
	return 0;
}
