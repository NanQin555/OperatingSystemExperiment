
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int rfd, wfd, n;
	char buf[1024];
	
	if(argc < 3)
	{
		printf("please give source file and dest file\n");
	}
	
	rfd = open(argv[1], O_RDONLY);
	if(rfd < 0)
	{
		printf("open read file %s error\n", argv[1]);
		return -1;
	}
	wfd = open(argv[2], O_WRONLY);
	if(wfd < 0)
	{
		printf("open write file %s error\n", argv[2]);
		return -1;
	}
	while(n=read(rfd, buf, 1024)>0)
	{
		if(write(wfd, buf, n) != n)
		{
			printf("write file error\n");
			return -1;
		}
	}
	if(n < 0)
		printf("read source file error\n");
	close(rfd);
	close(wfd);
	return 0;	
}
