#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_CLIENT 5 

void udpserver() {
    WSADATA wsa_data;
    SOCKET socket_desc;
    struct sockaddr_in server_addr, client_addr;
    char client_message[2000];
    char* server_message = "Hello from server\n";
    int client_struct_length = sizeof(client_addr);

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return -1;
    }

    // Create UDP socket:
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_desc == INVALID_SOCKET) {
        printf("Error while creating socket: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    printf("Socket created successfully\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(23);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);// herhangi bir yerel IP üzerinden gelen baðlantýyý kabul eder-host to network long  

    
    //Bind - server deðerleri
    if (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Couldn't bind to the port: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    printf("Bind Success\n");

    while (1) {
        int recv_size = recvfrom(socket_desc, client_message, sizeof(client_message), 0,
            (struct sockaddr*)&client_addr, &client_struct_length);
        if (recv_size == SOCKET_ERROR) {
            printf("Couldn't receive: %d\n", WSAGetLastError());
            WSACleanup();
            return -1;
        }
  
        if (recv_size > 2000) {
            printf("recv Buffer overflow \n");
            recv_size = 2000;
            
        }
     
        char client_ip[INET_ADDRSTRLEN]; // IP adresini tutmak için yeni bir karakter dizisi oluþtur
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN); //network to presentation - client_ip buffer'ýna IP'yi stringe çevirip ata
        printf("Received a message from IP: %s and client port: %i\n", client_ip, ntohs(client_addr.sin_port));
        
        printf("Msg from client: %.*s\n", recv_size, client_message);//client message'ýn okunan byte kadarýný yazdýr 

     /*   if (sendto(socket_desc, server_message, sizeof(server_message), 0,
            (struct sockaddr*)&client_addr, client_struct_length) == SOCKET_ERROR) {
            printf("Can't send: %d\n", WSAGetLastError());
            WSACleanup();
            return -1;
        }
        printf("Response from server: %s\n",server_message);*/
    }

    // Close the socket:
    closesocket(socket_desc);

    WSACleanup();

    return 0;
}