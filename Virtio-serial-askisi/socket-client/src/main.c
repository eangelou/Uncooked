/****************************************************************************
 * 
 * Client implementation
 * 
 ***************************************************************************/

#include "crypto.h"
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
#include <sys/select.h>


// Helper for perror
void error(const char *msg) {

//     printf("Error: %d", errno);
	if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINPROGRESS)
		return;
	perror(msg);
	exit(EXIT_FAILURE);
}

// The main client
int main(int argc, char *argv[]) {
	// Crypto vars
	int cryptofd = -1, cfd = -1;

	/* Open the crypto device */
	cryptofd = open("/dev/crypto", O_RDWR, 0);
	if (cryptofd < 0) {
		perror("open(/dev/crypto)");
		return 1;
	}

	/* Clone file descriptor */
	if (ioctl(cryptofd, CRIOGET, &cfd)) {
		perror("ioctl(CRIOGET)");
		return 1;
	}

	/* Set close-on-exec (not really neede here) */
	if (fcntl(cfd, F_SETFD, 1) == -1) {
		perror("fcntl(F_SETFD)");
		return 1;
	}

	// Define crypto vars
	__u8 my_key[KEY_SIZE] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6,
			7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1 };
	struct session_op * crypto_session;
	crypto_session = malloc(sizeof(struct session_op));
	char * fgets_data;

	// initialize crypto session
	init_crypto_session(cfd, my_key, crypto_session);

	// our protocol's EOT for each message
	int i = 0;
	char * eot = malloc(5 * sizeof(char));
	eot[0] = '\0';
	for (i = 1; i < 4; i++) {
		eot[i] = '.';
	}
	eot[4] = '\0';

	// Define global vars for sock
	int sockfd, portno, n, k;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	// The input buffer
	char in_buffer[ENC_SIZE];

	// The actual message buffer
	char in_data[MSG_SIZE];

	// The output buffer
	char out_buffer[ENC_SIZE];

	// The actual message buffer
	char out_data[MSG_SIZE];

	// The actual message buffer size
	int * msgsize;
	msgsize = malloc(sizeof(int));

	// Make sure we have hostname and port pair
	if (argc < 3 || argc > 3) {
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(0);
	}
	portno = atoi(argv[2]);

	while (1) {
		// Read the command
		printf("Please enter the command: ");
		bzero(out_buffer, ENC_SIZE);
		bzero(out_data, MSG_SIZE);
		bzero(in_buffer, ENC_SIZE);
		bzero(in_data, MSG_SIZE);

		fgets_data = fgets(in_data, MSG_SIZE, stdin);
		if (fgets_data == NULL )
			break; // End client on terminal End-of-Transmission

		// Connect to socket for transmission
		sockfd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = TCP socket

		// Open the Internet socket
		if (sockfd < 0)
			error("ERROR opening socket");
		server = gethostbyname(argv[1]);
		if (server == NULL ) {
			fprintf(stderr, "ERROR, no such host\n");
			exit(0);
		}
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *) server->h_addr_list[0], (char *) &serv_addr.sin_addr.s_addr,
				server->h_length);
		serv_addr.sin_port = htons(portno);
		if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))
				< 0)
			error("ERROR connecting");

		// Test encrypt/decrypt with cryptodev
		encrypt_data(cfd, in_data, crypto_session, in_buffer);

		// Write to the socket
		n = write(sockfd, in_buffer, ENC_SIZE);
		if (n < 0)
			error("ERROR writing to socket");

		// Read response
		char * resp;
		n = 1;
		while (n > 0) {
			n = read(sockfd, out_buffer, ENC_SIZE);
			if (n < 0)
				error("ERROR reading from socket");

//			char * cut = malloc(strlen(out_buffer) * sizeof(char));

			// Check for EOT (as defined by our protocol - 3 null-terminated dots)
			if (out_buffer[n - 5] == '\0' && out_buffer[n - 4] == '.'
					&& out_buffer[n - 3] == '.' && out_buffer[n - 2] == '.'
					&& out_buffer[n - 1] == '\0') {

//				memcpy( cut, out_buffer, (strlen(out_buffer) * sizeof(char))-5);

//				decrypt_data(cfd, out_buffer, crypto_session, out_data);
//
//				printf("SECOND: \n%s\n", out_data);

				break;
			}

//			strcpy(cut, out_buffer);

			decrypt_data(cfd, out_buffer, crypto_session, out_data);

			printf("FIRST: %s\n", out_data);

//			bzero(cut, strlen(out_buffer) * sizeof(char));
			bzero(out_buffer, ENC_SIZE);
			bzero(out_data, MSG_SIZE);

			fflush(stdout);
		}
		// Close the socket 
		close(sockfd);

	}

	printf("%s\n", "Client closing...");

	// Close the crypto session
	n = close_crypto_session(cfd, crypto_session);
	if (n < 0)
		error("Problem closing cryptodev:");

	/* Close cloned descriptor */
	if (close(cfd)) {
		perror("close(cfd)");
		return 1;
	}

	/* Close the original descriptor */
	if (close(cryptofd)) {
		perror("close(fd)");
		return 1;
	}

	return 0;
}

