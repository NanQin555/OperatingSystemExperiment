#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fun1()
{
	printf("fun1被调用！\n ");
}
void fun2()
{
	printf("fun2被调用！\n");
}
void fun3()
{
	printf("fun3被调用！\n");
}    

int main(int argc,char **argv)
{
  atexit(fun1);
  atexit(fun2);
  atexit(fun3);
  printf("进程结束！\n");
  return 0;
}


