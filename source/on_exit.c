#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void my_exit(int status,void *arg)
{
	printf("before exit()!\n");
	printf("exit (%d)\n",status);
	printf("arg = %s\n",(char*)arg);
}

int main(int argc,char**argv)
{
	char str[5];
	sprintf(str,"test");
	on_exit(my_exit,(void*)str);

	exit(12);
}

