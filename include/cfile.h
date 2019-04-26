
#ifndef _CFILE_H
#define _CFILE_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include  "log.h"

#define INODE_SIZE 144
//#define BLOCK_SIZE 10 //10* sizeof('a') 50
#define MAX_FILES 10


struct Inode {
    
//     dev_t     st_dev     ID of device containing file
// ino_t     st_ino     file serial number
// mode_t    st_mode    mode of file (see below)
// nlink_t   st_nlink   number of links to the file
// uid_t     st_uid     user ID of file
// gid_t     st_gid     group ID of file
// dev_t     st_rdev    device ID (if file is character or block special)
// off_t     st_size    file size in bytes (if file is a regular file)
// time_t    st_atime   time of last access
// time_t    st_mtime   time of last data modification
// time_t    st_ctime   time of last status change
// blksize_t st_blksize a filesystem-specific preferred I/O block size for
//                      this object.  In some filesystem types, this may
//                      vary from file to file
// blkcnt_t  st_blocks  number of blocks allocated for this object


    int inum;           
    int type;           			// 0 for file, 1 for directory
    int size = 0;        
    
    int in_use = 1; 					// true 1, false 0
    char* atime;                        //last access of file/directory
    char* mtime;                        //last modification of file/directory
    char owner = 'u'; 					// u: user, r: root
    int permissions = 777;
    int nlink;                      // 1 for file, 2 for directory
    char* group = "aaa";
    int offset;

    struct logAddress Block1Ptr= {
        .blockNo = 0,
        .segmentNo = 0
    };
    struct logAddress Block2Ptr= {
        .blockNo = 0,
        .segmentNo = 0
    };
    struct logAddress Block3Ptr= {
        .blockNo = 0,
        .segmentNo = 0
    };
    struct logAddress Block4Ptr= {
        .blockNo = 0,
        .segmentNo = 0
    };
    struct logAddress OtherBlocksPtr= {
        .blockNo = 0,
        .segmentNo = 0
    };
    // struct Block Block1Ptr;
    // struct Block Block2Ptr;
    // struct Block Block3Ptr;
    // struct Block Block4Ptr;
    // struct Block OtherBlocksPtr;

    //char filename[];
};


// struct Ifile {
// 	int inum;
// 	int size;
// 	struct logAddress addresses[MAX_FILES];
// 	struct Inode data[MAX_FILES];
// 	//struct Inode data[]; //Katy dummy var
// 	//struct logAddress addresses[1]; 
// };
//static const int FNAME_LEN = 50;
// struct Ifile {
//     int inum = 0;
//     vector<Inode> data; //location of Inode == inum, should be vector<Inode>
//     vector<logAddress> addresses; 
//     int size = 0; //data.size() * INODE_SIZE; //Xing: how to do something like this?
// };


// vector<logAddress> ifileAddress; 

int initFile(int size);

int File_Create(int inum, int type);
int File_Write(int inum, int offset, int length,  void * buffer);
int File_Read(int inum, int offset, int length,  void * buffer);

int Test_File_Create(int inum);

#ifdef __cplusplus
}
#endif

#endif /* file_h */