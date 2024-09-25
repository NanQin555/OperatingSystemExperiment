#include<stdio.h>
#include<sys/sem.h>
#define MYKEY 0x1a0a
#define IPC_R   000400
#define IPC_W   000200 
#define IPC_M   010000
int main()
{
    int semid;
    semid=semget(MYKEY,1,IPC_CREAT|IPC_R | IPC_W| IPC_M);
    printf("semid=%d\n",semid);
    return 0;
}

