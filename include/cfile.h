#ifndef file_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include  "log.h"

#define INODE_SIZE 144
#define BLOCK_SIZE 10 //10* sizeof('a') 50
#define MAX_FILES 10

//Delete from here
// struct logAddress {
// 	int blockNo;
// 	int segmentNo; 
// };

// struct Block
// {
// 	char data[BLOCK_SIZE];	
// };
// End delete


struct Inode {
    
    int inum;           
    int type;           			// 0 for file, 1 for directory
    unsigned int size;        
    unsigned int block_number; 
    
    int in_use; 					// true 1, false 0
    //time_t time_of_last_change;
    unsigned long time_of_last_change;
    char owner; 					// u: user, r: root
    int permissions;
    char* group;
    int offset;

    struct logAddress Block1Ptr;
    struct logAddress Block2Ptr;
    struct logAddress Block3Ptr;
    struct logAddress Block4Ptr;
    struct logAddress OtherBlocksPtr;
    // struct Block Block1Ptr;
    // struct Block Block2Ptr;
    // struct Block Block3Ptr;
    // struct Block Block4Ptr;
    // struct Block OtherBlocksPtr;

    //char filename[];
};


struct Ifile {
	int inum;
	int size;
	struct logAddress addresses[MAX_FILES];
	struct Inode data[MAX_FILES];
	//struct Inode data[]; //Katy dummy var
	//struct logAddress addresses[1]; 
};
//static const int FNAME_LEN = 50;
// struct Ifile {
//     int inum = 0;
//     vector<Inode> data; //location of Inode == inum, should be vector<Inode>
//     vector<logAddress> addresses; 
//     int size = 0; //data.size() * INODE_SIZE; //Xing: how to do something like this?
// };


// vector<logAddress> ifileAddress; 


int File_Create(int inum, int type);
int File_Write(int inum, int offset, int length, char* buffer);
int File_Read(int inum, int offset, int length, char* buffer);



#endif /* file_h */