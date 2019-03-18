//
//  file.h
//

#ifndef file_h
#define file_h
#include <stdio.h>
#include <string.h>
#include <time.h>
#include  "log.h"


#define INODE_SIZE 144
// struct IfileAddress {
//     vector<logAddress> addresses; 
// };


// struct IfileLocation {
//     u_int mostRecentSeg;
//     u_int mostRecentBlock;
// };

struct Inode {
    int inum;           // in spec
    string filename;     // in spec
    int type = 0;           // in spec -- 0 for file, 1 for directory
    u_int size = 0;         // in spec
    u_int block_number = 0; // in spec -- flash addresses of file's blocks
    
    int in_use = 0; 	// true 1, false 0
    //time_t time_of_last_change;
    struct tm * time_of_last_change;
    string owner = "user";
    int permissions = 0;
    u_int offset = 0;

    struct logAddress *Block1Ptr;
    struct logAddress *Block2Ptr;
    struct logAddress *Block3Ptr;
    struct logAddress *Block4Ptr;
    struct logAddress *OtherBlocksPtr;
};

//static const int FNAME_LEN = 50;
struct Ifile {
    int inum = 0;
    vector<Inode> data; //location of Inode == inum, should be vector<Inode>
    vector<logAddress> addresses; 
    int size = 0; //data.size() * INODE_SIZE; //Xing: how to do something like this?
};


int File_Create(int inum, int type);
int File_Write(int inum, int offset, int length, char* buffer);
int File_Read(int inum, int offset, int length, char* buffer);


#endif /* file_h */
