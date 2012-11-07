#ifndef _RUN_POPEN_H_
#define _RUN_POPEN_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

char *run_popen ( char *cmd, int * msgsize );

#endif