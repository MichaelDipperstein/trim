############################################################################
# Makefile for trim line trimmer and TAB removal program
############################################################################
CC = gcc
LD = gcc
CFLAGS = -I. -O3 -Wall -Wextra -pedantic -ansi -c
LDFLAGS = -O3 -o

# libraries
LIBS = -L optlist -loptlist

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

all:		trim$(EXE) optlist\liboptlist.a

trim$(EXE):	trim.o optlist\liboptlist.a
		$(LD) $< $(LIBS) $(LDFLAGS) $@

trim.o:		trim.c optlist/optlist.h
		$(CC) $(CFLAGS) $<

optlist\liboptlist.a:
		cd optlist && $(MAKE) liboptlist.a

clean:
		$(DEL) *.o
		$(DEL) trim$(EXE)
		cd optlist && $(MAKE) clean
