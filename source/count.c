

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t count_lock;
pthread_cond_t  count_ready;
int count = 0;

void* decrement_count(void* arg)
{
	pthread_mutex_lock(&count_lock);
	printf("decrement:waiting\n");
	pthread_cond_wait(&count_ready, &count_lock);
	//等待条件变量，期间互斥量仍然可用
	count = count - 1;
	printf("decrement:count=%d\n", count);
	pthread_mutex_unlock(&count_lock);
	printf("decrement quit\n");
	pthread_exit(NULL);
}

void *increment_count(void *arg)
{
	pthread_mutex_lock(&count_lock);
	printf("increment:running\n");
	count = count + 1;
	pthread_cond_signal(&count_ready);
	//触发条件变量
	printf("increment:count=%d\n", count);
	pthread_mutex_unlock(&count_lock);
	pthread_exit(NULL);
}


int main(int argc, char* argv[])
{
	pthread_t tid1,tid2;
	count=0;
	pthread_mutex_init(&count_lock,NULL);
	pthread_cond_init(&count_ready,NULL);
	pthread_create(&tid2,NULL,increment_count,NULL);
	sleep(2);
	pthread_create(&tid1,NULL,decrement_count,NULL);
	pthread_join(tid2,NULL);
	// printf("decrement quit\n");
	pthread_join(tid1, NULL);
	return 0;
}

