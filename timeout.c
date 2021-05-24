#include <stdio.h>   
#include <stdlib.h>     
#include <unistd.h> 
#include <errno.h> 
#include <string.h>
#include <signal.h>

// handler
static void handler(int sig){
    printf("Caught the signal!! \n"); 
}

int main(){
    struct sigaction sa; 
    char buf[100];
    int numRead ,savedErrno;

    // setting SIGALRM for interrupt handling
    sa.sa_flags = 0; // no special flag used
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    if (sigaction(SIGALRM, &sa, NULL) == -1)
        printf("sigaction");

    alarm(5); // timeout = 5 sec

    numRead = read(STDIN_FILENO, buf, 100); // stores num of bytes read from terminal

    savedErrno = errno;  // errno is caught
    alarm(0);  // turn off the timer
    errno = savedErrno;

    if (numRead == -1){ // nothing is read
        if (errno == EINTR) // EINTR = signal interrupted 
            printf("Read timed out\n");
        else
            printf("Read");
    } else
        printf("Successfuly read (%d bytes): %.*s",numRead, numRead, buf); // if there's no timeout

    exit(0);
}