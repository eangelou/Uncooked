#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <crypto/cryptodev.h>


#define	DATA_SIZE	768
#define	BLOCK_SIZE	16
#define	KEY_SIZE	32


int init_crypto_session(int cfd, __u8 my_key[KEY_SIZE], struct session_op * sess);
int close_crypto_session(int cfd, struct session_op * sess);
void encrypt_data ( int cfd, char * arr, struct session_op * sess, char * enc );
void decrypt_data ( int cfd, char * enc, struct session_op * sess, char * arr );

#endif
