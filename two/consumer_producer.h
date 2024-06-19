/*  consumer_producer.h
    Nanqin Wang
    2024.4.19   */
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <unistd.h>
#define NUM_CONSUMERS 2
#define NUM_PRODUCERS 2
#define NUM_BUFFER 5
#define BUFFER_SIZE 100
// #define BUFFER 0
#define PRODUCER_NOTIFY_CONSUMER 0
#define CONSUMER_NOTIFY_PRODUCER 1
union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
};
struct BufferPool {
    char Buffer[NUM_BUFFER][BUFFER_SIZE];
    int Buffer_status[NUM_BUFFER];
    //0: 无 1: 有
    int is_init;  //是否初始化 0:需要初始化 1：不需要初始化
    int Producer_status[NUM_PRODUCERS]; //1: 结束 0：继续
    int Consumer_status[NUM_CONSUMERS];
};
void Buffer_init(struct BufferPool *shm);
int is_Buffer_full(struct BufferPool *shm);
int is_Buffer_empty(struct BufferPool *shm);
int is_Producers_end(struct BufferPool *shm);
int is_Consumers_end(struct BufferPool *shm);
int init_sem(int sem_id, int init_value);
int init_semBuffer(int sem_id, int init_value);
int del_sem(int sem_id);
void semaphore_p(int sem_id, short sem_no);
void semaphore_v(int sem_id, short sem_no);
void print_semval(int sem_id, short sem_no);



void Buffer_init(struct BufferPool *shm) {
    for(int i=0; i<NUM_BUFFER; i++) {
        memset(shm->Buffer[i], 0, sizeof(shm->Buffer[i]));
        shm->Buffer_status[i] = 0;
        shm->Producer_status[i] = 0;
        shm->Consumer_status[i] = 0;
    }
    shm->is_init = 1;
}
int is_Buffer_full(struct BufferPool *shm) {
    for(int i=0; i<NUM_BUFFER; i++) {
        if(shm->Buffer_status[i] == 0) {
            return 0;
        }
    }
    return 1;
}
int is_Buffer_empty(struct BufferPool *shm) {
    for(int i=0; i<NUM_BUFFER; i++) {
        if(shm->Buffer_status[i] == 1) {
            return 0;
        }
    }
    return 1;
}
int is_Producers_end(struct BufferPool *shm) {
    for(int i=0; i<NUM_PRODUCERS; i++) {
        if(shm->Producer_status[i] == 0) {
            return 0;
        }
    }
    return 1;
}
int is_Consumers_end(struct BufferPool *shm) {
    for(int i=0; i<NUM_CONSUMERS; i++) {
        if(shm->Consumer_status[i] == 0) {
            return 0;
        }
    }
    return 1;
}
int init_sem(int sem_id, int init_value)
{
    union semun sem_union;
    sem_union.val = init_value; 
    //BUFFER
    for(int i=0; i<2; i++) {
        if (semctl(sem_id, i, SETVAL, sem_union) == -1)
        {
            perror("Initialize semaphore");
            return -1;
        }
    }
    printf("semaphore:sem init successfully\n");
    return 1;
}
int init_semBuffer(int sem_id, int init_value)
{
    union semun sem_union;
    sem_union.val = init_value;
    //BUFFER
    for(int i=0; i<NUM_BUFFER; i++) {
        if (semctl(sem_id, i, SETVAL, sem_union) == -1)
        {
            perror("Initialize semaphore");
            return -1;
        }
    }
    printf("semaphore:semBuffer init successfully\n");
    return 1;
}
int del_sem(int sem_id)
{
    union semun sem_union;
    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
    {
        perror("Delete semaphore");
        return -1;
    }
    return 1;
}
void semaphore_p(int sem_id, short sem_no)
{
    // printf("sem_no:%d, p\n", sem_no);
    // printf("     before p ");
    // print_semval(sem_id, sem_no);

    struct sembuf sem_b;
    sem_b.sem_num = sem_no; // 信号量集中信号量编号
    sem_b.sem_op = -1;      // P操作，每次分配1个资源
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1)
    {
        printf("sem_no:%d, p failed\n", sem_no);
        perror("semaphore_p failed");
    }
    // printf("sem_no:%d, p successfully\n", sem_no);
    // printf("     after p ");
    // print_semval(sem_id, sem_no);
}
void semaphore_v(int sem_id, short sem_no)
{
    // printf("sem_no:%d, v\n", sem_no);
    // printf("     before v ");
    // print_semval(sem_id, sem_no);
    struct sembuf sem_b;
    sem_b.sem_num = sem_no; // 信号量集中信号量编号
    sem_b.sem_op = 1;       // V操作，每次释放1个资源
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1)
    {
        printf("sem_no:%d, v failed\n", sem_no);
        perror("semaphore_v failed");
    }
    // printf("sem_no:%d, v successfully\n", sem_no);
    // printf("     after v ");
    // print_semval(sem_id, sem_no);
}
void print_semval(int sem_id, short sem_no) {
    int semval;
    semval = semctl(sem_id, sem_no, GETVAL);
    printf("Sem_no%d value is: %d\n", sem_no, semval);
}