#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int readLine(int sock, char *buffer, int bufferSize){
    int bytes_recieved;    // # of bytes fetched by read() 
    int total = 0;           // Total bytes read so far
    char *buf;

    if (bufferSize <= 0 || buffer == NULL){
        perror("Invalid value");
        errno = EINVAL;
        return -1;
    }
    buf = buffer;
    while(1){
        char ch;
        bytes_recieved = read(sock, &ch, 1); // reading a single byte from socket

        if(bytes_recieved < 0) 
                return -1;

        else if(bytes_recieved == 0){ 
            if (total == 0)     // No bytes read; return 0 
                return 0;
            else                // add '\0' 
                break;
        } 
        else{                        
            if(total < bufferSize-1){      // Discard > (n-1) bytes 
                total++;
                *buf++ = ch;
            }
            if(ch == '\n')
                break;
        }
    }
    *buf = '\0'; // null terminate string read
    return total;
}