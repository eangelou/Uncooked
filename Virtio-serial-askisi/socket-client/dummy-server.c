/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/param.h>

#define MSG_SIZE 256
#define DEVICE "/dev/pts/3"

int
login_tty(int fd)
{
	(void) setsid();
	if (ioctl(fd, TIOCSCTTY, (char *)NULL) == -1)
		return (-1);
	(void) dup2(fd, 0);
	(void) dup2(fd, 1);
	(void) dup2(fd, 2);
	if (fd > 2)
		(void) close(fd);
	return (0);
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     // Socket global variables
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[MSG_SIZE];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
    
     // Terminal globals
    char *nl = "\n";
    int i = 0;
    
    // Open terminal
     int mymaster, myslave;
      char myname_slave[4096];

	mymaster = open("/dev/ptmx", O_RDWR | O_NONBLOCK);
	
	if (mymaster == -1) {
	  printf("Problem");
	}

	if (grantpt(mymaster))
	  printf("Problem");

	if (unlockpt(mymaster))
	  printf("Problem");

	n = ptsname_r(mymaster, myname_slave, sizeof(myname_slave));
	printf("openpty: slave name %s", myname_slave);
	myslave = open(myname_slave, O_RDWR | O_NOCTTY);
	if (myslave == -1)
	  {
	    printf("Problem");
	  }
	printf("slave fd = %d", myslave);
	
	login_tty(myslave);
     
     // Read ports 
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     
     // Open socket
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     // Listen and accept connections (max 5)
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");
     bzero(buffer,MSG_SIZE);
     // Process messages
     while (1) {
	bzero(buffer,MSG_SIZE);
	//Read from socket
	n = read(newsockfd,buffer,MSG_SIZE-1);
	if (n < 0) error("ERROR reading from socket");
	
	// ioctl everything to a tty 
	for (i = 0; buffer[i]; i++)
	  ioctl (myslave, TIOCSTI, buffer+i);

	// Assume \n?
	ioctl (myslave, TIOCSTI, "\n");
	
	// Log message
	printf("Here is the message: %s\n",buffer);
	
	// Read response
	n = read(mymaster,buffer,MSG_SIZE-1);
	if (n < 0) printf("ERROR reading from terminal");
	
	// Acknowledge message
// 	n = write(newsockfd,"I got your message",18);
// 	if (n < 0) error("ERROR writing to socket");
	
	// Print output
	n = write(newsockfd,buffer,MSG_SIZE-1);
	if (n < 0) error("ERROR writing to socket");
     }
     close(newsockfd);
     close(sockfd);
     return 0; 
}
 
