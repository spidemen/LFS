# CSC 552 Makefile
#  Author: xing yuan Kathryn Paige Williams
#  Date: 10 Feb 2019
#  LFS project architecture
#  log layer :  
#  File layer :
#  Directory layer ""

.PHONY: depend clean backup setup

MAIN=fuse
LOG=log
FILE=file
MKLFS=mklfs
DIRECTORY=directory
TEST=test
TLFSS=lfs
LFS=..

CC=gcc

CFLAGS= -DUNIX -Wall -g -D_FILE_OFFSET_BITS=64 -std=c++11

HOME=${CURDIR}

INCLUDES = -I${LFS}/include  -I../usr/include/linux

VPATH=/home/bill/Desktop/source-code/csc552/LFS/fuse-2.9.9

LFLAGS= -L${LFS}LFS/lib  -lfuse -lstdc++ 

BIN=../bin

PKGFLAGS= pkg-config fuse --cflags --libs

SRCS = hello.C flash.C log.C

OBJS = $(SRCS:.C=.o)

LOGS=log.C flash.C
OBJLOG = log.o flash.o

FILES=file.C flash.C log.C
OBJFILE= file.o flash.o log.o

MKLF=log.C flash.C
OBJMKLFS= mklfs.o flash.o

DIRECTORYS=directory.C file.C log.C
OBJDIRECTORY=directory.o file.o log.o 

TESTS=testCase.C log.C flash.C
OBJTEST=testCase.o  log.o flash.o 

TLFS=lfs.C log.C file.C flash.C
OBJFLS=lfs.o log.o file.o flash.o

$(MAIN):  $(OBJS)
	 cd $(HOME)
	 $(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(MAIN) $(LFLAGS)
	 mv *.o $(BIN)

$(LOG):  $(OBJLOG)
	cd $(HOME)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJLOG) -o $(LOG) $(LFLAGS)
	mv *.o $(BIN)

$(MKLFS): $(OBJMKLFS)
	cd $(HOME)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJMKLFS) -o $(MKLFS) $(LFLAGS)
	mv *.o $(BIN)

$(FILE):  $(OBJFILE)
	cd $(HOME)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJFILE) -o $(FILE) $(LFLAGS)
	mv *.o $(BIN)

$(DIRECTORY): $(OBJDIRECTORY)
	cd $(HOME)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJDIRECTORY) -o $(DIRECTORY) $(LFLAGS)
	mv *.o $(BIN)

$(TEST): $(OBJTEST)
	cd $(HOME)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJTEST) -o $(TEST) $(LFLAGS)
	mv *.o $(BIN)

$(TLFSS): $(OBJFLS)
	cd $(HOME)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJFLS) -o $(TLFSS) $(LFLAGS)
	mv *.o $(BIN)

.C.o:
	$(CC) $(CFLAGS) $(INCLUDES) $(LFLAGS) -c $<
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $(LFLAGS) -c $<

depend: $(FILES)
	makedepend $(INCLUDES) $^
#Don't remove MLKFS, used in lfstest.py
clean:
	cd $(HOME)
	rm -f *.o *~ $(MAIN) $(LOG) $(FILE) $(MAKECLEANGARBAGE) $(BIN)/*

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
