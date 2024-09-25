#include<stdio.h>
#include<sys/sem.h>
#define MYKEY 0x1a0a
int main()
{
	int semid;
	semid=semget(MYKEY,1, IPC_CREAT);
	printf("semid=%d\n",semid);
	return 0;
}

