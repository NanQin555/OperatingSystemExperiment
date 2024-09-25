
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void cleanup()
{
	printf("cleanup\n");
}

void *test_cancel(void)
{
	pthread_cleanup_push(cleanup,NULL);
	printf("test_cancel\n");
	printf("test message\n");
	sleep(1);
	printf("child befor exit\n");
	//pthread_exit(0);
	printf("child have exit\n");
	pthread_cleanup_pop(0);
	printf("have do pop\n");
}

int main(){
	pthread_t tid;
	pthread_create(&tid,NULL,(void *)test_cancel,NULL);
	sleep(2);
	printf("befor cancel\n");
	//pthread_cancel(tid);
	printf("after cancel\n");
	pthread_join(tid,NULL);

}
