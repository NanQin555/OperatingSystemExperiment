// 信号量
// 拿起筷子逻辑不同
#include <stdio.h>
#include <semaphore.h>
#include <stdint.h>
#include <pthread.h>
#define MAX 100000
#define PHILOSOPHER_NUM 5
sem_t mlock[5];
sem_t ready[5];
int times[5] = {0};
void eat(int number) {
    printf("philosopher %d is eating\n", number);
    for(int i=0 ;i<MAX; i++) {
        int a=i;
    }
    sem_post(&mlock[number]);
    sem_post(&mlock[(number + 1) % 5]);
    sem_wait(&ready[number]);
    times[number]++;
}

void philosopher(int number) {
    //while(times[number] < 10) {
    while(1) {
        printf("philosopher %d is thinking\n", number);
        for(int i=0 ;i<MAX; i++) {
            int a=i;
        }
        if(number%2!=0) {//奇数
            sem_wait(&mlock[number]);
            if(sem_trywait(&mlock[(number+1)%5])==0) {
                sem_post(&ready[number]);
                eat(number);
            }
            else sem_post(&mlock[number]);
        }
        else {//偶数
            sem_wait(&mlock[(number+1)%5]);
            if(sem_trywait(&mlock[number])==0) {
                sem_post(&ready[number]);
                eat(number);
            }
            else sem_post(&mlock[(number+1)%5]);
        }
    }
}

void *philosopher_adapter(void *arg) {
    int number = (intptr_t)arg;
    philosopher(number);
    return NULL;
}

int main () {
    for(int i=0; i<5; i++) {
        sem_init(&mlock[i], 0, 1);
        sem_init(&ready[i], 0, 0);
    }

    // for (int i=0; i<PHILOSOPHER_NUM; i++) {
    //     pthread_t thread0;
    //     pthread_create(&thread0, NULL, philosopher_adapter, (void*)i);
    //     pthread_join(thread0, NULL);
    // }
    pthread_t thread0, thread1, thread2, thread3, thread4;
    pthread_create(&thread0, NULL, philosopher_adapter, (void*)0);
    pthread_create(&thread1, NULL, philosopher_adapter, (void*)1);
    pthread_create(&thread2, NULL, philosopher_adapter, (void*)2);
    pthread_create(&thread3, NULL, philosopher_adapter, (void*)3);
    pthread_create(&thread4, NULL, philosopher_adapter, (void*)4);

    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);

    for(int i=0; i<5; i++) {
        sem_destroy(&mlock[i]);
        sem_destroy(&ready[i]);
    }
    printf("End\n");
    return 0;
}