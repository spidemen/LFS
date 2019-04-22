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

SRCS = flash.C log.C
#OBJS = $(SRCS:.C=.o) $(SRCc:.c=.o)
OBJS=lfs.o flash.o  directory.o cfile.o log.o

LOGS=log.cpp  ../include/flash.h ../include/log.h flash.C
OBJLOG = log.o flash.o

FILES=cfile.cpp flash.C  ../include/flash.h  ../include/log.h  ../include/cfile.h
OBJFILE= cfile.o flash.o log.o

MKLF=log.C flash.C
OBJMKLFS= mklfs.o flash.o

DIRECTORYS=directory.cpp cfile.cpp log.cpp ../include/directory.h  ../include/cfile.h ../include/flash.h  ../include/log.h  
OBJDIRECTORY=directory.o cfile.o log.o flash.o

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
	$(CC1) $(CFLAGS) $(INCLUDES) $(OBJMKLFS) -o $(MKLFS) $(LFLAGS)
	mv *.o $(BIN)

$(FILE):  $(OBJFILE)
	cd $(HOME)
	$(CC1) $(CFLAGS) $(INCLUDES) $(OBJFILE) -o $(FILE) $(LFLAGS)
	mv *.o $(BIN)

$(DIRECTORY): $(OBJDIRECTORY)
	cd $(HOME)
	$(CC1) $(CFLAGS) $(INCLUDES) $(OBJDIRECTORY) -o $(DIRECTORY) $(LFLAGS)
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
# union compile with C 
log.o: $(LOGS)
	$(CC1) $(CFLAGS) $(INCLUDES) $(LFLAGS) -c -o $@ log.cpp
cfile.o: $(FILES)
	$(CC1) $(CFLAGS) $(INCLUDES) $(LFLAGS) -c -o $@ cfile.cpp
directory.o: $(DIRECTORYS)
	$(CC1) $(CFLAGS) $(INCLUDES) $(LFLAGS) -c -o $@ directory.cpp

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
	rm -f *.o *~ $(MAIN) $(LOG) $(FILE) $(TEST) $(LFSCK) $(MAKECLEANGARBAGE) $(BIN)/*

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
