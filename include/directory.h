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

int getAllFiles(const char *path,struct Inode *files,int size);  // opendir ,readdir

int getOneFile(const char *path, const char *filename,struct Inode *node);

int updateFile(const char *path, char *filename, struct stat *stbuf); // write 

int createFile(const char *path, char *filename, struct stat *stbuf);

int deleteFile(const char *path,char *filename,struct stat *stbuf);  // mark inode user=-1, then mark block point to be default value--call Log_writeDeadBlock

int readFile(const char *path, char *filename, int offset, char *buf);

int writeFile(const char *path, char *filename, int offset, char *buf);
//xing
// init , destroy, statfs, release,releasdir, symlink, truncate

// Katy
// fuse --create, chmod, chown, read, open, getattr, link


#ifdef __cplusplus
}
#endif


#endif