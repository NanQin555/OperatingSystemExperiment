/// @file TalkWeb_Server.c
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
#define LISTENQ 5
int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFSIZE];
    const char* exit_message = "exit";
    const char* received_message = "Message Received:";
    // create scoket
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0))<0) {
        perror("socket error");
        exit(1);
    }
    // bind socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))<0) {
        perror("bind error");
        exit(1);
    }
    // listen conection
    if(listen(server_fd, LISTENQ)<0) {
        perror("listen error");
        exit(1);
    }
    printf("Server listening on port %d \n", PORT);

    // accept connection
    // while(1) {
        if((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len)) < 0) {
            perror("accept error");
            exit(1);
        }
        printf("Client connected from %s:%d \n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        // communication
        while(1) {
            int received;
            memset(buffer, 0, BUFFSIZE);
            if((received = recv(client_fd, buffer, BUFFSIZE, 0))<0) {
                perror("recv error");
                break;
            }
            if(received == 0) {
                printf("Client disconnected!\n");
                break;
            }
            if(strcmp(buffer, exit_message)==0) {
                printf("Client has sent exit message!");
                break;
            }
            printf("%s %s\n", received_message, buffer);

            char msg[BUFFSIZE];
            memset(msg, 0, BUFFSIZE);

            strcat(msg, received_message);
            strcat(msg, buffer);
            send(client_fd, msg, strlen(msg), 0);
        }
        close(client_fd);
    // }
    close(server_fd);
    return 0;
}