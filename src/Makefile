# CSC 552 Makefile
#  Author: xing yuan
#  Date: 10 Feb 2019
#  LFS project architecture
#  log layer :  
#  File layer :
#  Directory layer ""

.PHONY: depend clean backup setup

MAIN=LFS

LFS=..

CC=g++

CFLAGS= -DUNIX -Wall -g

INCLUDES = -I${LFS}/include 

LFLAGS= -L${LFS}LFS/lib

SRCS = hello.C flash.c

OBJS = $(SRCS:.C=.o)

$(MAIN):  $(OBJS)
	 $(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(MAIN) $(LFLAGS)

.C.o:
	$(CC) $(CFLAGS) $(INCLUDES) $(LFLAGS) -c $<

clean:
	rm -f *.o *~ $(MAIN) $(MAKECLEANGARBAGE) 

backup:
	mkdir bak
	cp Makefile *.[Ch] bak

# Grab the sources for a user who has only the makefile
setup:
	/bin/cp -i $(MINIBASE)/src/*.[Ch] .
	-/bin/cp -i $(MINIBASE)/src/README .

# DO NOT DELETE THIS LINE -- make depend needs it 
