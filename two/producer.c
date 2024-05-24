/*  producer.c
    Nanqin Wang
    2024.4.19    */
#include "consumer_producer.h"
// producer
int main (int argc, char *argv[])  
{  
    int task_id = atoi(argv[1]);
    //信号量集
    int semid_Buffer = semget((key_t)2333, 5, 0666|IPC_CREAT);
    if(semid_Buffer == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    init_semBuffer(semid_Buffer, 1);
    int semid = semget((key_t)1234, 2, 0666|IPC_CREAT);
    if(semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    init_sem(semid, 0);

    //共享内存
    struct BufferPool *shm;
    int shmid = shmget((key_t)5678, sizeof(struct BufferPool), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    shm = (struct BufferPool*)shmat(shmid, NULL, 0);
    if (shm == (void *)(-1))
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    if(shm->is_init == 0) {
        Buffer_init(shm);
    }

    FILE *file = fopen("/home/nanqin/OperatingSystems/Experiment/two/sources.txt", "r");
    if (file == NULL) {
        perror("Failed to open the file");
        exit(EXIT_FAILURE);
    }
    
    int running = 1;  
    int index = 0;
    char buffer[BUFFER_SIZE];
    printf("producer%d is running\n", task_id);
    while (running)
    {
        semaphore_p(semid_Buffer, index); // 开始占用资源
        printf("producer%d is checking buffer%d\n", task_id, index);
        if (shm->Buffer_status[index]==0) 
        {
            if(fgets(buffer, 100, file) != NULL) {
                // int value = semctl(semid, CONSUMER_NOTIFY_PRODUCER, GETVAL);
                // if (value < 0) {
                //     semaphore_v(semid, CONSUMER_NOTIFY_PRODUCER);
                // }
                if(is_Buffer_empty(shm)) {
                    semaphore_v(semid, CONSUMER_NOTIFY_PRODUCER);
                }
                printf("buffer%d is empty\n", index);
                printf("producer%d product buffer: %s", task_id, buffer);
                strncpy(shm->Buffer[index], buffer, 100);
                shm->Buffer_status[index] = 1;
            }
            else {
                running = 0;
                shm->Producer_status[task_id] = 1;
            }
            semaphore_v(semid_Buffer, index);// 释放资源
            index++;
            index %= NUM_BUFFER;
        }
        else
        {
            printf("buffer%d is full\n", index);
            sleep(1);
            semaphore_v(semid_Buffer, index);// 释放资源
            index++;
            index %= NUM_BUFFER;
            if(is_Buffer_full(shm)) {
                printf("BufferPoll is full\n");
                // 通知消费者: 满了
                semaphore_p(semid, PRODUCER_NOTIFY_CONSUMER);
            }
        }
    }

    // 关闭文件
    if (fclose(file) != 0) {
        perror("Failed to close the file");
        return EXIT_FAILURE;
    }
    if(is_Consumers_end(shm)&&is_Producers_end(shm)) {
        //清理信号量集
        if (semctl(semid, 0, IPC_RMID, 0))
        exit(EXIT_SUCCESS);
        //清理共享内存
        if (shmdt(shm) == -1) {
            exit(EXIT_FAILURE);
        }
        if (shmctl(shmid, IPC_RMID, 0) == -1) {
            exit(EXIT_FAILURE);
        }
    } 
    else {
        // 断开进程空间和共享内存的连接
        if (shmdt(shm) == -1)
        {
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
}