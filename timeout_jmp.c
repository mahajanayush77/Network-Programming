#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h> 
#include <stdlib.h>

jmp_buf buffer; //  longjmp buffer

void timeout() {
    longjmp(buffer, 1); // go back to buffer and return 1
}

int main(){
    char buf[100]; // buffer to read
    int numRead;

    signal(SIGALRM, timeout); // setting system interrupt handler SIGALRM with longjmp

    if(setjmp(buffer) != 0){ // current jump position is not buffer
        printf("Read timed out\n");
        return -1;
    }
    alarm(5); // timeout = 5 sec

    numRead = read(STDIN_FILENO, buf, 100); // stores num of bytes read from terminal
    alarm(0);  // turn off the timer

    printf("Successfuly read (%d bytes): %.*s",numRead, numRead, buf); // if there's no timeout

    exit(0);
}
