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
	
	struct session_info_op siop;

	siop.ses = (*sess).ses;
	if (ioctl(cfd, CIOCGSESSINFO, &siop)) {
		perror("ioctl(CIOCGSESSINFO)");
		return -1;
	}
	printf("Got %s with driver %s\n",
			siop.cipher_info.cra_name, siop.cipher_info.cra_driver_name);
	if (!(siop.flags & SIOP_FLAG_KERNEL_DRIVER_ONLY)) {
		printf("Note: This is not an accelerated cipher\n");
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
		     encrypted[ENC_SIZE],
		     decrypted[DATA_SIZE],
		     iv[BLOCK_SIZE],
		     key[KEY_SIZE];
	} data;
	struct crypt_op cryp;
	
	memset ( &cryp, 0, sizeof ( cryp ) );
	
	/* Use the garbage that is on the stack :-) */
	memset ( &data, 0, sizeof ( data ) );
	
	/* Copy value of arr to buff */
	strcpy(data.in, arr);
	
	/* Encrypt data.in to data.encrypted */
	cryp.ses = (*sess).ses;
	cryp.len = DATA_SIZE;
	cryp.src = data.in;
	cryp.dst = data.encrypted;
	cryp.op = COP_ENCRYPT;
	if ( ioctl ( cfd, CIOCCRYPT, &cryp ) ) {
		perror ( "ioctl(COP_ENCRYPT)" );
		enc = NULL;
		return ;
	}
	
	strcpy(enc, data.encrypted);
	
	return;
	
}


void decrypt_data ( int cfd, char * enc, struct session_op * sess, char * arr )
{
	struct {
		__u8	in[DATA_SIZE],
		     encrypted[ENC_SIZE],
		     decrypted[DATA_SIZE],
		     iv[BLOCK_SIZE],
		     key[KEY_SIZE];
	} data;
	struct crypt_op cryp;
	
	memset ( &cryp, 0, sizeof ( cryp ) );
	
	/* Use the garbage that is on the stack :-) */
	memset ( &data, 0, sizeof ( data ) );
	
	/* Copy value of enc to buff */
	strcpy(data.encrypted, enc);
	
	/* Decrypt data.encrypted to data.decrypted */
	cryp.ses = (*sess).ses;
	cryp.len = sizeof ( data.encrypted );
	cryp.iv = data.iv;
	cryp.src = data.encrypted;
	cryp.dst = data.decrypted;
	cryp.op = COP_DECRYPT;
	if ( ioctl ( cfd, CIOCCRYPT, &cryp ) ) {
		perror ( "ioctl(COP_DECRYPT)" );
		arr = NULL;
		return;
	}
	
	strcpy(arr, data.decrypted);

	return;
}
