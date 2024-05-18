#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

#define SIZE 1024
#define PORT 23
int main() {
    WSADATA wsa_data;
    char ip[16], command[32], buffer[SIZE], path[64], recvBuf[SIZE];
    int bytes_recv;
    
    int wsa = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (wsa != 0) {
        printf("[ERROR] WSAStartup failed: %d\n", wsa);
        WSACleanup();
        return 1;
    }

    SOCKET server_sockfd;
    struct sockaddr_in server_addr,from_addr;
    socklen_t fromlen = sizeof(from_addr);

    //get command
    printf("command IP path\n");
    scanf_s("%s %s %s", command,32, ip,16, path,64);
   
    if (strcmp(command,"send") == 0 && ip != '\0' && path != NULL) {
        
        //udp soket tanýmla
        server_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        
        //verdiðin pathteki dosyayý "read" modunda aç
        FILE* sendFile = fopen(path,"r");
        if (sendFile == NULL) {
            printf("Failed to open file\n");
            WSACleanup();
            return -1;
        }
        //file boyutunu al
        fseek(sendFile, 0, SEEK_END);
        int fileSize = ftell(sendFile);
        rewind(sendFile);

        //fread ile buffer'a okuma yap - okunan byte sayýsýný readedBytes'a ver
        size_t readedBytes = fread(buffer, 1, fileSize, sendFile);
        //end of file ise -> okuma tamamlandý
        if (feof(sendFile)) {
            printf("Read file success.\n\n");
        }
        else if (ferror(sendFile)) {
            printf("Error reading file\n");
            fclose(sendFile);
            WSACleanup();
            return 1;
        }
        //null terminated
        buffer[readedBytes] = '\0';
        fclose(sendFile);

        if (server_sockfd == INVALID_SOCKET) {
            printf("[ERROR] socket error: %d\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }
        
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);

        //presentation to network - string verilen arrayi network(binary form) çevir
        if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0)
        {
            printf("Invalid IP\n");
            closesocket(server_sockfd);
            return 1;
        }
        //Soketi - server_addr'in IP ve portuna baðla
        if (bind(server_sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr)) == SOCKET_ERROR) {
            printf("Bind error\n");
            closesocket(server_sockfd);
            return 1;
        }

        int bytes_sent;
        printf("Sending Data: %s\n", buffer);

        //server_socketi aracýlýðýyla - server addresine buffer'ý gönder
        bytes_sent = sendto(server_sockfd, buffer, (int)strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (bytes_sent == SOCKET_ERROR) {
                printf("[ERROR] sending data to the server: %d\n", WSAGetLastError());
                closesocket(server_sockfd);
                WSACleanup();
                return 1;
        }
        printf("Data sent: %d bytes\n", bytes_sent);

        //Eðer datanýn nereden geldiði ile ilgili bilgiyi tutmak istiyorsan FROM_ADDR ve FROMLEN parametresini kullan -> ELSE null
        //socketten data geldi - recvBuf ile datayý tut - datanýn nereden geldiðini FROM_ADDR ve FROMLEN ile tut
        bytes_recv = recvfrom(server_sockfd,recvBuf,bytes_sent,0,&from_addr,&fromlen);
        if (bytes_recv < 0) {
                printf("RECV Error\n");
                closesocket(server_sockfd);
                return 1;
        }

        //File'ý append modunda açma iþlemini yap - eðer file yoksa recv.txt oluþtur
        FILE* recvFile = fopen("recv.txt", "a");
        if (recvFile == NULL) {
                printf("Failed to create file\n");
                return 1;
        }

        //null terminate
        recvBuf[bytes_recv] = '\0';
        //recvfrom'dan aldýðýn recvBuf datasýný recvFile'a yaz.
        fwrite(recvBuf, 1, (size_t)strlen(recvBuf), recvFile);
           
        //buffer'da SIZE kadar elemaný sýfýrla
        memset(buffer, 0, SIZE);
    }
    else {
           printf("Invalid command || IP || Path entered.\n");
           return -1;
    }

    closesocket(server_sockfd);
    WSACleanup();

    return 0;
}