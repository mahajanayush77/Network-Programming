#include<stdio.h>
#include<stdlib.h>
#include<netdb.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>
#include<arpa/inet.h> 

int main(int argc,char *argv[]){
    struct hostent *he;
    if(argc<2){
        printf("usage : ./a.out <hostname>\n");
        exit(EXIT_FAILURE);
    }
    he = gethostbyname(argv[1]); 
    if(he==NULL){
        perror("gethostbyname error");
        exit(EXIT_FAILURE);
    }
   
    char *IP;
    IP = inet_ntoa(*((struct in_addr*)he->h_addr_list[0]));
    printf("Official name : %s\n",he->h_name);
    printf("Host Address Type (2 for AF_INET and 10 for AF_INET6) : %d\n",he->h_addrtype);
    printf("Length of Address in bytes : %d\n",he->h_length);
    printf("Host Address : %s\n",IP);
    return 0;
}
