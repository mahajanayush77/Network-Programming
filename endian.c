#include <stdio.h>

int main(){
    unsigned int i =1;
    char *c = (char*) &i; // size of char = 1 byte, so it stores only first byte of integer i

    if(*c){
        printf("Little Endian \n"); // last byte is stored first *c = 1
    }
    else{
        printf("Big Endian \n"); // first byte is stored first *c = 0
    }

    return 0;
}