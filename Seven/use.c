#include <linux/sched.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#define MAX 102400
#define RDWRSIZE 1024
#define PROCESS_INFO 1
#define OFFSET_SAVE_CUR 2
#define OFFSET_SET_SAVED 3
#define OFFSET_READ 1
#define OFFSET_WRITE 2
char buffer_user[MAX];
int main() {
    ssize_t write_len, read_len;

    memset(buffer_user, 0, sizeof(buffer_user));
    int fd = open("/dev/my_char_dev", O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }
    ioctl(fd, OFFSET_SAVE_CUR, OFFSET_READ);
    ioctl(fd, OFFSET_SAVE_CUR, OFFSET_WRITE);
    // test write
    char text[] = "Hello World!";
    write_len = write(fd, text, strlen(text)+1);
    read_len = read(fd, buffer_user, strlen(text)+1);
    printf("read_len: %ld\n", read_len);
    if(read_len < 0) {
        perror("read");
        return -1;
    }
    else if(read_len == 0) {}
    else {
        printf("%.*s\n", strlen(text)+1, buffer_user);
    }
    ioctl(fd, OFFSET_SET_SAVED, OFFSET_READ);
    ioctl(fd, OFFSET_SET_SAVED, OFFSET_WRITE);

    // test ioctl
    if(ioctl(fd, PROCESS_INFO, 0) < 0) {
        perror("ioctl Process_info");
        return -1;
    }

    read_len = read(fd, &buffer_user, MAX);
    printf("read_len: %ld\n", read_len);
    if(read_len < 0) {
        perror("read");
        return -1;
    }
    else if(read_len == 0) {}
    else {
        printf("%s\n", buffer_user);
    }

    // test write
    ioctl(fd, OFFSET_SET_SAVED, OFFSET_READ);
    ioctl(fd, OFFSET_SET_SAVED, OFFSET_WRITE);
    char text1[] = "Hello Kernel!";
    write_len = write(fd, text1, strlen(text1)+1);
    read_len = read(fd, buffer_user, strlen(text1)+1);
    printf("read_len: %ld\n", read_len);
    if(read_len < 0) {
        perror("read");
        return -1;
    }
    else if(read_len == 0) {}
    else {
        // printf("%s\n",buffer_user);
        printf("%.*s\n", strlen(text1)+1, buffer_user);
    }
    close(fd);
    return 0;
}