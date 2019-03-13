//
//  file.h
//

#ifndef file_h
#define file_h
#include <stdio.h>
#include <time.h>

typedef struct Inode {
    int inum;           // in spec
    char* filename;     // in spec
    char* type;         // in spec
    u_int size;          // in spec
    u_int block_number;  // in spec -- flash addresses of file's blocks
    
    int in_use; 	// true 1, false 0
    //time_t time_of_last_change;
    struct tm * time_of_last_change;
    char* owner;
    int permissions;
    u_int offset;
}Inode;


#endif /* file_h */
