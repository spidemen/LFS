//
//  file.h
//

#ifndef file_h
#define file_h
#include <stdio.h>
#include <time.h>
#include  "log.h"

//static const int FNAME_LEN = 50;

struct Inode {
    int inum;           // in spec
    char *filename;     // in spec
    int type;           // in spec -- 0 for file, 1 for directory
    u_int size;         // in spec
    u_int block_number; // in spec -- flash addresses of file's blocks
    
    int in_use; 	// true 1, false 0
    //time_t time_of_last_change;
    struct tm * time_of_last_change;
    char* owner;
    int permissions;
    u_int offset;

    struct logAddress *Block1Ptr;
    struct logAddress *Block2Ptr;
    struct logAddress *Block3Ptr;
    struct logAddress *Block4Ptr;
    struct logAddress *OtherBlocksPtr;
};




#endif /* file_h */
