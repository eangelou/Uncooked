/****************************************************************************
 * Copied from cryptodev_demos for convenience
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <crypto/cryptodev.h>

#define	DATA_SIZE	4096
#define	BLOCK_SIZE	16
#define	KEY_SIZE	32

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
		return 1;
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
	
	/* Copy value of arr to buff */
	uint i;
	for ( i=0; i<sizeof ( arr ); i++ ) {
		data.encrypted[i] = arr[i];
	}
	
	/* Decrypt data.encrypted to data.decrypted */
	cryp.ses = (*sess).ses;
	cryp.len = sizeof ( arr );
	cryp.iv = data.iv;
	cryp.src = data.encrypted;
	cryp.dst = data.decrypted;
	cryp.op = COP_DECRYPT;
	if ( ioctl ( cfd, CIOCCRYPT, &cryp ) ) {
		perror ( "ioctl(CIOCCRYPT)" );
		return 1;
	}
	
	return &data.decrypted;
}

static int
test_crypto ( int cfd )
{
	struct {
		__u8	in[DATA_SIZE],
		     encrypted[DATA_SIZE],
		     decrypted[DATA_SIZE],
		     iv[BLOCK_SIZE],
		     key[KEY_SIZE];
	} data;
	struct session_op sess;
	struct crypt_op cryp;

	memset ( &sess, 0, sizeof ( sess ) );
	memset ( &cryp, 0, sizeof ( cryp ) );

	/* Use the garbage that is on the stack :-) */
	memset ( &data, 0, sizeof ( data ) );

	char arr[4096];
	int ret = read ( 2,arr,sizeof ( arr ) );

// 	char * arr = "my test";
	// copy value of arr to buff
	uint i;
	for ( i=0; i<sizeof ( arr ); i++ ) {
		data.in[i] = arr[i];
	}

	/* Get crypto session for AES128 */
	sess.cipher = CRYPTO_AES_CBC;
	sess.keylen = KEY_SIZE;
	sess.key = data.key;
	if ( ioctl ( cfd, CIOCGSESSION, &sess ) ) {
		perror ( "ioctl(CIOCGSESSION)" );
		return 1;
	}

	struct session_info_op siop;

	siop.ses = sess.ses;
	if ( ioctl ( cfd, CIOCGSESSINFO, &siop ) ) {
		perror ( "ioctl(CIOCGSESSINFO)" );
		return -1;
	}
	printf ( "Got %s with driver %s\n",
	         siop.cipher_info.cra_name, siop.cipher_info.cra_driver_name );
	if ( ! ( siop.flags & SIOP_FLAG_KERNEL_DRIVER_ONLY ) ) {
		printf ( "Note: This is not an accelerated cipher\n" );
	}

	/* Encrypt data.in to data.encrypted */
	cryp.ses = sess.ses;
	cryp.len = sizeof ( data.in );
	cryp.src = data.in;
	cryp.dst = data.encrypted;
	cryp.iv = data.iv;
	cryp.op = COP_ENCRYPT;
	if ( ioctl ( cfd, CIOCCRYPT, &cryp ) ) {
		perror ( "ioctl(CIOCCRYPT)" );
		return 1;
	}

	fprintf ( stdout,"%s %s\n", "Test data when encrypted:", data.encrypted );

	/* Decrypt data.encrypted to data.decrypted */
	cryp.src = data.encrypted;
	cryp.dst = data.decrypted;
	cryp.op = COP_DECRYPT;
	if ( ioctl ( cfd, CIOCCRYPT, &cryp ) ) {
		perror ( "ioctl(CIOCCRYPT)" );
		return 1;
	}

	fprintf ( stdout,"%s %s\n", "Test data when decrypted:", data.decrypted );

	/* Verify the result */
	if ( memcmp ( data.in, data.decrypted, sizeof ( data.in ) ) != 0 ) {
		fprintf ( stderr,
		          "FAIL: Decrypted data are different from the input data.\n" );
		return 1;
	} else
		printf ( "Test passed\n" );

	/* Finish crypto session */
	if ( ioctl ( cfd, CIOCFSESSION, &sess.ses ) ) {
		perror ( "ioctl(CIOCFSESSION)" );
		return 1;
	}

	return 0;
}


int
full_test_main()
{
	int fd = -1, cfd = -1;

	/* Open the crypto device */
	fd = open ( "/dev/crypto", O_RDWR, 0 );
	if ( fd < 0 ) {
		perror ( "open(/dev/crypto)" );
		return 1;
	}

	/* Clone file descriptor */
	if ( ioctl ( fd, CRIOGET, &cfd ) ) {
		perror ( "ioctl(CRIOGET)" );
		return 1;
	}

	/* Set close-on-exec (not really neede here) */
	if ( fcntl ( cfd, F_SETFD, 1 ) == -1 ) {
		perror ( "fcntl(F_SETFD)" );
		return 1;
	}

	/* Run the test itself */
	if ( test_crypto ( cfd ) )
		return 1;

	/* Close cloned descriptor */
	if ( close ( cfd ) ) {
		perror ( "close(cfd)" );
		return 1;
	}

	/* Close the original descriptor */
	if ( close ( fd ) ) {
		perror ( "close(fd)" );
		return 1;
	}

	return 0;
}
