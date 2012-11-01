/*
 * Demo on how to use OpenBSD /dev/crypto device.
 *
 * Author: Michal Ludvig <michal@logix.cz>
 *         http://www.logix.cz/michal
 *
 * Note: by default OpenBSD doesn't allow using
 *       /dev/crypto if there is no hardware accelerator
 *       for a given algorithm. To change this you'll have to
 *       set cryptodevallowsoft=1 in 
 *       /usr/src/sys/crypto/cryptodev.c and rebuild your kernel.
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <crypto/cryptodev.h>

#define	DATA_SIZE	4096
#define	BLOCK_SIZE	16
#define	KEY_SIZE	32
static int
test_crypto(int cfd)
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

	memset(&sess, 0, sizeof(sess));
	memset(&cryp, 0, sizeof(cryp));

	/* Use the garbage that is on the stack :-) */
	memset(&data, 0, sizeof(data)); 
	
	char arr[4096];
	int ret = read(2,arr,sizeof(arr));
	
// 	char * arr = "my test"; 
	// copy value of arr to buff
	for (uint i=0; i<sizeof(arr); i++)
	{
	  data.in[i] = arr[i];
	}

	/* Get crypto session for AES128 */
	sess.cipher = CRYPTO_AES_CBC;
	sess.keylen = KEY_SIZE;
	sess.key = data.key;
	if (ioctl(cfd, CIOCGSESSION, &sess)) {
		perror("ioctl(CIOCGSESSION)");
		return 1;
	}
	
	struct session_info_op siop;
	
	siop.ses = sess.ses;
	if (ioctl(cfd, CIOCGSESSINFO, &siop)) {
		perror("ioctl(CIOCGSESSINFO)");
		return -1;
	}
	printf("Got %s with driver %s\n",
			siop.cipher_info.cra_name, siop.cipher_info.cra_driver_name);
	if (!(siop.flags & SIOP_FLAG_KERNEL_DRIVER_ONLY)) {
		printf("Note: This is not an accelerated cipher\n");
	}

	/* Encrypt data.in to data.encrypted */
	cryp.ses = sess.ses;
	cryp.len = sizeof(data.in);
	cryp.src = data.in;
	cryp.dst = data.encrypted;
	cryp.iv = data.iv;
	cryp.op = COP_ENCRYPT;
	if (ioctl(cfd, CIOCCRYPT, &cryp)) {
		perror("ioctl(CIOCCRYPT)");
		return 1;
	}
	
	fprintf(stdout,"%s %s\n", "Test data when encrypted:", data.encrypted);
	
	/* Decrypt data.encrypted to data.decrypted */
	cryp.src = data.encrypted;
	cryp.dst = data.decrypted;
	cryp.op = COP_DECRYPT;
	if (ioctl(cfd, CIOCCRYPT, &cryp)) {
		perror("ioctl(CIOCCRYPT)");
		return 1;
	}

	fprintf(stdout,"%s %s\n", "Test data when decrypted:", data.decrypted);
	
	/* Verify the result */
	if (memcmp(data.in, data.decrypted, sizeof(data.in)) != 0) {
		fprintf(stderr,
			"FAIL: Decrypted data are different from the input data.\n");
		return 1;
	} else
		printf("Test passed\n");

	/* Finish crypto session */
	if (ioctl(cfd, CIOCFSESSION, &sess.ses)) {
		perror("ioctl(CIOCFSESSION)");
		return 1;
	}

	return 0;
}

int
main()
{
	int fd = -1, cfd = -1;

	/* Open the crypto device */
	fd = open("/dev/crypto", O_RDWR, 0);
	if (fd < 0) {
		perror("open(/dev/crypto)");
		return 1;
	}

	/* Clone file descriptor */
	if (ioctl(fd, CRIOGET, &cfd)) {
		perror("ioctl(CRIOGET)");
		return 1;
	}

	/* Set close-on-exec (not really neede here) */
	if (fcntl(cfd, F_SETFD, 1) == -1) {
		perror("fcntl(F_SETFD)");
		return 1;
	}

	/* Run the test itself */
	if (test_crypto(cfd))
		return 1;

	/* Close cloned descriptor */
	if (close(cfd)) {
		perror("close(cfd)");
		return 1;
	}

	/* Close the original descriptor */
	if (close(fd)) {
		perror("close(fd)");
		return 1;
	}

	return 0;
}
