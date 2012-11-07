/****************************************************************************
 * 
 * Crypto convenience classes
 * 
 ***************************************************************************/
#include "crypto.h"

int init_crypto_session(int cfd, __u8 my_key[KEY_SIZE], struct session_op * sess)
{
	memset ( sess, 0, sizeof ( struct session_op ) );
	
	/* Get crypto session for AES128 */
	(*sess).cipher = CRYPTO_AES_CBC;
	(*sess).keylen = KEY_SIZE;
	(*sess).key = my_key;
	if ( ioctl ( cfd, CIOCGSESSION, sess ) ) {
		perror ( "ioctl(CIOCGSESSION)" );
	}
	
	return 0;
}

int close_crypto_session(int cfd, struct session_op * sess)
{
	/* Finish crypto session */
	if ( ioctl ( cfd, CIOCFSESSION, &(*sess).ses ) ) {
		perror ( "ioctl(CIOCFSESSION)" );
		return 1;
	}

	return 0;
}

void encrypt_data ( int cfd, char * arr, struct session_op * sess, char * enc )
{
	struct {
		__u8	in[DATA_SIZE],
		     encrypted[DATA_SIZE],
		     decrypted[DATA_SIZE],
		     iv[BLOCK_SIZE],
		     key[KEY_SIZE];
	} data;
	struct crypt_op cryp;
	
	memset ( &cryp, 0, sizeof ( cryp ) );
	
	/* Use the garbage that is on the stack :-) */
	memset ( &data, 0, sizeof ( data ) );
	
	printf("arr: %s\n",arr);
	
	/* Copy value of arr to buff */
	strncpy(data.in, arr, sizeof(arr) / sizeof(*arr));
	
	/* Encrypt data.in to data.encrypted */
	cryp.ses = (*sess).ses;
	cryp.len = sizeof ( data.in );
	cryp.src = data.in;
	cryp.dst = data.encrypted;
	cryp.iv = data.iv;
	cryp.op = COP_ENCRYPT;
	if ( ioctl ( cfd, CIOCCRYPT, &cryp ) ) {
		perror ( "ioctl(CIOCCRYPT)" );
		return ;
	}
	
	strncpy(enc, data.encrypted , sizeof(data.encrypted) / sizeof(*data.encrypted));
	
	return;
	
}


void decrypt_data ( int cfd, char * enc, struct session_op * sess, char * arr )
{
	struct {
		__u8	in[DATA_SIZE],
		     encrypted[DATA_SIZE],
		     decrypted[DATA_SIZE],
		     iv[BLOCK_SIZE],
		     key[KEY_SIZE];
	} data;
	struct crypt_op cryp;
	
	memset ( &cryp, 0, sizeof ( cryp ) );
	
	/* Use the garbage that is on the stack :-) */
	memset ( &data, 0, sizeof ( data ) );
	
	/* Copy value of enc to buff */
		strncpy(data.encrypted, enc, sizeof(enc) / sizeof(*enc));
	
	/* Decrypt data.encrypted to data.decrypted */
	cryp.ses = (*sess).ses;
	cryp.len = sizeof ( arr );
	cryp.iv = data.iv;
	cryp.src = data.encrypted;
	cryp.dst = data.decrypted;
	cryp.op = COP_DECRYPT;
	if ( ioctl ( cfd, CIOCCRYPT, &cryp ) ) {
		perror ( "ioctl(CIOCCRYPT)" );
		return;
	}
	
	strncpy(arr, data.decrypted , sizeof(data.decrypted) / sizeof(*data.decrypted));

	return;
}