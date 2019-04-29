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

extern int Directory_getAllFiles(const char *path,struct stat *stbuf,int size);  // opendir ,readdir

extern int Directory_getOneFile(const char *path, const char *filename,struct stat *stbuf);

extern int Directory_updateFile(const char *path, char *filename, struct stat *stbuf); // write 

extern int Directory_createFile(const char *path, char *filename, struct stat *stbuf);

extern int Directory_deleteFile(const char *path,char *filename,struct stat *stbuf);  // mark inode user=-1, then mark block point to be default value--call Log_writeDeadBlock

extern int Directory_readFile(const char *path, char *filename, int offset, char *buf);

extern int Directory_writeFile(const char *path, char *filename, int offset, char *buf);

int convertInodeToStat(struct Inode inode, struct stat s);

void  Directoy_destory();

//xing
// init , destroy, statfs, release,releasdir, symlink, truncate, mkdir, rmdir, getattr

// Katy
// fuse --create, chmod, chown, read, open, link


#ifdef __cplusplus
}
#endif


#endif