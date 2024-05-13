#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

#define SIZE 1024

int main() {

    // Initializing Winsock
    WSADATA wsa_data;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (iResult != 0) {
        printf("[ERROR] WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Defining the IP and Port
    char* ip = "192.168.1.21";
    const int port = 23;

    // Defining variables
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

    // Setting up the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

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

    // Sending the file data to the server
    char buffer[SIZE];
    int n;
    while (fgets(buffer, SIZE, fp) != NULL) {
        printf("[SENDING] Data: %s", buffer);

        n = sendto(server_sockfd, buffer, (int)strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (n == SOCKET_ERROR) {
            printf("[ERROR] sending data to the server: %d\n", WSAGetLastError());
            fclose(fp);
            closesocket(server_sockfd);
            WSACleanup();
            return 1;
        }
        memset(buffer, 0, SIZE);
    }

    // Sending the 'END'
    strcpy_s(buffer, SIZE, "END");
    n = sendto(server_sockfd, buffer, (int)strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (n == SOCKET_ERROR) {
        printf("[ERROR] sending data to the server: %d\n", WSAGetLastError());
        fclose(fp);
        closesocket(server_sockfd);
        WSACleanup();
        return 1;
    }

    printf("[SUCCESS] Data transfer complete.\n");
    printf("[CLOSING] Disconnecting from the server.\n");

    fclose(fp);
    closesocket(server_sockfd);
    WSACleanup();

    return 0;
}