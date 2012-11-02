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
#include <sys/stream.h>
#include <sys/stropts.h>

int forkpty (int *amaster, char *name, void *unused1, void *unused2)
{
    int master, slave;
    char *slave_name;
    pid_t pid;

    master = open( "/dev/ptmx", O_RDWR );
    if ( master < 0 )
        return -1;

    if ( grantpt( master ) < 0 )
    {
        close( master );
        return -1;
    }

    if ( unlockpt( master ) < 0 )
    {
        close( master );
        return -1;
    }

    slave_name = ptsname( master );
    if ( slave_name == NULL )
    {
        close( master );
        return -1;
    }

    slave = open( slave_name, O_RDWR );
    if (slave < 0)
    {
        close( master );
        return -1;
    }

    if (ioctl( slave, I_PUSH, "ptem" )   < 0
    ||  ioctl( slave, I_PUSH, "ldterm" ) < 0)
    {
        close( slave );
        close( master );
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
        case  0:      /* Child */
            close (master);
            dup2 (slave, STDIN_FILENO);
            dup2 (slave, STDOUT_FILENO);
            dup2 (slave, STDERR_FILENO);
            return 0;
        default:      /* Parent */
            close (slave);
            return pid;
    }

    return -1;
}
