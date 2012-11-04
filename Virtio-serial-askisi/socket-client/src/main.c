#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define MSG_SIZE 4096

// Helper for perror
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

// The main client
int main(int argc, char *argv[])
{
    // Define global vars for sock
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    // The actual message buffer
    char buffer[MSG_SIZE];
    
    // Make sure we have hostname and port pair
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = TCP socket
    
    // Open the Internet socket
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    while (1){
	// Read the command
	printf("Please enter the command: ");
	bzero(buffer,MSG_SIZE);
	fgets(buffer,MSG_SIZE,stdin);
	
	// End client on End-of-Transmission
	if (buffer[0]=='\0') 
	  break;
	
	// Write to the socket
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0) 
	    error("ERROR writing to socket");
	
	
	// Read response
	n=1;
	while (n>0) {
	  n = read(sockfd,buffer,MSG_SIZE-1);
	  if (n < 0) 
	      error("ERROR reading from socket");
	  printf("%s\n",buffer);
	  if (n<=MSG_SIZE )
	    break;
	}
    }
    
    printf("%s\n", "Client closing...");
    
    // Close the socket and return (shoud loop here for a proper terminal)
    close(sockfd);
    return 0;
}
