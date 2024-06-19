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
#include <sys/wait.h>
#include <netinet/in.h>

#define MAX_RECEIVE_BUFFER_SIZE 1024
#define MAX_SEND_BUFFER_SIZE 1024000
#define MAX_CLIENT_NUM 256
#define PORT 8082
#define LISTENQ 5
const char *html_path = "/home/nanqin/OperatingSystems/Experiment/Six/index.html";
const char *pdf_path = "/home/nanqin/OperatingSystems/Experiment/Six/example.pdf";
const char *mongoose_c_path = "/home/nanqin/OperatingSystems/Experiment/Six/mongoose.c";
const char *mongoose_h_path = "/home/nanqin/OperatingSystems/Experiment/Six/mongoose.h";
const char *fushi_path = "/home/nanqin/OperatingSystems/Experiment/Six/image/fushi.jpeg";
void handle_client(int client_fd);
void send_error(int client_fd, int status_code, const char *message);
void send_connect(int client_fd);
void send_file(int client_fd, const char *file_path, const char *content_type);
int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    // pid_t pidlist[MAX_CLIENT_NUM];

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

    while(1) {
        if((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len)) < 0) {
            perror("accept error");
            exit(1);
        }
        printf("Client connected from %s:%d \n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        // communication

        pid_t pid;
        if((pid=fork())<0) {
            perror("fork error");
            exit(1);
        } 
        else if(pid==0) {
            close(server_fd);
            handle_client(client_fd);
            exit(EXIT_SUCCESS);
        }
        else {
            close(client_fd);
        }
    }
    close(server_fd);
    return 0;
}
void handle_client(int client_fd) {
    char buffer[MAX_RECEIVE_BUFFER_SIZE];
    char *last = NULL;
    int received;
    // HTTP receive
    if ((received = recv(client_fd, buffer, MAX_RECEIVE_BUFFER_SIZE, 0)) < 0) {
        perror("recv error");
        return;
    }
    printf("Received request: %s", buffer);
    buffer[received] = '\0';
    // 分割请求行
    char *request_line = strtok_r(buffer, "\r\n", &last);
    char method[256], path[256], protocol[256];
    sscanf(request_line, "%s %s %s", method, path, protocol);

    if (strcmp(method, "GET") != 0 || strcmp(protocol, "HTTP/1.1") != 0) {
        // not support method or protocol
        send_error(client_fd, 405, "Method Not Allowed");
        close(client_fd);
        return;
    }
    send_connect(client_fd);
    

    // requeset
    printf("%s", request_line);
    if (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0) {
        send_file(client_fd, html_path, "text/html");
    } else if (strcmp(path, "/example.pdf") == 0) {
        send_file(client_fd, pdf_path, "application/pdf");
    } else if (strcmp(path, "/mongoose.c")==0) {
        send_file(client_fd, mongoose_c_path, "text/plain");
    } else if (strcmp(path, "/mongoose.h")==0) {
        send_file(client_fd, mongoose_h_path, "text/plain");
    } else if (strcmp(path, "/image/fushi.jpeg")==0) {
        send_file(client_fd, fushi_path, "image/jpeg");
    } else {
        send_error(client_fd, 404, "Not Found");
    }

    close(client_fd);
}

void send_error(int client_fd, int status_code, const char *message) {
    char response[1024];
    sprintf(response,
            "HTTP/1.1 %d %s\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            status_code, message, message);
    write(client_fd, response, strlen(response));
}
void send_connect(int client_fd) {
    char response[1024];
    sprintf(response,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "charset=UTF-8\r\n"
            "Connection: keep-alive\r\n");
    send(client_fd, response, strlen(response), 0);
}

void send_file(int client_fd, const char *file_path, const char *content_type) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        send_error(client_fd, 404, "Not Found");
        return;
    }

    // Get file size (less than 2GB)
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // send HTTP head
    char response_header[1024];
    snprintf(response_header, sizeof(response_header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %ld\r\n"
             "Connection: keep-alive\r\n\r\n",
             content_type, file_size);
    if(send(client_fd, response_header, strlen(response_header), 0)<0) {
        perror("send");
        fclose(file);
        return;
    }

    // send content
    char buffer[MAX_SEND_BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, MAX_SEND_BUFFER_SIZE, file)) > 0) {
        if(send(client_fd, buffer, bytes_read, 0)<0) {
            printf("send error: %s", buffer);
            perror("send");
            break;

        }
    }
    if(bytes_read==0) {
       printf("send to client successfuly\n");
    }
    fclose(file);
} 