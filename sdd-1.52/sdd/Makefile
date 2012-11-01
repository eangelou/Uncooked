#ident @(#)Makefile	1.11 03/10/23 
###########################################################################
SRCROOT=	..
RULESDIR=	RULES
include		$(SRCROOT)/$(RULESDIR)/rules.top
###########################################################################

INSDIR=		bin
TARGET=		sdd
CPPOPTS +=	-DUSE_REMOTE
CPPOPTS +=	-DUSE_LARGEFILES
CFILES=		sdd.c
HFILES=		
#LIBS=		-lunos
LIBS=		-lrmt -lschily $(LIB_SOCKET)
XMK_FILE=	Makefile.man

###########################################################################
include		$(SRCROOT)/$(RULESDIR)/rules.cmd
###########################################################################

count:		$(CFILES) $(HFILES)
		count $r1

