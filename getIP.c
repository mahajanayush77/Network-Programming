#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {

	char hostname[128];
	char ip[16];
	struct hostent* host;
	struct sockaddr_in sock_addr;
	
	gethostname(hostname, sizeof(hostname)); // retrieves hostname
	printf("hostname: %s", hostname); // prints the name of localhost of the machine

	host = gethostbyname(hostname); // to retrieve host info

	printf("\nip address list:\n");
	for (int i = 0; host->h_addr_list[i]; i++){
		sock_addr.sin_addr = *((struct in_addr*) host->h_addr_list[i]); // retrieves the internet addr of the socket
		inet_ntop(AF_INET, &sock_addr.sin_addr, ip, sizeof(ip)); // converts the sinternet addr of machine to a string
		printf("%s\n", ip);
	}

    // getservbyaddr
	struct servent *sptr = getservbyname("ftp", "tcp"); // retrieving info of a service with name "ftp" and protocol "tcp"

    if(!sptr){
    printf("\nNo entry found for this service.\n");
    }
	else{
    printf("\nservice name is : %s\n"
         "service port is : %d\n"
         "protocol used by service is : %s\n",
         sptr->s_name, sptr->s_port, sptr->s_proto); // printing info related to service
	}
    
	return 0;
}