#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <crypto/cryptodev.h>

#define	KEY_SIZE	32
#define MSG_SIZE 4096

// Helper for perror
void error ( const char *msg )
{

//     printf("Error: %d", errno);
	if ( errno == EAGAIN || errno == EWOULDBLOCK || errno==EINPROGRESS )
		return;
	perror ( msg );
	exit ( EXIT_FAILURE );
}

int non_block ( int fd )
{
	// Set socket read non-blocking
	// timeout structure passed into select
	struct timeval tv;
	// fd_set passed into select
	fd_set fds;
	// Set up the timeout.  here we can wait for 1 second
	tv.tv_sec = 0;
	tv.tv_usec = 10;

	// Zero out the fd_set - make sure it's pristine
	FD_ZERO ( &fds );
	// Set the FD that we want to read
	FD_SET ( fd, &fds );
	// select takes the last file descriptor value + 1 in the fdset to check,
	// the fdset for reads, writes, and errors.  We are only passing in reads.
	// the last parameter is the timeout.  select will return if an FD is ready or
	// the timeout has occurred
	select ( 1, &fds, NULL, NULL, &tv );
	// return 0 if sock is not ready to be read.
	return FD_ISSET ( fd, &fds );
}

// The main client
int main ( int argc, char *argv[] )
{
	// Crypto vars
	int cryptofd = -1, cfd = -1;
	
	/* Open the crypto device */
	cryptofd = open ( "/dev/crypto", O_RDWR, 0 );
	if ( cryptofd < 0 ) {
		perror ( "open(/dev/crypto)" );
		return 1;
	}

	/* Clone file descriptor */
	if ( ioctl ( cryptofd, CRIOGET, &cfd ) ) {
		perror ( "ioctl(CRIOGET)" );
		return 1;
	}

	/* Set close-on-exec (not really neede here) */
	if ( fcntl ( cfd, F_SETFD, 1 ) == -1 ) {
		perror ( "fcntl(F_SETFD)" );
		return 1;
	}
	
	// Define crypto vars
	__u8 my_key[KEY_SIZE] = {0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1};
	struct session_op * crypto_session;
	crypto_session = malloc(sizeof(struct session_op));
	char * in_data = malloc( MSG_SIZE);
	char * out_data = malloc( MSG_SIZE);
	
	// initialize crypto session
	
	printf("Sess: %d", (*crypto_session).ses);
	
	init_crypto_session(cfd, my_key, crypto_session); 
	
	printf("Sess: %d", (*crypto_session).ses);
	
	// Define global vars for sock
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	// The input buffer
	char in_buffer[MSG_SIZE];

	// The actual message buffer
	char buffer[MSG_SIZE];

	// Make sure we have hostname and port pair
	if ( argc < 3 ) {
		fprintf ( stderr,"usage %s hostname port\n", argv[0] );
		exit ( 0 );
	}
	portno = atoi ( argv[2] );

	while ( 1 ) {
		// Read the command
		printf ( "Please enter the command: " );
		bzero ( buffer,MSG_SIZE );
		bzero ( in_buffer,MSG_SIZE );
		fgets ( in_buffer,MSG_SIZE,stdin );

		// End client on End-of-Transmission
		if ( in_buffer[0]=='\0' )
			break;

		// Connect to socket for transmission
		sockfd = socket ( AF_INET, SOCK_STREAM, 0 ); // AF_INET = TCP socket

		// Open the Internet socket
		if ( sockfd < 0 )
			error ( "ERROR opening socket" );
		server = gethostbyname ( argv[1] );
		if ( server == NULL ) {
			fprintf ( stderr,"ERROR, no such host\n" );
			exit ( 0 );
		}
		bzero ( ( char * ) &serv_addr, sizeof ( serv_addr ) );
		serv_addr.sin_family = AF_INET;
		bcopy ( ( char * ) server->h_addr,
		        ( char * ) &serv_addr.sin_addr.s_addr,
		        server->h_length );
		serv_addr.sin_port = htons ( portno );
		if ( connect ( sockfd, ( struct sockaddr * ) &serv_addr,sizeof ( serv_addr ) ) < 0 )
			error ( "ERROR connecting" );

		encrypt_data (cfd, in_buffer, crypto_session, in_data );
			
		printf("Encrypted: %s",in_data );
		
		// Write to the socket
		n = write ( sockfd,in_data, sizeof(in_data) );
		if ( n < 0 )
			error ( "ERROR writing to socket" );


		// Read response
		n=1;
		while ( n>0 ) {
			n = read ( sockfd,buffer,MSG_SIZE );
			if ( n < 0 )
				error ( "ERROR reading from socket" );

			// Check for EOT (as defined by our protocol - 3 null-terminated dots)
			if ( buffer[n-5] == '\0' && buffer[n-4] == '.' && buffer[n-3] == '.' && buffer[n-2] == '.' && buffer[n-1] == '\0' ) {
				printf ( "%s",buffer );
				break;
			}

			printf ( "%s",buffer );
			fflush ( stdout );
		}

		close ( sockfd );

	}

	printf ( "%s\n", "Client closing..." );

	// Close the socket 
	close ( sockfd );
	
	// Close the crypto session
	n = close_crypto_session(cfd, crypto_session);
	if (n<0) error("Problem closing cryptodev:");
	
	/* Close cloned descriptor */
	if ( close ( cfd ) ) {
		perror ( "close(cfd)" );
		return 1;
	}

	/* Close the original descriptor */
	if ( close ( cryptofd ) ) {
		perror ( "close(fd)" );
		return 1;
	}
	
	return 0;
}


