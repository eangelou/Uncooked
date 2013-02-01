/* 
 * A simple server in the internet domain using TCP
 *   The port number is passed as an argument 
 */

#include "run-popen.h"
#include "crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <crypto/cryptodev.h>

// perror helper
void error(const char *msg) {
	perror(msg);
	exit(1);
}

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

	// Socket global variables
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char * out_data = malloc(MSG_SIZE);
	char out_buffer [ENC_SIZE];
	char in_data[MSG_SIZE];
	char in_buffer[ENC_SIZE];

	struct sockaddr_in serv_addr, cli_addr;
	int n, k;
	int i = 0;

	// our protocol's EOT for each message
	char * eot = malloc(5 * sizeof(char));
	eot[0] = '\0';
	for (i = 1; i < 4; i++) {
		eot[i] = '.';
	}
	eot[4] = '\0';

	// Read ports
	if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
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
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	// Listen and accept connections (max 5)
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	// Process messages
	while (1) {

		//
		int * msgsize;
		msgsize = malloc(sizeof(int));

		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
			error("ERROR on accept");

		in_buffer[0] = '\n';
		while (in_buffer[0] != '\0') {
			bzero(in_buffer, ENC_SIZE);
			bzero(in_data, MSG_SIZE);
			bzero(out_buffer, ENC_SIZE);
			bzero(out_data, strlen(out_data));

			//Read from socket
			n = 1;
			n = read(newsockfd, in_buffer, ENC_SIZE);
			if (n < 0)
				error("ERROR reading from socket");

			decrypt_data(cfd, in_buffer, crypto_session, in_data);

			if (in_data[0] == '\0' || in_buffer[0] == '\0')
				break;

			// Run command and get output
			if ((in_buffer[0] == '\n' && in_buffer[1] == '\0')
					|| (in_data[0] == '\n' && in_data[1] == '\0')) {
				sprintf(out_data, "No Command!\n");
				*msgsize = strlen(out_data) * sizeof(char);
				printf("No Command!\n");
			} else {
				out_data = run_popen(in_data, msgsize);
			}

			printf("Output: %s\n", out_data);


				// Test encrypt data before transmitting
				encrypt_data(cfd, out_data, crypto_session, out_buffer);

				// Send output
				int i = 0;
				while (n > 0) {
					n = write(newsockfd, out_buffer + i, ENC_SIZE - i);
					if (n < 0)
						error("ERROR writing to socket");
					if (n >= ENC_SIZE + i) {
						// Send EOT for us
						k = write(newsockfd, eot, 5 * sizeof(char));
						if (k < 0)
							error("ERROR writing to socket");
						break;
					} else
						i += n;
				}

				// Test decrypt data transmitted
				decrypt_data(cfd, out_buffer, crypto_session, in_data);
				printf(" Decrypted: %s\n", in_data);

		}
		close(newsockfd);

	}

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

	close(sockfd);
	return 0;
}

