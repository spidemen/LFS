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

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";
static const char *link_path = "/link";

// TmP32719
// Bug with mount

static int lfs_getattr(const char *path, struct stat *stbuf)
{
    printf("getattr function test src test2....path= %s \n",path);
    int res = 0;
    memset(stbuf, 0, sizeof(struct stat));

    stbuf->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
	stbuf->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
	stbuf->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
	stbuf->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now

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
    } else{
        stbuf->st_mode = S_IFREG | 0644;
		stbuf->st_nlink = 1;
		stbuf->st_size = 1024;
    }
   //    res = -ENOENT;

    return res;
}

static int lfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
    printf("read dir function being called \n");
    (void) offset;
    (void) fi;
    filler( buf, ".", NULL, 0 ); // Current Directory
	filler( buf, "..", NULL, 0 ); // Parent Directory
    
    filler(buf, hello_path + 1, NULL, 0);
    filler(buf, link_path + 1, NULL, 0);

    if (strcmp(path, "/") == 0){
        filler( buf, "file54", NULL, 0 );
		filler( buf, "file349", NULL, 0 );
    }

    return 0;
}

static int lfs_open(const char *path, struct fuse_file_info *fi)
{


    // cout<<"open file function called "<<endl;     
    printf("open file function called  src\n");
    init("FuseFileSystem");
//     printf("*******************Log layer test 1 simple small write and read ****************************** \n");
// 	char  buf[50]="Hello LFS, welcome to CSC 545 OS class";
// //	strcat(buf,cat);
// //	char  *buf="Hello LFS, welcome to CSC 545 OS class";
// 	inum num=1;
// 	struct logAddress address;
// 	if(!Log_Write(num, 1, 40,(void*)buf,&address)){
// 		char bufR[40];
// 		if(!Log_read(address, 40,(void *)bufR)){
// 			printf("return logadress segmentNo= %d  blockNo=%d \n",address.segmentNo,address.blockNo);
// 			if(strcmp(buf,bufR)!=0){
// 				printf("Fail:  write string  %s does not match read string %s \n",buf,bufR);
// 			}else{
// 				printf("**************Success    test 1 pass*******************************\n ");
// 			}
// 		}
// 	}
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

    // implement FILE_READ to get content
    size_t len;
    (void) fi;
    printf("read function was being called \n");

    char file54Text[] = "Hello World From File54!";
	char file349Text[] = "Hello World From File349!";
	char *selectedText = NULL;
    if ( strcmp( path, "/file54" ) == 0 ){
        size=strlen( selectedText ) - offset;
		selectedText = file54Text;
     printf(" string %s  size=%d \n",selectedText,size);
        memcpy( buf, selectedText + offset, size );
    }
	else if ( strcmp( path, "/file349" ) == 0 ){
        size=strlen( selectedText ) - offset;
		selectedText = file349Text;
        memcpy( buf, selectedText + offset, size );
    }
    else  if(strcmp(path, hello_path) == 0){
        len = strlen(hello_str);
        if (offset < len) {
            if (offset + size > len)
                size = len - offset;
            memcpy(buf, hello_str + offset, size);
        } else
        size = 0;
    } else  
    if(strcmp(path, hello_path) != 0)
        return -ENOENT;
   
    return size;
}

static int lfs_readlink(const char *path, char *buf, size_t size)
{
    printf("read link function was called \n");
    if(strcmp(path, link_path) != 0)
        return -ENOENT;
    buf[size - 1] = '\0'; 
    strncpy(buf, ".", size-1);
    strncat(buf, hello_path, size-2);

    return 0;
}

static int  lfs_truncate(const char* path, off_t size){
       printf("trucate function was called \n");
       return 0;
}
 static int  lfs_chown(const char* path, uid_t uid, gid_t gid){
     printf("chown function was called \n");
     return 0;
 }
static int lfs_setxattr(const char *path, const char *name, const char *value, size_t size, int flags){
    printf("set attar function was called \n");
    return 0;
}
static int lfs_getxattr(const char *path, const char *name, char *value, size_t size)
{
    printf(" get x attr function was called \n");
    return 0;
}
static int lfs_utime(const char *path, struct utimbuf *ubuf){
    printf(" untime function was called \n");
    return 0;
}
static int lfs_write(const char* path, const char *buf, size_t size, off_t offset, struct fuse_file_info* fi){
    printf("write function was called \n");
    return 0;
}
void lfs_destroy(void* private_data){
    printf("destory function was called\n ");
    return ;
}
// static void lfs_close(){
//     printf("close function was called \n");
//    // return 0;
// }
 static int lfs_release(const char* path, struct fuse_file_info *fi){
     printf("release function was called \n ");
     return 0;
 }
static int  lfs_flush(const char* path, struct fuse_file_info* fi){
    printf(" flush function was called \n");
    return 0;
 }

static int lfs_ftruncate(const char* path, off_t size){
    printf(" ftruncate function was called \n");
    return 0;
}
static int lfs_mknod(const char* path, mode_t mode, dev_t rdev){
   printf(" mknod function was called \n");
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
    .setxattr=lfs_setxattr,
    .getxattr=lfs_getxattr,
    .utime= lfs_utime,
  //  .close= lfs_close,
    // .mkdir 	= lfs_mkdir,
     .destroy     = lfs_destroy,
     .mknod       = lfs_mknod,
    // .symlink     = lfs_symlink,
    // .unlink      = lfs_unlink,
    // .rmdir       = lfs_rmdir,
    // .rename      = lfs_rename,
    // .link        = lfs_link,
    // .chmod       = lfs_chmod,
      .chown       = lfs_chown,
      .truncate    = lfs_truncate,
     .ftruncate   = lfs_ftruncate,
    //.create      = dir_create,
    //.open        = dir_open,
    //.read        = dir_read,
      .write       = lfs_write,
    // .statfs      = lfs_statfs,
    // .release     = lfs_release,
    // .opendir     = lfs_opendir,
     .release  = lfs_release,
    //.fsync       = dir_fsync,
     .flush       = lfs_flush,
    //.fsyncdir    = dir_fsyncdir,
    //.lock        = dir_lock,
    //.bmap        = dir_bmap,
    //.ioctl       = dir_ioctl,
    //.poll        = dir_poll,
     
};


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
