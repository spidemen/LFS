
#ifndef _CFILE_H
#define _CFILE_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include  "log.h"

#define INODE_SIZE 144
//#define BLOCK_SIZE 10 //10* sizeof('a') 50
#define MAX_FILES 10

#define GLOBAL_INUM 0
#define IFILE_INUM 0
#define TYPE_F 3 //3, was 0 //KATY possible issue but I think I checked all uses?
#define TYPE_D 2 //2, was 1
#define TYPE_L 4 //4, was 2


struct Inode {
    

    int inum;           
    int type = 0;           			// 0 for file, 1 for directory, 2 link, 3 special file
    off_t size;   
    int numBlocks = 0;              //the size but in blocks     
    
    int in_use = 1; 					// true 1, false 0
    time_t atime;                        //last access of file/directory
    time_t mtime;                        //last modification of file/directory
    time_t ctime;                        //last status change
    uid_t owner; 	
    gid_t group;			
    mode_t permissions; 
    int nlink = 1;                      // 1 for file, 2 for directory
    
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

    char filename[50];
    char path[50];
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

extern int initFile(int size);

extern int File_Create(int inum, int type);
extern int File_Write(int inum, int offset, int length,  void * buffer);
extern int File_Read(int inum, int offset, int length,  void * buffer);

extern  int File_Free(int inum);
extern int File_Get(int inum, struct Inode *node);  // 0 success  2: deleted file  1: Fail does not exit

extern void File_Destroy();

extern int Test_File_Create(int inum);
extern int Test_File_Write(int inum);
extern void Show_Ifile_Contents();

extern int File_Naming(int inum,const char *directory,const char *filename,struct stat *stbuf);   // pass the directory and filename and do update on inode
extern  int convertInodeToStat(int num, struct stat *s);
//extern  int convertInodeToStat(struct Inode* inode, struct stat *stbuf);

extern int Change_Permissions(int inum, mode_t permissions);
extern int Change_Owner(int inum, uid_t owner);
extern int Change_Group(int inum, gid_t group);

  // NOTICE:  test initIfle:  after finish write and restart , remeber to call log_checkPoint function,otherwise there will a lots of bug or just call Log_destory
#ifdef __cplusplus
}
#endif

#endif /* file_h */