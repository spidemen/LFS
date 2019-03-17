#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <flash.h>
#include <vector>
#include "log.h"
#include <time.h>
#include "file.h"

int GLOBAL_INUM = 0;
static const int IFILE_INUM = 0;
static const int TYPE_F = 0;
static const int TYPE_D = 1;

Ifile *ifile = new Ifile;


// return 0 if things are fine, 1 if error
/*
Notice: Katy
Possibley, you need to rewrite your code, please implement log_write and Log_read. 
rewrite file :  1st write "hello word "  2nd write "katy, morining" offerset is 4, then return string should hello katy, morining. 
Implement this, first need to get string hello world using log_read and then concat these two string "hello, katy morining" , write
to this flash using log_write. 
Noramally, read and write should be fine. Notice, rewrite , concat , offset...
Notice: every  write should do update inode as well as file data, which means need to call log_write twice 
you can use one point to metadat, another to data, another to precious block data , another to logaddress, last to file data
*/

#define INODE_SIZE 144;
int Get_Inode(int inum, struct Inode *iptr) { // all inodes are stored in ifile (file at root directory (like testdir))
   	
    printf("Getting Inode data (bad way)...\n");
    *iptr = ifile->data.at(inum);
    int flag; 
    int offset = inum * INODE_SIZE; //Xing
    int length = INODE_SIZE;


    //Get most recent ifile address
    logAddress ladd = ifile->addresses.back();
    printf("ifile address %d %d\n", ladd.segmentNo, ladd.blockNo);
    // ladd.segmentNo = ifileLoc.mostRecentSeg;
    // ladd.blockNo = ifileLoc.mostRecentBlock;

    // char rbuf[BLOCK_SIZE];
    // flag = Log_read(ladd, BLOCK_SIZE, rbuf);
    // if (flag) {
    //     printf("Error: File_Read\n");
    // }
    // Inode *iread = (Inode *)rbuf;
    //printf("Read inode %d of type %d owned by %d from flash @ time %s \n", iread->inum, iread->type, iread->permissions, asctime( iread->time_of_last_change));
    
    printf("Read inode %d of type %d permissions %d from flash @ time %s \n", iptr->inum, iptr->type, iptr->permissions, asctime( iptr->time_of_last_change));
    
    return 0;
}

int Inode_Create(int inum, int type, struct Inode *iptr) {

	printf("Creating inum %d of type %d\n", inum, type);
    iptr->inum = inum;
    iptr->type = type; 
    iptr->in_use = 1;

    if (type == 0) { //then file so Unix default is 664
        iptr->permissions = 664; //default for file (0) is 664, directory (1) is 775
    } else {
        iptr->permissions = 775; //default for file (0) is 664, directory (1) is 775
    }
    
    struct tm * time_of_last_change;
    time_t rawtime;
    time ( &rawtime );
    iptr->time_of_last_change = localtime( &rawtime );
    
    
    iptr->Block1Ptr = NULL;
    iptr->Block2Ptr = NULL;
    iptr->Block3Ptr = NULL;
    iptr->Block4Ptr = NULL;
    iptr->OtherBlocksPtr = NULL;

    GLOBAL_INUM++; //Katy Bug: when crashes, resets to 0, this is bad
    ifile->data.push_back(*iptr);

    //ifile->addresses.push_back() //update ifile address

    printf( "Inode %d created at: %s", inum, asctime( iptr->time_of_last_change));
    return 0;
}

int Change_File_Permissions(int inum, int permissions) {
    return 0;
}

int Change_File_Owner(int inum) {
    return 0;
}

int Change_File_Group(int inum) {
    return 0;
}

 // Creates an Inode and records it

int File_Create(int inum, int type) {
    // Need to create the inode for the file
    string filename;
    printf("((Entered File_Create( %d, %d)\n", inum, type);
    if (inum == 0) { //ifile
    	filename = ".ifile";
    }

    // Make an Inode for the file
    struct Inode *inode = new Inode;
    inode->filename = filename;
    //creates and writes Inode
    Inode_Create(inum, type, inode); 

    //sprintf(buffer, "inode_%d %x", iptr->inum, &iptr); //Xing - works when buffer is initialized
    // e.g. if I do char *buffer = "my contents";  things are fine
    
    logAddress logAddress1;
    int flag;
    u_int length = 50;
    u_int block = 1;
    void *buf = (void *) inode;


   	flag = Log_Write(inum, 2, 50, (void *)buf, logAddress1); //Xing -- replace with Inode
    if (inum == IFILE_INUM) {
        ifile->addresses.push_back(logAddress1);
    }

    if (flag) {
        printf("Error: Failed Log_Write test\n");
    } else {
        printf("Success, wrote to log at logAddress: block %d, segment %d\n", logAddress1.blockNo, logAddress1.segmentNo);
    }
    
    printf("Confirming that what File_Create wrote...\n");
    logAddress ladd;
    ladd.segmentNo = 1;
    ladd.blockNo = 3;
    
    
    char rbuf[BLOCK_SIZE];
    flag = Log_read(ladd, BLOCK_SIZE, rbuf);
    if (flag) {
        printf("Error: File_Read\n");
    }
    Inode *iread = (Inode *)buf;
    printf("Read inode %d from flash @ time %s \n", iread->inum, asctime( iread->time_of_last_change));
    
    return 0;
}



/* File_Read
Reads the contents of the blocks that the Inode "inum" points to
*/
int File_Read(int inum, int offset, int length, char* buffer) {

    // Consult inode map to get disk address for inode inum
    printf("File_Read: inum %d is type %d\n", ifile->data.at(inum).inum, ifile->data.at(inum).type);
    // Read inode into memory (if not already in buffer cache)
    // and find appropriate data pointer

    // Read the data block that is referenced by the data pointer
    // logAddress ladd;
    // ladd.segmentNo = 1;
    // ladd.blockNo = 3;
    // int flag;

    //     char buf[length];
    //     if (length > BLOCK_SIZE) {
    //         Call Log_read enoguh times 


    //         return a buffer with a pointer in it


    //         use memcpy 
    //         memcopy(buffer, pointer, i)
    //     }
    // flag = Log_read(ladd, length, buffer);
    // if (flag) {
    //     printf("Error: File_Read\n");
    // }
    // printf("Read from flash: %s\n", buffer);



    return 0;
}


/* File_Write
Assigns the block pointers of the Inode with inum "inum"
inum: the inum of the inode of the file to be accessed
offset: the starting offset of the I/O in bytes
length: length of the I/O in bytes
buffer: the I/O, what we're writing
// */
int File_Write(int inum, int offset, int length, char* buffer) {

    logAddress ladd;
    int flag;
    int ifileoffset = offset * inum;

    // Retrieve file
    Inode *fileinode = new Inode;
    Get_Inode(inum, fileinode);

    //Update Inode information
    if (length > fileinode->size) fileinode->size = length;
    printf("FILE WRITE SIZE OF INODE %d\n", sizeof(fileinode));


    flag = Log_Write(inum, offset, length, (void *)buffer, ladd); //Xing -- replace with Inode //WRite the contents
    if (flag) {
        printf("Error: Failed File_Write test\n");
    } else {
        printf("Success, wrote to log\n");
    }

    flag = Log_Write(IFILE_INUM, ifileoffset, length, (void *)buffer, ladd); //Xing -- replace with Inode // WRite the inode
    if (flag) {
        printf("Error: Failed to write inode properly\n");
    } else {
        printf("Success, wrote inodes to ifile\n");
    }


    

    // Modify file at it's offset

    // Record the changed time
    // time_t rawtime;
    // time ( &rawtime );
    // this_inode.time_of_last_change = localtime( &rawtime );

    //Check what was written
    char* rbuf;
    File_Read(inum, 0, 20, rbuf);
    char* ibuf;
    File_Read(IFILE_INUM, 0, 20, rbuf);

    return 0;
}


// int File_Free(int inum) {

// 	printf("--File for inum-%d freed\n", inum);
//     return 0;
// }

// int Test_File_Read() {
    
//     int inum = GLOBAL_INUM;
//     int offset = 3;
//     int length = 50;
//     char rbuffer[50];
    
//     if (File_Read(inum, offset, length, rbuffer) == 0) {
//         printf("SUCCESS -- File_Read worked\n");
//     } else {
//         printf("TEST FAILED -- Test_File_Write\n");
//     }

//     return 0;
// }

int Test_File_Write() {

    int inum = GLOBAL_INUM;
    int type = TYPE_F;
    int offset = 1;
    int length = INODE_SIZE;
    Inode *inode = new Inode;
    char* buffer = "henlo world";

    printf("Now creating new inum %d\n", inum);
    File_Create(inum, type);

    if (File_Write(inum, offset, length, buffer) == 0) {
        printf("SUCCESS -- File_Write worked\n");
    } else {
        printf("TEST FAILED -- Test_File_Write\n");
    }
    return 0;
}


//Test_File_Create makes an Inode but writes "hello world - Test_File_Create" to log
int Test_File_Create() { 
    int inum = IFILE_INUM;
    int type = TYPE_D;
    if (File_Create(inum, type) == 0) {
        printf("SUCCESS -- File_Create worked\n");
    } else {
        printf("TEST FAILED -- Test_File_Create\n");
    }

    int offset = 0;
    int length = 50;
    char *rbuf;
    File_Read(inum, offset, length, rbuf);
    return 1;
}

/***
before submission, a code review is necessary. 
Please first make your code no any compile bug, can pass basic tests, after that we
will do code review. Focuse on style , format , readable. 
I will optimize my code after I finish my part
*/
int main(int argc, char *argv[])
{

	// For Katy, when use log layer, fist make mklfs then can call
	//init("FuseFileSystem");   
	 // this use to init block size and segment , fuse file system
    // When you want to use function from log Layer, comment main funcion of log.C

	printf("Begin file layer...\n");
    struct Inode i;
    printf("Size of inode %d\n", sizeof(i));

	// Create the ifile
	Test_File_Create(); 

    
    // Update ifile information
    Test_File_Write();

    // Inode *iptr;
    // struct IfileLocation *ifileLoc;
    // Get_Inode(0, iptr, ifileLoc);
    // printf("Got inode %d\n", iptr->inum, ifileLoc->mostRecentSeg, ifileLoc->mostRecentBlock);
    //Test_File_Write();
    //Test_File_Read();


	// // Check that the ifile was created properly (inode made and written in self)
	// int offset = 0;
	// int length = 10;
	// char rbuf[10];
	// printf("Start reading inum: %d\n", IFILE_INUM);
	// File_Read(IFILE_INUM, offset, length, rbuf);

	// // printf("\n\n Begin file tests...\n");
	// offset = 4;
	// length = 10;
	// int thisFile = GLOBAL_INUM;
	// printf("Create this new file: %d\n", thisFile);
	// File_Create(thisFile, TYPE_F);
	// printf("File_Create completed. Inode %d exists. \n", thisFile);
	// char *wbuf = "hello world";
	// File_Write(thisFile, offset, length, wbuf);
	// printf("File_Write completed. Inode %d was writen. \n", thisFile);

	// File_Free(IFILE_INUM); //Xing

    //Change_File_Permissions(int inum, int permissions);
    //Change_File_Owner(int inum);
    //Change_File_Group(int inum);



    return 1;
}
