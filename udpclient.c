#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8000
#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <server_ip> <file_path>\n", argv[0]);
        exit(1);
    }

    const char* server_ip = argv[1];
    const char* file_path = argv[2];

    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        exit(1);
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    FILE* file = fopen(file_path, "rb");
    if (!file) {
        printf("Error opening file: %d\n", GetLastError());
        closesocket(sock);
        WSACleanup();
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char buffer[BUFFER_SIZE];
    int bytes_sent;
    int total_bytes_sent = 0;

    while (total_bytes_sent < file_size) {
        int bytes_to_read = BUFFER_SIZE < file_size - total_bytes_sent ? BUFFER_SIZE : file_size - total_bytes_sent;
        size_t bytes_read = fread(buffer, 1, bytes_to_read, file);

        if (sendto(sock, buffer, bytes_read, 0, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            printf("sendto failed: %d\n", WSAGetLastError());
            break;
        }

        total_bytes_sent += bytes_read;
    }

    printf("File sent successfully. Total bytes sent: %ld\n", total_bytes_sent);

    fclose(file);
    closesocket(sock);
    WSACleanup();

    return 0;
}