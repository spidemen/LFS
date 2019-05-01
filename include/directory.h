#ifndef _DIRECTORY_H
#define _DIRECTORY_H

#ifdef __cplusplus
extern "C" {
#endif


#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <flash.h>
#include  "cfile.h"
#include <unistd.h>
#include <sys/types.h>
#define IFILE 0

#define NAMESIZE 50


 struct directoryTable
 { 
 	char path[NAMESIZE];
 	char filename[NAMESIZE];
 	int inum;
 };

int currentinum=1;

extern int initDirectory(int cachesize);

extern int Directory_getAllFiles(const char *path,struct stat *stbuf,int *size,char allFileName[40][10]);  // opendir ,readdir

extern int Directory_getOneFile(const char *path, const char *filename,struct stat *stbuf,char *filenameReturn);

extern int Directory_Types(const char *path,struct stat *stbuf,int *num);  // return 2--directory, 3-file, 4-link

extern int Directory_updateFile(const char *path, char *filename, struct stat *stbuf); // write 

extern int Directory_EntyUpdate(const char *path,int type);				// 1- add entry, 2-remove entry   only work directory

extern int Directory_Open(const char *path);  // 0 success, -1 fail	

extern int Directory_createFile(const char *path, struct stat *stbuf);

extern int Directory_deleteFile(const char *path,struct stat *stbuf);  // mark inode user=-1, then mark block point to be default value--call Log_writeDeadBlock

extern int Directory_readFile(const char *path, int offset, int size, char *buf);

extern int Directory_writeFile(const char *path, int offset, int size,char *buf);


void  Directory_Destroy();

int InitStat(struct stat *stbuf);
//xing
// init , destroy, statfs, release,releasdir, symlink, truncate, mkdir, rmdir, getattr.chmod. chmod

// Katy
// fuse --create, , read, open, link


#ifdef __cplusplus
}
#endif


#endif