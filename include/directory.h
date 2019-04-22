#ifndef _DIRECTORY_H
#define _DIRECTORY_H

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <flash.h>
#include  "file.h"

#define IFILE 0

#define NAMESIZE 50
typedef string FileName;

int inodeSize=144;   // default vaule

int currentinum=1;


 struct directoryTable
 { 
 	char path[NAMESIZE];
 	char filename[NAMESIZE];
 	int inum;
 };

 int initDirectory();

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

#endif