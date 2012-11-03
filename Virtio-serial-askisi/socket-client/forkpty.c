/*
 * forkpty() replacement for Solaris.
 * I don't know who originally wrote this, but it is needed to
 * compile tintin++ on Solaris 10 by system.c
 *
 * Add a line for "forkpty.o" in Makefile and add an extern
 * int forkpty ( ... ) declaration to the top of system.c
 * to compile tt++ cleanly on Solaris using GCC 3.4.
 *
 * 7/13/08 gkl
 */


#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
// #include <sys/stream.h>
#include <sys/stropts.h>

int forkpty (int *amaster, char *name, void *unused1, void *unused2)
{
    int master, slave;
    char *slave_name;
    pid_t pid;
    
    setsid();

    master = open("/dev/ptmx", O_RDWR|O_NOCTTY);
//     master = posix_openpt(O_RDWR);
    if ( master < 0 ){
	printf("master error");
	perror("forkpty error1");
        return -1;
    }

    printf("master: %d\n",master);
    if ( grantpt( master ) < 0 )
    {
        close( master );
	perror("forkpty error2");
        return -1;
    }

    if ( unlockpt( master ) < 0 )
    {
        close( master );
	perror("forkpty error3");
        return -1;
    }
    
    slave_name = ptsname( master );
    if ( slave_name == NULL )
    {
        close( master );
	perror("forkpty error4");
        return -1;
    }

    slave = open( slave_name, O_RDWR );
    if (slave < 0)
    {
        close( master );
	perror("forkpty error5");
        return -1;
    }
    printf("slave: %d\n",slave);

    if (ioctl( slave, I_PUSH, "ptem" )   < 0
    ||  ioctl( slave, I_PUSH, "ldterm" ) < 0)
    {
        close( slave );
        close( master );
	perror("forkpty error6");
        return -1;
    }

    if (amaster)
        *amaster = master;

    if (name)
        strcpy (name, slave_name);
    
    pid = fork ();
    switch (pid)
    {
        case -1:      /* Error */
            return -1;
	    perror("forkpty error7");
        case  0:      /* Child */
            close (master);
            dup2 (slave, STDIN_FILENO);
            dup2 (slave, STDOUT_FILENO);
            dup2 (slave, STDERR_FILENO);
            return 0;
        default:      /* Parent */
            close (slave);
	    perror("forkpty error8");
            return pid;
    }

    return -1;
}

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
