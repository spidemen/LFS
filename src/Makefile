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

CC=gcc

CFLAGS= -DUNIX -Wall -g -D_FILE_OFFSET_BITS=64

HOME=/home/bill/Desktop/source-code/csc552/LFS/src

INCLUDES = -I${LFS}/include  -I../usr/include/linux

VPATH=/home/bill/Desktop/source-code/csc552/LFS/fuse-2.9.9

LFLAGS= -L${LFS}LFS/lib  -lfuse

BIN=../bin

PKGFLAGS= pkg-config fuse --cflags --libs

SRCS = hello.C flash.c

OBJS = $(SRCS:.C=.o)

$(MAIN):  $(OBJS)
	 cd $(HOME)
	 $(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(MAIN) $(LFLAGS)
	 mv *.o $(BIN)
#.C.o:
#	$(CC) $(CFLAGS) $(INCLUDES) $(LFLAGS) -c $<

clean:
	cd $(HOME)
	rm -f *.o *~ $(MAIN) $(MAKECLEANGARBAGE) $(BIN)/*

backup:
	mkdir bak
	cp Makefile *.[Ch] bak

# install fuse
setup:
	cd $(VPATH)
	./configure
	make -j8
	sudo make install

# DO NOT DELETE THIS LINE -- make depend needs it 
