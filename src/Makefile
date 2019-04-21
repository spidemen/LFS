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
LFSCK=lfsck
TLFSS=lfs
LFS=..

CC=gcc

CC1=g++

#CFLAGS= -DUNIX -Wall -g -D_FILE_OFFSET_BITS=64 -std=c++11
CFLAGS=  -DUNIX  -Wall -g -D_FILE_OFFSET_BITS=64 -std=c++11 `pkg-config fuse --cflags --libs`

HOME=${CURDIR}

#INCLUDES = -I${LFS}/include  -I../usr/include/linux -I/usr/include/python2.7
INCLUDES = -I${LFS}/include

VPATH=/home/bill/Desktop/source-code/csc552/LFS/fuse-2.9.9

LFLAGS= -lfuse -lstdc++

#LFLAGS= -L${LFS}LFS/lib  -lfuse -lstdc++ -lpython2.7

BIN=../bin

PKGFLAGS= pkg-config fuse --cflags --libs

<<<<<<< HEAD
SRCS = hello.C

#OBJS = $(SRCS:.C=.o)
OBJS=lfs.o flash.o log.o
OBJC=log.o

LOGS=log.C flash.C
=======
SRCS = flash.C log.C
#OBJS = $(SRCS:.C=.o) $(SRCc:.c=.o)
OBJS=lfs.o flash.o log.o

LOGS=log.cpp  ../include/flash.h ../include/log.h flash.C
>>>>>>> ebeb226ab1ba6b6e0b7d968858be397a7dca2ad5
OBJLOG = log.o flash.o

FILES=cfile.c flash.C log.c
OBJFILE= cfile.o flash.o log.o

MKLF=log.C flash.C
OBJMKLFS= mklfs.o flash.o

DIRECTORYS=directory.C file.C log.C
OBJDIRECTORY=directory.o file.o log.o 

TESTS=testCase.C log.C flash.C
OBJTEST=testCase.o  file.o log.o flash.o 

LFSCKS=lfsck.C log.C flash.C
OBJLFSCK= lfsck.o log.o flash.o

TLFS=lfs.C log.C file.C flash.C
OBJFLS=lfs.o log.o flash.o

$(MAIN):  $(OBJS)
	 cd $(HOME)
	 $(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(MAIN) $(LFLAGS)
	 mv *.o $(BIN)

$(LOG):  $(OBJLOG)
	cd $(HOME)
	$(CC1) $(CFLAGS) $(INCLUDES) $(OBJLOG) -o $(LOG) $(LFLAGS)
	mv *.o $(BIN)

$(MKLFS): $(OBJMKLFS)
	cd $(HOME)
<<<<<<< HEAD
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJMKLFS) -o $(MKLFS) $(LFLAGS)
=======
	$(CC1) $(CFLAGS) $(INCLUDES) $(OBJMKLFS) -o $(MKLFS) $(LFLAGS)
>>>>>>> ebeb226ab1ba6b6e0b7d968858be397a7dca2ad5
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

$(LFSCK): $(OBJLFSCK)
	cd $(HOME)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJLFSCK) -o $(LFSCK) $(LFLAGS)
	mv *.o $(BIN)

$(TLFSS): $(OBJFLS)
	cd $(HOME)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJFLS) -o $(TLFSS) $(LFLAGS)
	mv *.o $(BIN)
<<<<<<< HEAD
=======
# union compile with C 
log.o: $(LOGS)
	$(CC1) $(CFLAGS) $(INCLUDES) $(LFLAGS) -c -o $@ log.cpp
>>>>>>> ebeb226ab1ba6b6e0b7d968858be397a7dca2ad5

.C.o:
	$(CC1) $(CFLAGS) $(INCLUDES) $(LFLAGS) -c $<
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $(LFLAGS) -c $<
.cpp.o:
	$(CC1) $(CFLAGS) $(INCLUDES) $(LFLAGS) -c $<

depend: $(FILES)
	makedepend $(INCLUDES) $^
#Don't remove MLKFS, used in lfstest.py
clean:
	cd $(HOME)
<<<<<<< HEAD
	rm -f *.o *~ $(MAIN) $(LOG) $(FILE) $(MAKECLEANGARBAGE) $(BIN)/*
=======
	rm -f *.o *~ $(MAIN) $(LOG) $(FILE) $(TEST) $(LFSCK) $(MAKECLEANGARBAGE) $(BIN)/*
>>>>>>> ebeb226ab1ba6b6e0b7d968858be397a7dca2ad5

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
