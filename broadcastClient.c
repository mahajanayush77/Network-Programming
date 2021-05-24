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
    struct sockaddr_in targetAddr; /* Broadcast address */
    unsigned short broadcastPort;     /* Server port */
    unsigned short targetPort;     /* Server port */
    char recvString[MAXRECVSTRING+1]; /* Buffer for received string */
    int recvStringLen;                /* Length of received string */

    char *sendString;                 /* String to broadcast */
    int broadcastPermission;          /* Socket opt to set permission to broadcast */
    unsigned int sendStringLen;       /* Length of string to broadcast */
    
    if (argc < 4)    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <Broadcast Port> <Server Port> <message>\n", argv[0]);
        exit(1);
    }

    broadcastPort = atoi(argv[1]);   /* First arg: broadcast port */
    targetPort = atoi(argv[2]);   /* First arg: broadcast port */
    sendString = argv[3];            /* Second arg:  string to broadcast */

    sendStringLen = strlen(sendString);

    /* Create a best-effort datagram socket using UDP */
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
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    /* Construct bind structure */
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);  /* Any incoming interface */
    broadcastAddr.sin_port = htons(broadcastPort);      /* Broadcast port */

    memset(&targetAddr, 0, sizeof(targetAddr));   /* Zero out structure */
    targetAddr.sin_family = AF_INET;                 /* Internet address family */
    targetAddr.sin_addr.s_addr = INADDR_BROADCAST;
    targetAddr.sin_port = htons(targetPort);         /* Broadcast port */

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

            /* Broadcast sendString in datagram to clients every 3 seconds*/
        if (sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *) 
               &targetAddr, sizeof(targetAddr)) != sendStringLen)
            DieWithError("sendto() sent a different number of bytes than expected");
        sleep(3);
    }
    
    close(sock);
    exit(0);
}