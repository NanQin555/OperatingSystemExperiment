#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

char    buf1[] = "abcdefghij";
char    buf2[] = "ABCDEFGHIJ";
#define FILE_MODE   0x0644

void err_sys(const char *err)
{
	printf("%s\n", err);
	exit(0);
}

int main(void)
{
        int             fd;
	char buf[11];
	int i=0;

        if ( (fd = open("file.hole", O_CREAT|O_RDWR, FILE_MODE)) < 0)
                err_sys("creat error");
        if (write(fd, buf1, 10) != 10)
                err_sys("buf1 write error");
        if (lseek(fd, 40, SEEK_SET) == -1)
                err_sys("lseek error");
        if (write(fd, buf2, 10) != 10)
                err_sys("buf2 write error");
	if(lseek(fd, 0 , SEEK_SET) == -1)
		err_sys("seek to begin error\n");
	memset(buf, 0, 11);
	if(read(fd, buf, 10) < 10)
		err_sys("read et begin error\n");
	printf("At begin have read:%s\n", buf);
	
	if (lseek(fd, 10, SEEK_SET) == -1)
		err_sys("seek to 10 error\n");
	memset(buf, 0, 11);
	if(read(fd, buf, 10) != 10)
		err_sys("read at 10 error\n");
	printf("At begin of 10, have read data:\n");
	for(i=0; i<10; i++)
	{
		printf("%x", buf[i]);
	}
	printf("\n");
	
	if (lseek(fd, 40, SEEK_SET) == -1)
		err_sys("seek to 10 error\n");
	memset(buf, 0,10);
	if(read(fd, buf, 10) != 10)
		err_sys("read at 10 error\n");
	printf("At begin of 10, have read data:%s\n", buf);
	
        exit(0);
}

