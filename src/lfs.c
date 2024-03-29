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

//#include "directory.h"

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";
static const char *link_path = "/link";

int cacheSize = 4;
char allFileName[40][10];

static int lfs_getattr(const char *path, struct stat *stbuf)
{
    printf("getattr function test src test2....path= %s \n", path);
    int res = 0;
    memset(stbuf, 0, sizeof(struct stat));
    int num;
    int types = Directory_Types(path, stbuf, &num);
    if (types == 0)
    {
        res = -ENOENT;
    }
    else if (types == 2)
    { // directory
        stbuf->st_ino = 3;
    }
    else if (types == 3)
    { // file
        //stbuf->st_size = 300;
        stbuf->st_ino = 17;
    }
    else if (types == 4)
    { // link;
        stbuf->st_ino = 17;
    }
    else
    {
        stbuf->st_mode = S_IFREG | 07;
        stbuf->st_nlink = 1;
        //      stbuf->st_size = 500;
        stbuf->st_ino = 17;
    }

    return res;
}

static int lfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
    printf("read dir function being called \n");
    (void)fi;
    int nextoffset = 0;
    int size;
    struct stat stbuf[10];
    memset(allFileName, 0, 400);
    if (Directory_getAllFiles(path, stbuf, &size, allFileName))
    {
        return -ENOENT;
    }
    else
    {
        for (int i = 0; i < size; i++)
        {
            printf("fuse get filename = %s\n", allFileName[i]);
            filler(buf, allFileName[i], &stbuf[i], 0);
        }
    }

    //    filler( buf, ".", NULL, 0 ); // Current Directory
    //   //   nextoffset+=((24+strlen(".")+7)&~7);
    // if(filler( buf, "..", NULL, 0)) return 0; // Parent Directory

    //    filler(buf, hello_path + 1, NULL, 0);
    //    filler(buf, link_path + 1, NULL, 0);
    //   //  nextoffset+=((24+strlen("..")+7)&~7);
    //    if (strcmp(path, "/") == 0){
    //        if(filler( buf, "file54", NULL, 0)) return 0;
    //        //   nextoffset+=((24+strlen("file54")+7)&~7);
    // 	// if(filler( buf, "file349", NULL, nextoffset )) return 0;
    //  //        nextoffset+=((24+strlen("file349")+7)&~7);
    //  //       if(filler( buf, "offsettest", NULL, nextoffset)) return 0;
    //      //  if(filler( buf, "offsettest1", NULL, offset+3 ))  return 0;
    //    }

    return 0;
}

static int lfs_open(const char *path, struct fuse_file_info *fi)
{

    // cout<<"open file function called "<<endl;
    printf("open file function called  src\n");
    // if (strcmp(path, hello_path) != 0)
    //     return -ENOENT;

    // if ((fi->flags & 3) != O_RDONLY)
    //     return -EACCES;

    return 0;
}

static int lfs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{

    // cout<<"create function called"<<endl;
    printf("create function called  src\n");
    struct stat stbuf;
    Directory_createFile(path,&stbuf);
    // if (strcmp(path, hello_path) != 0)
    //     return -ENOENT;

    // if ((fi->flags & 3) != O_RDONLY)
    //     return -EACCES;

    return 0;
}

static int lfs_init(const char *path, mode_t mode, struct fuse_file_info *fi)
{

    printf("init function called  src \n");
    initDirectory(4);
    // if (strcmp(path, hello_path) != 0)
    //     return -ENOENT;

    // if ((fi->flags & 3) != O_RDONLY)
    //     return -EACCES;

    return 0;
}

static int lfs_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{

    // implement FILE_READ to get content
    size_t len;
    (void)fi;
    printf("read function was being called \n");

    Directory_readFile(path, offset, size, buf);

    return size;
}

static int lfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    printf("write function was called \n");

    Directory_writeFile(path, offset, size, buf);
    return size;
}

static int lfs_readlink(const char *path, char *buf, size_t size)
{
    printf("read link function was called \n");
    if (strcmp(path, link_path) != 0)
        return -ENOENT;
    buf[size - 1] = '\0';
    strncpy(buf, ".", size - 1);
    strncat(buf, hello_path, size - 2);

    return 0;
}

static int lfs_truncate(const char *path, off_t size)
{
    printf("trucate function was called \n");
    return 0;
}
static int lfs_chown(const char *path, uid_t uid, gid_t gid)
{
    printf("chown function was called \n");

    return 0;
}

void lfs_destroy(void *private_data)
{
    printf("destory function was called\n ");
    Directory_Destroy();
    return;
}


static int lfs_mknod(const char *path, mode_t mode, dev_t rdev)
{
    printf(" mknod function was called \n");
    
    return 0;
}
static int lfs_mkdir(const char *path, mode_t mode)
{
    printf("mkdir called .................\n");
    Directory_EntyUpdate(path, 1); // add entry
                                   //        return -errno;
    return 0;
}

static int lfs_rmdir(const char *path)
{

    printf(" rmdir was called \n");
    Directory_EntyUpdate(path, 2); // remove entry
    return 0;
}

int lfs_statfs(const char *path, struct statvfs *stbuf)
{
    
    printf(" stafs function was called \n");

    return 0;
}
static int lfs_opendir(const char *path, struct fuse_file_info *fi)
{
    printf(" opendir function was called \n ");
    return Directory_Open(path);
}
static int lfs_chmod(const char *path, mode_t mode)
{
    printf(" chmod function was called \n ");
    Directory_Permission(path,mode);

    return 0;
}

static int lfs_rename(const char *from, const char *to)
{
    printf("renme function was called \n ");
    Directory_EntryRename(from,to,0);  
    return 0;
}

static int lfs_release(const char *path, struct fuse_file_info *fi)
{
    printf("release function was called \n ");

    return 0;
}
static int lfs_flush(const char *path, struct fuse_file_info *fi)
{
    printf(" flush function was called \n");

    return 0;
}

static int lfs_ftruncate(const char *path, off_t size)
{
    printf(" ftruncate function was called \n");
    return 0;
}

static int lfs_lock(const char *path, struct fuse_file_info *fi, int cmd, struct flock *locks)
{

    printf("lock function was called \n ");
    return 0;
}

static int lfs_symlink(const char *to, const char *from)
{
    printf("symlink function was called from %s  to %s \n ", from, to);
    return 0;
}
static int lfs_unlink(const char *path)
{
    printf("unlink function was called \n ");
    Directory_EntyUpdate(path, 3); // remove file
    return 0;
}
static int  lfs_link(const char* from, const char* to){
    printf("Link  from  %s  to  %d \n",from, to);
    return 0;
  }

  static int lfs_setxattr(const char *path, const char *name, const char *value, size_t size, int flags)
{
    printf("set attar function was called \n");
    return 0;
}
static int lfs_getxattr(const char *path, const char *name, char *value, size_t size)
{
    printf(" get x attr function was called \n");
    return 0;
}
static int lfs_utime(const char *path, struct utimbuf *ubuf)
{
    printf(" untime function was called \n");
    return 0;
}
static int lfs_poll(const char *path, struct fuse_file_info *fi, struct fuse_pollhandle *ph, unsigned *reventsp)
{
    printf(" poll funciton was called \n ");
    return 0;
}
static int lfs_ioctl(const char *path, int cmd, void *arg, struct fuse_file_info *fi, unsigned int flags, void *data)
{
    printf("ioctl function was called \n ");
    return 0;
}
static int lfs_bmap(const char *path, size_t blocksize, uint64_t *blockno)
{
    printf("lfs_bmap function was called \n ");
    return 0;
}
static struct fuse_operations lfs_oper = {

    .getattr = lfs_getattr,
    .readdir = lfs_readdir,
    .open = lfs_open,
    .read = lfs_read,
    .readlink = lfs_readlink,
    .create = lfs_create,
    .init = lfs_init,
    .setxattr = lfs_setxattr,
    .getxattr = lfs_getxattr,
    .utime = lfs_utime,
    .mkdir = lfs_mkdir,
    .destroy = lfs_destroy,
    .mknod = lfs_mknod,
    .symlink = lfs_symlink,
    .unlink = lfs_unlink,
    .rmdir = lfs_rmdir,
    .rename = lfs_rename,
     .link        = lfs_link,
    .chmod = lfs_chmod,
    .chown = lfs_chown,
    .truncate = lfs_truncate,
    .ftruncate = lfs_ftruncate,
    .write = lfs_write,
    .statfs = lfs_statfs,
    .opendir = lfs_opendir,
    .release = lfs_release,
    //.fsync       = dir_fsync,
    .flush = lfs_flush,
    //.fsyncdir    = dir_fsyncdir,
    .lock = lfs_lock,
    .bmap = lfs_bmap,
    .ioctl = lfs_ioctl,
    .poll = lfs_poll,

};

int main(int argc, char *argv[])
{
    char **nargv = NULL;
    int i;
    int nargc;

#define NARGS 3

    nargc = argc + NARGS;

    nargv = (char **)malloc(nargc * sizeof(char *));
    nargv[0] = argv[0];
    nargv[1] = "-f";
    nargv[2] = "-s";
    nargv[3] = "-d";
    for (i = 1; i < argc; i++)
    {
        nargv[i + NARGS] = argv[i];
    }
    return fuse_main(nargc, nargv, &lfs_oper, NULL);
}
