#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa_data;
    SOCKET socket_desc;
    struct sockaddr_in server_addr, client_addr;
    char server_message[2000], client_message[2000];
    int client_struct_length = sizeof(client_addr);

    // Initialize Winsock:
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

    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(23);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);// herhangi bir yerel IP üzerinden gelen baðlantýyý kabul eder-host to network long  

    
    // Bind to the set port and IP:
    if (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Couldn't bind to the port: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    printf("Done with binding\n");

    printf("Listening for incoming messages...\n\n");

    while (1) {
        int recv_size = recvfrom(socket_desc, client_message, sizeof(client_message), 0,
            (struct sockaddr*)&client_addr, &client_struct_length);
        if (recv_size == SOCKET_ERROR) {
            printf("Couldn't receive: %d\n", WSAGetLastError());
            WSACleanup();
            return -1;
        }

        printf("Received message from IP: %s and client port: %i\n",
            inet_ntop(AF_INET, &client_addr.sin_addr, server_message, sizeof(server_message)), ntohs(client_addr.sin_port));

        printf("Msg from client: %.*s\n", recv_size, client_message);//client message'ýn okunan byte kadarýný yazdýr 

        //// Respond to client:
        //strcpy_s(server_message, sizeof(server_message) +1 , client_message);

        //if (sendto(socket_desc, server_message, strlen(server_message), 0,
        //    (struct sockaddr*)&client_addr, 2000) == SOCKET_ERROR) {
        //    printf("Can't send: %d\n", WSAGetLastError());
        //    WSACleanup();
        //    return -1;
        //}
    }

    // Close the socket:
    closesocket(socket_desc);

    WSACleanup();

    return 0;
}