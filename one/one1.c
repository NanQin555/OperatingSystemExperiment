// 互斥锁
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#define MAX 100000
#define PHILOSOPHER_NUM 5
pthread_mutex_t mlock[5];
pthread_cond_t cond[5];
int times[5] = {0};
void eat(int number) {
    printf("philosopher %d is eating\n", number);
    for(int i=0 ;i<MAX; i++) {
        int a=i;
    }
    pthread_mutex_unlock(&mlock[number]);
    pthread_mutex_unlock(&mlock[(number + 1) % 5]);
    pthread_cond_signal(&cond[(number + 1) % 5]);
    pthread_cond_signal(&cond[(number - 1 + 5) % 5]);
    times[number]++;
}

void philosopher(int number) {
    while(1) {
        printf("philosopher %d is thinking\n", number);
        for(int i=0 ;i<MAX; i++) {
            int a=i;
        }
        pthread_mutex_lock(&mlock[number]);
        while (pthread_mutex_trylock(&mlock[(number + 1) % 5]) != 0) {
            pthread_cond_wait(&cond[number], &mlock[number]);
            // pthread_cond_wait(&cond[number]);
            // pthread_mutex_lock(&mlock[number]);
        }
        eat(number);
    }
}

void *philosopher_adapter(void *arg) {
    int number = (intptr_t)arg;
    philosopher(number);
    return NULL;
}

int main () {
    for(int i=0; i<5; i++) {
        pthread_mutex_init(&mlock[i], NULL);
        pthread_cond_init(&cond[i], NULL);
    }

    pthread_t thread0, thread1, thread2, thread3, thread4;
    pthread_create(&thread1, NULL, philosopher_adapter, (void*)1);
    pthread_create(&thread2, NULL, philosopher_adapter, (void*)2);
    pthread_create(&thread3, NULL, philosopher_adapter, (void*)3);
    pthread_create(&thread4, NULL, philosopher_adapter, (void*)4);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);

    for(int i=0; i<PHILOSOPHER_NUM; i++) {
        pthread_mutex_destroy(&mlock[i]);
        pthread_cond_destroy(&cond[i]);
    }
    printf("End\n");
    return 0;
}