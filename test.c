#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];
int main () {
    pid_t pid=fork();
    int source = open("/home/nanqin/OperatingSystems/source.txt", O_RDONLY);
    if (source < 0) perror("Source file not found");
	if(pid==0) {
		if(read(source, buffer, BUFFER_SIZE)>0) {
            printf("%s\n", buffer);
        }
	}
    else {
        sleep(10);
		if(read(source, buffer, BUFFER_SIZE)>0) {
            printf("%s\n", buffer);
        }
    }
    close(source);
}

void cpfile_text(int source, int target) {
    ssize_t bytesRead;
    while ((bytesRead = read(source, buffer, BUFFER_SIZE)) > 0) {
        if(write(target, buffer, bytesRead) != bytesRead) {
            perror("Error writing to target file");
        }
    }
    if (bytesRead < 0) {
        perror("Error reading from source file");
    }
}