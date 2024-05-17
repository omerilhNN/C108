#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

#define SIZE 1024
#define PORT 23
int main() {
    WSADATA wsa_data;
    char ip[16];
    char buffer[SIZE];
    char path[64];
    char command[32];

    int wsa = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (wsa != 0) {
        printf("[ERROR] WSAStartup failed: %d\n", wsa);
        return 1;
    }
    SOCKET server_sockfd;
    struct sockaddr_in server_addr;

    printf("command IP path\n");
    scanf_s("%s %s %s", command,32, ip,16, path,64);
   
    if (strcmp(command,"send") == 0 && ip != '\0' && path != '\0') {
        server_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        FILE* file = fopen(path,"r");

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
        int bytes_sent;
        while (fgets(buffer, SIZE, file) != NULL) {
            /*bytes_sent = fgets(buffer, SIZE, file)) > 0*/
            printf("\nSending Data: %s", buffer);

            //server_socketi aracýlýðýyla - server addresine buffer'ý gönder
            bytes_sent = sendto(server_sockfd, buffer, (int)strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            if (bytes_sent == SOCKET_ERROR) {
                printf("[ERROR] sending data to the server: %d\n", WSAGetLastError());
                closesocket(server_sockfd);
                WSACleanup();
                return 1;
            }
            printf("\nData sent: %d bytes", bytes_sent);
            printf("Disconnecting from the server.\n\n");

            //her buffer'da SIZE kadar elemaný sýfýrla
            memset(buffer, 0, SIZE);
        }
    }
    else {
           printf("Invalid command\n");
           return -1;
    }

   

    closesocket(server_sockfd);
    WSACleanup();

    return 0;
}