#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
//#include "log.h"
#include <time.h>
#include "file.h"

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";
static const char *link_path = "/link";

using namespace std;

// struct Inode Get_Inode(int inum) { // all inodes are stored in ifile (file at root directory (like testdir))
//     // Get inode
//     struct Inode this_inode;
//     // Call down into memory to get inode with inum

//     return this_inode;
// }

int Inode_Create(int inum, char* type) {
    
    struct Inode new_inode;
    new_inode.inum = inum;
    new_inode.type = type;
    //new_inode.block_number = 
    new_inode.in_use = 1;
    time_t rawtime;
    time ( &rawtime );
    new_inode.time_of_last_change = localtime( &rawtime );
    new_inode.owner = "user";
    new_inode.permissions = 777;

    
    printf( "Time created: %s", asctime( new_inode.time_of_last_change));

    return 1; 
}

int Change_File_Permissions(int inum, int permissions) {
    // Retrieve inode
    //struct Inode this_inode = Get_Inode(inum);
    //this_inode.permissions = permissions;
    // set inode 
    return 0;
}

int Change_File_Owner(int inum) {
    
    return 0;
}

int Change_File_Group(int inum) {
    
    return 0;
}

int File_Create(int inum, char* type) {
    // Need to create the inode for the file
    Inode_Create(inum, type);

    // Need to create an empty file in the log layer

    return 0;
}

int File_Write(int inum, int offset, int length, char* buffer) {
    printf("%s\n", buffer);
    // Retrieve inode

    // Retrieve file

    // Modify file at it's offset

    // Record the changed time
    // time_t rawtime;
    // time ( &rawtime );
    // this_inode.time_of_last_change = localtime( &rawtime );

    return 0;
}

int File_Read(int inum, int offset, int length, int buffer) {

    // Consult inode map to get disk address for inode inum

    // Read inode into memory (if not already in buffer cache)
    // and find appropriate data pointer

    // Read the data block that is referenced by the data pointer
    return 0;
}

int File_Free(int inum) {
    return 0;
}

int main(int argc, char *argv[])
{
	printf("hello file.c\n");
	// Creat iFile
	//File_Create(1, "filetype");
    return 1;
}
