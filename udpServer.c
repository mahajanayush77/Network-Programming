#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)

int main(){
    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // ipv4 address family
    hints.ai_socktype = SOCK_DGRAM; // datagram type socket(UDP)
    hints.ai_flags = AI_PASSIVE; // flag is set as binding the local adrrinfo 

    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address); // bind adrress is set to local adrress and port : 8080

    printf("Creating socket...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,bind_address->ai_socktype, bind_address->ai_protocol); // creating a socket

    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Binding socket to local address...\n"); // binding the socket
    if (bind(socket_listen,bind_address->ai_addr, bind_address->ai_addrlen)){
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_address); // frees the bind_address addrinfo structure

    printf("Listening to socket...\n");
    
    while(1){
        struct sockaddr_storage client_address; // storing client address info
        socklen_t client_len = sizeof(client_address); // len of client adrress

        char read[1024]; // read buffer
        int bytes_received = recvfrom(socket_listen, read, 1024, 0,(struct sockaddr *)&client_address, &client_len); // bytes received from the client
        if (bytes_received < 1){
            fprintf(stderr, "connection closed. (%d)\n",GETSOCKETERRNO());
            return 1;
        }
        
        printf("Client : %.*s", bytes_received, read); // printing the received message from client
        
        
        sendto(socket_listen, read, bytes_received, 0,(struct sockaddr*)&client_address, client_len); // sending the message back to client (ECHO)
    }

    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen); // stop listening to socket

    printf("Finished.\n");

    return 0;
}
