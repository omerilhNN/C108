#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 23
#define MAXLINE 1000

typedef struct {
    long long sequence;
    long long timestamp;
} DataPacket;

int main() {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed");
        return 1;
    }

    SOCKET listener = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (listener == INVALID_SOCKET) {
        printf("server_socket failed: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    struct sockaddr_in addr;
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET,"192.168.254.21",&addr.sin_addr) < 0)
    {
        printf("INVALID IP\n");
        return 1;
    }
   
    if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        printf("bind failed\n");
        closesocket(listener);
        WSACleanup();
        return 1;
    }

    char buffer[MAXLINE];
    DataPacket packet;
    struct sockaddr_in cliaddr;
    int len = sizeof(cliaddr);

    while (1) {
        int bytesReceived = recvfrom(listener, (char*)&packet, sizeof(DataPacket), 0, (struct sockaddr*)&cliaddr, &len);
        if (bytesReceived > 0) {
            printf("Received packet: sequence=%lld, timestamp=%lld\n", packet.sequence, packet.timestamp);
        }
        else if(bytesReceived ==  0){
            
        }

       


        // Send response back to client
        char* message = "Hello Client";
        sendto(listener, message, strlen(message), 0, (struct sockaddr*)&cliaddr, len);
        
    }

    closesocket(listener);
    WSACleanup();
    return 0;
}