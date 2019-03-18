#include "log.h"
#include "file.h"
#include  "directory.h"
#include "flash.h"

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>


static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";
static const char *link_path = "/link";

static int hello_getattr(const char *path, struct stat *stbuf)
{
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

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
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

static int hello_open(const char *path, struct fuse_file_info *fi)
{
    if (strcmp(path, hello_path) != 0)
        return -ENOENT;

    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
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

static int hello_readlink(const char *path, char *buf, size_t size)
{
    if(strcmp(path, link_path) != 0)
        return -ENOENT;
    buf[size - 1] = '\0'; 
    strncpy(buf, ".", size-1);
    strncat(buf, hello_path, size-2);

    return 0;
}

static struct fuse_operations hello_oper = {
    .getattr	= hello_getattr,
    .readdir	= hello_readdir,
    .open	= hello_open,
    .read	= hello_read,
    .readlink   = hello_readlink,
};

int main(int argc, char *argv[])
{
	 char 	**nargv = NULL;
	  int		i;
	  int      nargc;

	if(argc<2)
	 { 
       cout<<"create a lfs: usage  lfs [options] file mountpoint"<<endl;
       cout<<"  -f "<<endl;
       cout<<"Pass the ‘-f’ argument to fuse_main so it runs in the foreground. "<<endl;
       cout<<"-s num, --cache=num  "<<endl;
       cout<<"Size of the cache in the Log layer, in segments. Default is 4."<<endl;
       cout<<"-i num, --interval=num "<<endl;
       cout<<"Checkpoint interval, in segments. Default is 4. "<<endl;
       cout<<"-c num, --start=num "<<endl;
       cout<<"Threshold at which cleaning starts, in segments. Default is 4."<<endl;
      }
  // default vaule 
    int cache=4;
    int interval=4;
    int cleanstart=4;
    int cleanstop=8;
    char *fuseargv;
    for(int i=1;i<argc-2;){
       // cout<<".  "<<argv[i]<<endl;
        if(strcmp(argv[i],"-s")==0){
            if(i+2<=argc){
                cache=stoi(argv[i+1]);
                i+=2;
                continue;
            }else{
                cout<<"Error usage: -s must follow a number  ex: -b 2"<<endl;
            }
        } else
        if(strcmp(argv[i],"-i")==0){
            if(i+2<=argc){
                interval=stoi(argv[i+1]);
                 i+=2;
                 continue;
            }else{
                cout<<"Error usage: -i must follow a number ex: -l 32 "<<endl;
            }
        } else
        if(strcmp(argv[i],"-c")==0){
            if(i+2<=argc){
                cleanstart=stoi(argv[i+1]);
                 i+=2;
                 continue;
            }else{
                cout<<"Error usage: -c must follow a number: ex -c 100 "<<endl;
            }
        }
        if(strcmp(argv[i],"-C")==0){
            if(i+2<argc){
                cleanstop=stoi(argv[i+1]);
                  i+=2;
                  continue;
            } else{
                cout<<"Error usage: -C must follow a number: ex -C 100 "<<endl;
            }
        }else if(strcmp(argv[i],"-f")==0){
        		fuseargv=argv[i+1];
        		i+=2;
        		continue;
        }
         else
         {
          //   cout<<" i= "<<i<<" argc ="<<argc<<endl;
	        cout<<"create a lfs: usage  lfs [options] file mountpoint"<<endl;
	     	 cout<<"  -f "<<endl;
	       	cout<<"Pass the ‘-f’ argument to fuse_main so it runs in the foreground. "<<endl;
	       	cout<<"-s num, --cache=num  "<<endl;
	       	cout<<"Size of the cache in the Log layer, in segments. Default is 4."<<endl;
	       	cout<<"-i num, --interval=num "<<endl;
	       	cout<<"Checkpoint interval, in segments. Default is 4. "<<endl;
	       	cout<<"-c num, --start=num "<<endl;
	       	cout<<"Threshold at which cleaning starts, in segments. Default is 4."<<endl;
           return 0;
        }
    }

	   

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
	    return fuse_main(nargc, nargv, &hello_oper, NULL);
}