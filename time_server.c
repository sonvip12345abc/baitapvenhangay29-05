#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    // Đọc lệnh từ client
    ssize_t bytesRead = read(clientSocket, buffer, BUFFER_SIZE - 1);
    if (bytesRead <= 0) {
        perror("Lỗi đọc từ socket");
        close(clientSocket);
        return;
    }

    // Xử lý lệnh
    buffer[bytesRead] = '\0';
    if (strncmp(buffer, "GET_TIME", 8) != 0) {
        strcpy(response, "Lệnh không hợp lệ");
    } else {
        char format[BUFFER_SIZE] ;
        strncpy(format,buffer+9,BUFFER_SIZE);
        if (format == NULL) {
            strcpy(response, "Lệnh không hợp lệ");
        } else {
             printf("Đã nhận lệnh GET_TIME với format:%s\n", format);
            time_t rawtime ;
            struct tm *timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);

            if (strcmp(format,"dd/mm/yyyy") == 0) {
                printf("dung dinh dang");
                strftime(response, BUFFER_SIZE, "%d/%m/%y", timeinfo);
            } else if (strcmp(format,"dd/mm/yy") == 0) {
                strftime(response, BUFFER_SIZE, "%d/%m/%y", timeinfo);
            } else if (strcmp(format,"mm/dd/yyyy") == 0) {
                strftime(response, BUFFER_SIZE, "%m/%d/%Y", timeinfo);
            } else if (strcmp(format, "mm/dd/yy") == 0) {
                strftime(response, BUFFER_SIZE, "%m/%d/%y", timeinfo);
            } else {
                strcpy(response, "Định dạng không hợp lệ");
            }
        }
    }

    // Gửi phản hồi cho client
    ssize_t bytesSent = write(clientSocket, response, strlen(response));
    if (bytesSent <= 0) {
        perror("Lỗi ghi vào socket");
    }

    // Đóng kết nối với client
    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientLength;

    // Tạo socket server
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Lỗi tạo socket");
        exit(1);
    }

    // Cấu hình địa chỉ server
    memset((char*)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8888);

    // Gắn địa chỉ với socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Lỗi gắn địa chỉ với socket");
        exit(1);
    }

    // Lắng nghe kết nối từ client
    if (listen(serverSocket, 5) < 0) {
        perror("Lỗi lắng nghe kết nối");
        exit(1);
    }

    printf("Time server đang chạy...\n");

    // Chấp nhận và xử lý các kết nối từ client
    while (1) {
        clientLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientLength);
        if (clientSocket < 0) {
            perror("Lỗi chấp nhận kết nối");
            continue;
        }

        printf("Đã kết nối với client\n");

        // Xử lý client trong một tiến trình con
        pid_t childPid = fork();
        if (childPid == 0) {
            // Tiến trình con
            close(serverSocket);
            handleClient(clientSocket);
            exit(0);
        } else if (childPid > 0) {
            // Tiến trình cha
            close(clientSocket);
        } else {
            perror("Lỗi tạo tiến trình con");
            close(clientSocket);
        }
    }

    // Đóng socket server
    close(serverSocket);

    return 0;
}
