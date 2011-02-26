############################################################################
# Makefile for trim line trimmer and TAB removal program
#
#   $Id: Makefile,v 1.3 2007/09/30 04:52:11 michael Exp $
#   $Log: Makefile,v $
#   Revision 1.3  2007/09/30 04:52:11  michael
#   Replace getopt with optlist.
#
#   Revision 1.2  2007/07/16 02:28:43  michael
#   Use -pedantic option when compiling.
#
#   Revision 1.1.1.1  2006/12/31 06:14:58  michael
#   Initial Release
#
#
############################################################################
CC = gcc
LD = gcc
CFLAGS = -I. -O3 -Wall -Wextra -pedantic -ansi -c
LDFLAGS = -O3 -o

# libraries
LIBS = -L. -loptlist

# Treat NT and non-NT windows the same
ifeq ($(OS),Windows_NT)
	OS = Windows
endif

ifeq ($(OS),Windows)
	EXE = .exe
	DEL = del
else	#assume Linux/Unix
	EXE =
	DEL = rm
endif

all:		trim$(EXE) liboptlist.a

trim$(EXE):	trim.o liboptlist.a
		$(LD) $< $(LIBS) $(LDFLAGS) $@

trim.o:		trim.c optlist.h
		$(CC) $(CFLAGS) $<

liboptlist.a:	optlist.o
		ar crv liboptlist.a optlist.o
		ranlib liboptlist.a

optlist.o:	optlist.c optlist.h
		$(CC) $(CFLAGS) $<

clean:
		$(DEL) *.o
		$(DEL) *.a
		$(DEL) trim$(EXE)
