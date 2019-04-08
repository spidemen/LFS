/*
    FUSE: Filesystem in Userspace
    Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

    This program can be distributed under the terms of the GNU GPL.
    See the file COPYING.

    gcc -Wall hello.c -o hello -lfuse
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

 #include "log.h"
// // #include "file.h"
// // #include  "directory.h"
// #include "flash.h"
//#include <iostream>

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";
static const char *link_path = "/link";

// TmP32719
// Bug with mount
// cannot call open and write function here, will fixed this bug after phase 1


static int lfs_getattr(const char *path, struct stat *stbuf)
{
   //  cout<<"C++ getattr function "<<endl;
    printf("getattr function test src test2....\n");
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0777;
        stbuf->st_nlink = 2;
        stbuf->st_ino = 3;
    } else if (strcmp(path, hello_path) == 0) {
        stbuf->st_mode = S_IFREG | 0777;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(hello_str);
        stbuf->st_ino = 17;
    } else if (strcmp(path, link_path) == 0) {
        stbuf->st_mode = S_IFLNK | 0777;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(hello_path);
        stbuf->st_ino = 17;
    } else
        res = -ENOENT;

    return res;
}

static int lfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, hello_path + 1, NULL, 0);
    filler(buf, link_path + 1, NULL, 0);

    return 0;
}

static int lfs_open(const char *path, struct fuse_file_info *fi)
{

    // cout<<"open file function called "<<endl;     
    printf("open file function called  src\n");

    if (strcmp(path, hello_path) != 0)
        return -ENOENT;

    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}


static int lfs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{

    // cout<<"create function called"<<endl;     
    printf("create function called  src\n");

    if (strcmp(path, hello_path) != 0)
        return -ENOENT;

    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}

static int lfs_init(const char *path, mode_t mode, struct fuse_file_info *fi)
{

    // cout<<"init function called"<<endl;     
     printf("init function called  src \n");
     
    if (strcmp(path, hello_path) != 0)
        return -ENOENT;

    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}


static int lfs_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
    size_t len;
    (void) fi;
    if(strcmp(path, hello_path) != 0)
        return -ENOENT;

    len = strlen(hello_str);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, hello_str + offset, size);
    } else
        size = 0;

    return size;
}

static int lfs_readlink(const char *path, char *buf, size_t size)
{
    if(strcmp(path, link_path) != 0)
        return -ENOENT;
    buf[size - 1] = '\0'; 
    strncpy(buf, ".", size-1);
    strncat(buf, hello_path, size-2);

    return 0;
}

static struct fuse_operations lfs_oper = {

    .getattr	= lfs_getattr,
    .readdir	= lfs_readdir,
    .open	= lfs_open,
    .read	= lfs_read,
    .readlink   = lfs_readlink,
    .create= lfs_create,
    .init= lfs_init,

     //  getattr : hello_getattr,
     // readdir : hello_readdir,
     // open    : hello_open,
     // read    : hello_read,
     // readlink   : hello_readlink,
     // create   : hello_create,
     // init     :   hello_init,
};

// static struct lfs_operations : fuse_operations {
//        fms_operations() {
//          getattr  = lfs_getattr,
//          readdir    = lfs_readdir,
//          open   = lfs_open,
//          read   = lfs_read,
//          readlink   = lfs_readlink,
//         create= lfs_create,
//          init= lfs_init,
//        }
// } lfs_oper_init;

int main(int argc, char *argv[])
{
    char 	**nargv = NULL;
    int		i;
    int         nargc;

#define NARGS 3

    nargc = argc + NARGS;

    nargv = (char **) malloc(nargc * sizeof(char*));
    nargv[0] = argv[0];
    nargv[1] = "-f";
    nargv[2] = "-s";
    nargv[3] = "-d";
    for (i = 1; i < argc; i++) {
	nargv[i+NARGS] = argv[i];
    }
    return fuse_main(nargc, nargv, &lfs_oper, NULL);
}
