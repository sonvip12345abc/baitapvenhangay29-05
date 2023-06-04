#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"  // Địa chỉ IP của server
#define SERVER_PORT 8888       // Cổng của server

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    // Tạo socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Không thể tạo socket");
        exit(1);
    }

    // Thiết lập thông tin server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &(serverAddr.sin_addr)) <= 0) {
        perror("Địa chỉ IP không hợp lệ");
        exit(1);
    }

    // Kết nối tới server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Không thể kết nối tới server");
        exit(1);
    }

    // Nhập lệnh từ người dùng
    printf("Nhập lệnh (GET_TIME [format]): ");
    fgets(buffer, sizeof(buffer), stdin);

    // Gửi lệnh tới server
    if (send(clientSocket, buffer, strlen(buffer), 0) < 0) {
        perror("Không thể gửi lệnh tới server");
        exit(1);
    }

    // Nhận phản hồi từ server
    memset(buffer, 0, sizeof(buffer));
    if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0) {
        perror("Lỗi khi nhận phản hồi từ server");
        exit(1);
    }

    // Hiển thị phản hồi từ server
    printf("Phản hồi từ server: %s\n", buffer);

    // Đóng kết nối
    close(clientSocket);

    return 0;
}