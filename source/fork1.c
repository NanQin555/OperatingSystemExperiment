#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int       glob = 6;               /* external variable in initialized data */
char    buf[ ] = "a write to stdout\n";
int  main(void)
{
        int       var;            /* automatic variable on the stack */
        pid_t   pid;
        var = 88;
        if (write(STDOUT_FILENO, buf, sizeof(buf)-1) != sizeof(buf)-1)
	{
		printf("write stdout error\n");
		return -1;
	}

        printf("before fork\n");        /* we don't flush stdout */
        if ( (pid = fork()) < 0)
	{
		printf("fork error \n");
		return -1;
	}
        else if (pid == 0) {            /* child */
                glob++;                                 /* modify variables */
                var++;
        } else
                sleep(2);                               /* parent */
        printf("pid = %d, glob = %d, var = %d\n", getpid(), glob, var);
        exit(0);
}
