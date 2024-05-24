/// @file TalkWeb_Client.c
/// @author Wang nanqin
/// @time 2024/05/24
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define BUFFSIZE 1024
#define PORT 8080
#define SERVER_IP "127.0.0.1"
int main() {
    int socket_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFSIZE];
    char send_message[BUFFSIZE];
    // memset(buffer, 0, BUFFSIZE);
    // memset(send_message, 0, BUFFSIZE);
    const char* exit_message = "exit";
    // create socket
    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0))<0) {
        perror("socket error");
        exit(1);
    }
    // set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // connection
    if(connect(socket_fd, (struct  sockaddr*)&server_addr, sizeof(server_addr))<0) {
        perror("connect error");
        exit(1);
    }
    while(1) {
        // send message
        printf("Send message: ");
        scanf("%s", send_message);
        send(socket_fd, send_message, strlen(send_message), 0);
        memset(buffer, 0, BUFFSIZE);
        if(recv(socket_fd, buffer, BUFFSIZE, 0)<0) {
            perror("recv error");
            exit(1);
        }
        printf("%s\n", buffer);

        // exit message
        if(strcmp(send_message, exit_message)==0) {
            printf("Send Exit message\n");
            close(socket_fd);
            break;
        }
    }

    return 0;
}