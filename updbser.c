#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define MAXRECVSTRING 255  /* Longest string to receive */

void DieWithError(char *errorMessage)  /* External error handling function */
{
printf("%s",errorMessage);
}

int main(int argc, char *argv[])
{
    int sock;                         /* Socket */
    struct sockaddr_in broadcastAddr; /* Broadcast address */
    unsigned short broadcastPort;     /* Server port */
    char *sendString;                 /* String to broadcast */
    int broadcastPermission;          /* Socket opt to set permission to broadcast */
    unsigned int sendStringLen;       /* Length of string to broadcast */
    
    struct sockaddr_in targAddr; /* Broadcast address */
    unsigned short targPort;     /* Server port */
    char recvString[MAXRECVSTRING+1]; /* Buffer for received string */
    int recvStringLen;                /* Length of received string */

    if (argc < 4)                     /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage:  %s <Broadcast Port> <Client port> <Send String>\n", argv[0]);
        exit(1);
    }

    broadcastPort = atoi(argv[1]);    /* Second arg:  broadcast port */
    targPort = atoi(argv[2]);    /* Second arg:  broadcast port */
    sendString = argv[3];             /* Third arg:  string to broadcast */

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Set socket to allow broadcast */
    broadcastPermission = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, 
          sizeof(broadcastPermission)) < 0)
        DieWithError("setsockopt() failed");
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (void *) &broadcastPermission, 
          sizeof(broadcastPermission)) < 0)
        DieWithError("setsockopt() failed");
    
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    /* Construct local address structure */
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    broadcastAddr.sin_port = htons(broadcastPort);         /* Broadcast port */

    memset(&targAddr, 0, sizeof(targAddr));   /* Zero out structure */
    targAddr.sin_family = AF_INET;                 /* Internet address family */
    targAddr.sin_addr.s_addr = INADDR_BROADCAST;
    targAddr.sin_port = htons(targPort);         /* Broadcast port */

    /* Bind to the broadcast port */
    if (bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr)) < 0)
        DieWithError("bind() failed");

    sendStringLen = strlen(sendString);  /* Find length of sendString */

        /* Broadcast sendString in datagram to clients every 3 seconds*/
    while(1){    
        if (sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *) 
               &targAddr, sizeof(targAddr)) != sendStringLen)
            DieWithError("sendto() sent a different number of bytes than expected");

        sleep(3);
        memset(recvString, '\0', MAXRECVSTRING);
        if ((recvStringLen = recv(sock, recvString, MAXRECVSTRING, 0)) < 0)
            DieWithError("recvfrom() failed");
        printf("Received: %s\n", recvString);    /* Print the received string */
    }

    /* NOT REACHED */
}