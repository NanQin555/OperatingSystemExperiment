#include "consumer_producer.h"
int main(void)
{
    //信号量集
    //1:缓冲区 2：生产者对消费者通知 3：消费者对生产者通知
    int semid = semget(ftok("semaphore.temp", 0), 3, 0666|IPC_CREAT);
    if(semid == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    init_sem(semid,1);

    //共享内存
    struct BufferPool *shm;
    int shmid = shmget(ftok("shared.temp", 0), sizeof(struct BufferPool), 0666 | IPC_CREAT);
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
    //init BufferPool
    for(int i=0; i<NUM_BUFFER; i++) {
        memset(shm->Buffer[i], 0, sizeof(shm->Buffer[i]));
        shm->Buffer_status[i] = 0;
    }

    pid_t pid_arr[NUM_PRODUCERS + NUM_CONSUMERS];
    //创建生产者进程
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        pid_t pid = fork();
        int status;
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            printf("Producer%d is created\n", i);
            producer_task(i);
        } else 
            pid_arr[i] = pid;
    }
    //创建消费者进程
    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            printf("Consumer%d is created\n", i);
            consumer_task(i);
        } else 
            pid_arr[NUM_CONSUMERS+i] = pid;
    }

    // 等待所有子进程结束
    for (size_t i = 0; i < NUM_PRODUCERS + NUM_CONSUMERS; ++i) {
        int status;
        pid_t waited_pid = waitpid(pid_arr[i], &status, 0);
    }

    // 清理共享内存
    if (shmdt(shm) == -1)
    {
        exit(EXIT_FAILURE);
    }
    if (shmctl(shmid, IPC_RMID, 0) == -1)
    {
        exit(EXIT_FAILURE);
    }
    // 清理信号量集
    if (semctl(semid, 0, IPC_RMID, 0))
    exit(EXIT_SUCCESS);
    // return 0;
}