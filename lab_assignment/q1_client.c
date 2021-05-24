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
    printf("Configuring remote address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    struct addrinfo *peer_address; // peer address info set to 127.0.0.1 : 8080
    if (getaddrinfo("127.0.0.1", "8080", &hints, &peer_address)) {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Remote address is: ");
    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,address_buffer, sizeof(address_buffer),service_buffer, sizeof(service_buffer),NI_NUMERICHOST);
    printf("%s %s\n", address_buffer, service_buffer); // getting the info of peer address (hostname,etc.)

    printf("Creating socket...\n");
    SOCKET socket_peer;
    socket_peer = socket(peer_address->ai_family,peer_address->ai_socktype, peer_address->ai_protocol); // creating peer socket
    if (!ISVALIDSOCKET(socket_peer)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Connecting...\n");
    if (connect(socket_peer,peer_address->ai_addr, peer_address->ai_addrlen)){ // connecting socket to the peer
        fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(peer_address); // frees the peer_address addrinfo structure

    printf("Connected.\n");
    printf("To send data, enter text followed by enter.\n");

    while(1) {
        fd_set reads; // file descriptor to read
        FD_ZERO(&reads); // clears the file descriptor
        FD_SET(socket_peer, &reads); // sets the bit for fd socket_peer in fd set reads
        FD_SET(0, &reads); // sets the bit for fd 0 in fd set reads

        struct timeval timeout; 
        timeout.tv_sec = 0; 
        timeout.tv_usec = 100000; // specifies in micro seconds the interval for which the select() must wait for fd

        if(select(socket_peer+1, &reads, 0, 0, &timeout) < 0){
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }
        
        // check whether socket_peer fd is still present in set
        if(FD_ISSET(socket_peer, &reads)){
            char read[4096];
            int bytes_received = recv(socket_peer, read, 4096, 0);
            if(bytes_received < 1){
                printf("Connection closed by peer.\n");
                break;
            }
            printf("Received from server (%d bytes): %.*s\n",bytes_received, bytes_received, read);
        }

        if(FD_ISSET(0, &reads)){
            char read[4096];
            if (!fgets(read, 4096, stdin)) break;
            int bytes_sent = send(socket_peer, read, strlen(read), 0);
        }
    }

    printf("Closing socket...\n");
    CLOSESOCKET(socket_peer);

    printf("Finished.\n");
    return 0;
}
