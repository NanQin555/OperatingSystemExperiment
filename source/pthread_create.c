#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *childthread(void)
{
	int i;
	for(i=0;i<10;i++)
	{
		printf("childthread message\n");
		sleep(100);
	}
}

int main(){
	pthread_t tid;

	printf("create childthread\n"); 
	pthread_create(&tid,NULL,(void *) childthread,NULL);
	sleep(3);
	printf("process exit\n"); 
}

