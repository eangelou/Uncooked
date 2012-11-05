#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#define MSG_SIZE 4


char * make_message ( int * msgsize, const char *fmt, ... )
{
	int n;
	int size = 1;     /* Guess we need no more than 100 bytes. */
	char *p, *np;
	va_list ap;

	if ( ( p = malloc ( size ) ) == NULL )
		return NULL;

	while ( 1 ) {

		/* Try to print in the allocated space. */
		va_start ( ap, fmt );
		n = vsnprintf ( p, size, fmt, ap );
		va_end ( ap );

		*msgsize = n; /* get original size*/

		/* If that worked, return the string. */
		if ( n > -1 && n < size )
			return p;

		/* Else try again with more space. */
		if ( n > -1 )  /* glibc 2.1 */
			size = n+1; /* precisely what is needed */
		else           /* glibc 2.0 */
			size *= 2;  /* twice the old size */

		if ( ( np = realloc ( p, size ) ) == NULL ) {
			free ( p );
			return NULL;
		} else {
			p = np;
		}
	}
}

char *run_popen ( char *cmd, int * msgsize )
{

	FILE *fp;
	int status;
	char path[MSG_SIZE];
	char * buffer;
	int last_write=0;

	/* Open the command for reading. */
	fp = popen ( cmd, "r" );
	if ( fp == NULL ) {
		printf ( "Failed to run command\n" );
		return NULL;
	}

	/* Read the output a line at a time - output it. */
	while ( fgets ( path, sizeof ( path )-1, fp ) != NULL ) {
		buffer = make_message ( msgsize,"%s%s",buffer,path );
	}

	/* close */
	pclose ( fp );

	return buffer;
}



