#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

#define SIZE 1024
#define PORT 23

int main() {
    WSADATA wsa_data;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (iResult != 0) {
        printf("[ERROR] WSAStartup failed: %d\n", iResult);
        return 1;
    }
    char* ip = "192.168.254.16";

    SOCKET server_sockfd;
    struct sockaddr_in server_addr;
    char* filename = "test.txt";
    FILE* fp = fopen(filename, "r");

    // Creating a UDP socket
    server_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server_sockfd == INVALID_SOCKET) {
        printf("[ERROR] socket error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) < 0)
    {
        printf("Invalid IP\n");
        closesocket(server_sockfd);
        return 1;
    }

    // Reading the text file
    if (fp == NULL) {
        printf("[ERROR] reading the file: %d\n", GetLastError());
        closesocket(server_sockfd);
        WSACleanup();
        return 1;
    }

    char buffer[SIZE];
    int bytes_sent;
    while (1) {
        bytes_sent = fgets(buffer, SIZE, fp);
        if (bytes_sent <= 0) break; // okunacak data kalmamýþsa break

        printf("[SENDING] Data: %s", buffer);

        bytes_sent = sendto(server_sockfd, buffer, (int)strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (bytes_sent == SOCKET_ERROR) {
            printf("[ERROR] sending data to the server: %d\n", WSAGetLastError());
            fclose(fp);
            closesocket(server_sockfd);
            WSACleanup();
            return 1;
        }
        memset(buffer, 0, SIZE);
    }

    
    strcpy_s(buffer, sizeof("END"), "END");

    printf("\n[SUCCESS] Data transfer complete.\n");
    printf("[CLOSING] Disconnecting from the server.\n");

    fclose(fp);
    closesocket(server_sockfd);
    WSACleanup();

    return 0;
}