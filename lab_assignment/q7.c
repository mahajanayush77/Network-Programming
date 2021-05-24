#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#define RECV_FAILED -1

int crlf(int s, char *buffer,int bufferSize, int *bytesReceived){
		// s is socket descriptor
        // *buffer is buffer to store received msg 
        // *bytesReceived is size of the received msg

        *bytesReceived = recv(s,buffer,bufferSize,0); // socket system call to receive message

        if(*bytesReceived < 0){ 
            perror("Unable to receive data"); // print error to stderr
            return (RECV_FAILED); 
        }

        int i = 2;
        printf(" 1st Line : ");
        for (char *p = buffer;*p; p++){
         if (*p != '\r') 
            putchar(*p);
         else
            printf("\n %d Line : ",i++);
        }

        return *bytesReceived; // return the bytes recieved
}