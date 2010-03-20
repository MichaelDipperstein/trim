############################################################################
# Makefile for trim line trimmer and TAB removal program
#
#   $Id: Makefile,v 1.1.1.1 2006/12/31 06:14:58 michael Exp $
#   $Log: Makefile,v $
#   Revision 1.1.1.1  2006/12/31 06:14:58  michael
#   Initial Release
#
#
############################################################################
CC = gcc
LD = gcc
CFLAGS = -I. -O3 -Wall -ansi -c
LDFLAGS = -O3 -o

# libraries
LIBS = -L. -lgetopt

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

all:		trim$(EXE) libgetopt.a

trim$(EXE):	trim.o libgetopt.a
		$(LD) $< $(LIBS) $(LDFLAGS) $@

trim.o:		trim.c getopt.h
		$(CC) $(CFLAGS) $<

libgetopt.a:	getopt.o
		ar crv libgetopt.a getopt.o
		ranlib libgetopt.a

getopt.o:	getopt.c getopt.h
		$(CC) $(CFLAGS) $<

clean:
		$(DEL) *.o
		$(DEL) *.a
		$(DEL) trim$(EXE)
