
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <queue>
#include <iostream>

using namespace std;

pthread_mutex_t mutex;
std::queue<int> product;


void * produce(void *ptr)
{
	for (int i = 0; i < 10; ++i)
	{
		pthread_mutex_lock(&mutex);
		product.push(i);
		pthread_mutex_unlock(&mutex); 
	}
	return NULL;
}

void * consume(void *ptr)
{
	for (int i = 0; i < 10;)
	{
		pthread_mutex_lock(&mutex);
		if (product.empty())
		{
			pthread_mutex_unlock(&mutex);
			continue;
		}

		++i;
		std::cout<<"consume:"<<product.front()<<endl;
		product.pop();
		pthread_mutex_unlock(&mutex); 
	}
	return NULL;
}


int main()
{
	pthread_t tid1, tid2;
	void *retVal;
	pthread_mutex_init(&mutex, NULL);
	pthread_create(&tid1, NULL, consume, NULL);
	pthread_create(&tid2, NULL, produce, NULL);
	pthread_join(tid1, &retVal);
	pthread_join(tid2, &retVal);
	return 0;
}

