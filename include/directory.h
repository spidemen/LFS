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

extern int initDirectory();

int Directory_getAllFiles(const char *path,struct Inode *files,struct stat *stbuf,int size);  // opendir ,readdir

int Directory_getOneFile(const char *path, const char *filename,struct stat *stbuf);

int Directory_updateFile(const char *path, char *filename, struct stat *stbuf); // write 

int Directory_createFile(const char *path, char *filename, struct stat *stbuf);

int Directory_deleteFile(const char *path,char *filename,struct stat *stbuf);  // mark inode user=-1, then mark block point to be default value--call Log_writeDeadBlock

int Directory_readFile(const char *path, char *filename, int offset, char *buf);

int Directory_writeFile(const char *path, char *filename, int offset, char *buf);

int convertInodeToStat(struct Inode inode, struct stat s);

void  Directory_destory();

//xing
// init , destroy, statfs, release,releasdir, symlink, truncate

// Katy
// fuse --create, chmod, chown, read, open, getattr, link


#ifdef __cplusplus
}
#endif


#endif