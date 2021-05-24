#include<stdio.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include<stdlib.h>
void str_echo(int s)
{
char buf[50];
//receiving data from client
recv(s,buf,50,0);
puts("Message from Client...");
fputs(buf,stdout);
send(s,buf,50,0);
}
int main()
{
 int ls,cs,len,maxfd;
 fd_set	ready_sockets, current_sockets;
 struct sockaddr_in serv,cli;
 puts("I am Server...");
//creating socket
 ls=socket(AF_INET,SOCK_STREAM,0);
 puts("Socket Created Successfully...");
//socket address structure
 serv.sin_family=AF_INET;
 serv.sin_addr.s_addr=INADDR_ANY;
 serv.sin_port=htons(5000);
 bind(ls,(struct sockaddr*)&serv,sizeof(serv));
 puts("Binding Done...");
 listen(ls,3);
 puts("Listening for Client...");
 
 maxfd = ls;
 FD_ZERO(&current_sockets);
 FD_SET(ls,&current_sockets); 
 while(1)
 {
    ready_sockets = current_sockets;		/* structure assignment */
if(select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL)<0){
        perror("Error occured");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < FD_SETSIZE; i++)
    {
    if(FD_ISSET(i,&ready_sockets)){
        puts("ready to connect");
	    if(i==ls){
            len=sizeof(cli);
            //accepting client connection
            cs=accept(ls,(struct sockaddr*)&cli,&len);
           FD_SET(cs,&current_sockets); 
            puts("\nConnected to Client...");
        str_echo(cs);
        
//close(ls);
        } else{
	str_echo(cs);
	FD_CLR(i,&current_sockets);
}
    }

}

//close(cs);
 }
 return 0;
}