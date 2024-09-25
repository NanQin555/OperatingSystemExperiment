#include <stdlib.h>
#include <sys/ipc.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
    key_t key;
    int n;
    char i;
    printf("input a charater:");
    i = getchar();
    for (n = 0; n < 4; i++, n++)
    {
        key = ftok(".", i);
        printf("id=%c\t key=[%08X]\t MSB=%c\n", i, key, key >> 24);
    }
    return 0;
}
