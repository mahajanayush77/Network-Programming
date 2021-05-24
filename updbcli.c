#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define MAXRECVSTRING 255  /* Longest string to receive */

void DieWithError(char *errorMessage) /* External error handling function */
{
printf("%s",errorMessage);
}

int main(int argc, char *argv[])
{
    int sock;                         /* Socket */
    struct sockaddr_in broadcastAddr; /* Broadcast Address */
    unsigned short broadcastPort;     /* Server port */
    char recvString[MAXRECVSTRING+1]; /* Buffer for received string */
    int recvStringLen;                /* Length of received string */

    struct sockaddr_in targAddr; /* Broadcast address */
    unsigned short targPort;     /* Server port */
    char *message;                 /* String to broadcast */
    int broadcastPermission;          /* Socket opt to set permission to broadcast */
    unsigned int messageLen;       /* Length of string to broadcast */
    
    if (argc < 4)    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <Broadcast Port> <Server Port> <Message to broadcast>\n", argv[0]);
        exit(1);
    }

    broadcastPort = atoi(argv[1]);   /* First arg: broadcast port */
    targPort = atoi(argv[2]);   /* First arg: broadcast port */
    message = argv[3];            /* Second arg:  string to broadcast */

    messageLen = strlen(message);

    /* Create a best-effort datagram socket using UDP */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Set socket to allow broadcast */
    broadcastPermission = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, 
          sizeof(broadcastPermission)) < 0)
        DieWithError("setsockopt() failed");
    
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *) &broadcastPermission, 
          sizeof(broadcastPermission)) < 0)
        DieWithError("setsockopt() failed");

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (void *) &broadcastPermission, 
          sizeof(broadcastPermission)) < 0)
        DieWithError("setsockopt() failed");

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    /* Construct bind structure */
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);  /* Any incoming interface */
    broadcastAddr.sin_port = htons(broadcastPort);      /* Broadcast port */

    memset(&targAddr, 0, sizeof(targAddr));   /* Zero out structure */
    targAddr.sin_family = AF_INET;                 /* Internet address family */
    targAddr.sin_addr.s_addr = INADDR_BROADCAST;
    targAddr.sin_port = htons(targPort);         /* Broadcast port */

    /* Bind to the broadcast port */
    if (bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr)) < 0)
        DieWithError("bind() failed");

    if ((recvStringLen = recvfrom(sock, recvString, MAXRECVSTRING, 0, NULL, 0)) < 0)
    DieWithError("recvfrom() failed");

    recvString[recvStringLen] = '\0';
    printf("Received: %s\n", recvString);    /* Print the received string */

    while(1){
        
        memset(recvString, '\0', MAXRECVSTRING);
        if ((recvStringLen = recv(sock, recvString, MAXRECVSTRING, 0)) < 0)
        DieWithError("recvfrom() failed");
        printf("Received: %s\n", recvString);    /* Print the received string */

            /* Broadcast message in datagram to clients every 3 seconds*/
        if (sendto(sock, message, messageLen, 0, (struct sockaddr *) 
               &targAddr, sizeof(targAddr)) != messageLen)
            DieWithError("sendto() sent a different number of bytes than expected");
        sleep(3);
    }
    
    close(sock);
    exit(0);
}