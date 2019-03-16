#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <flash.h>
#include "log.h"
#include <time.h>
#include "file.h"
int GLOBAL_INUM = 0;
static const int IFILE_INUM = 0;
static const int TYPE_F = 0;
static const int TYPE_D = 1;
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
// int Write_To_Ifile(struct Inode *iptr) {
// 	char *filename = (*iptr).filename;
//     char* buffer = "hello world";
//     int inum = (*iptr).inum;
//     u_int blocks = 100;
//     Flash f = Flash_Open(filename, FLASH_ASYNC, &blocks);
    
//     if (f == NULL) {
//         printf("----Error: couldn't get Flash handle to .ifile\n");
//         exit(1);
//     }

//     char buf[200];
//     sprintf(buf, "%d|%s", inum, filename);

//     if(Flash_Write(f, 0, 1, (void*)buf)){ //if returns 1, error //Xing params
// 		printf("----Write error: cannot write inode %d -- %s to flash \n", iptr->inum, iptr->filename);
// 	}else{
// 		printf("--Successful Flash write. \n");
// 	}
//     Flash_Close(f);
//     return 0;
// }

// int Get_Inode(int inum, struct Inode *iptr) { // all inodes are stored in ifile (file at root directory (like testdir))
//    	u_int blocks = 100;
//     // Call down into memory to get inode with inum
//     Flash f = Flash_Open("./.ifile", FLASH_ASYNC, &blocks);
//     if (f == NULL) {
//         printf("----Error: couldn't get Flash handle to .ifile\n");
//         exit(1);
//     }
//     printf("--Trying to get contents of ifile\n");
//     char *buf;
//     u_int sector = 0; // starting offset, in sectors
//     u_int count = 1; // # of sectors to read
//     if(Flash_Read(f, 0, 1, (void*)buf)){ //if returns 1, error //Xing params
// 		printf("----Read error: cannot read inode %d -- %s to flash \n", iptr->inum, iptr->filename);
// 	}else{
// 		printf("--Successful Flash read. \n");
// 	}
//     Flash_Close(f);

//     printf("--Buffer contents: %s", buf);



//     // while (fgets(line, sizeof line, fptr) != NULL) {
//     //     if (count == inum) {
//     //         printf("File line: %s", line);
//     //         sscanf(line, "%lx", &ul);
//     //         printf("--The ul %x and the pointer %x\n", &ul, ptr);
//     //         ptr = (struct Inode *) &ul;
//     //         printf("--The ul %x and the pointer %x\n", &ul, ptr);
//     //         printf("Ptr's stuff %d\n", (*ptr).inum); //Xing
//     //     } 
//     //     count++;
//     // }
    
//     // fclose(fptr);
//     return 0;
// }

int Inode_Create(int inum, int type, struct Inode *iptr) {

	printf("Creating inum %d of type %d\n", inum, type);
    iptr->inum = inum;
    iptr->type = type; 
    iptr->in_use = 1;
    iptr->owner = "user";
    iptr->permissions = 0775 | 0664; //default for file (0) is 664, directory (1) is 775

    struct tm * time_of_last_change;
    time_t rawtime;
    time ( &rawtime );
    iptr->time_of_last_change = localtime( &rawtime );
    
    
    iptr->Block1Ptr = NULL;
    iptr->Block2Ptr = NULL;
    iptr->Block3Ptr = NULL;
    iptr->Block4Ptr = NULL;
    iptr->OtherBlocksPtr = NULL;

    GLOBAL_INUM++;

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
    char *filename;
    printf("((Entered File_Create( %d, %d)\n", inum, type);
    if (inum == 0) { //ifile
    	filename = ".ifile";
    } else {
    	printf("-- make a diff finode entry\n", inum, type);
    	sprintf(filename, "file_%d", inum); 
    }

    // Make an Inode for the file
    printf("Make Inode for %s...\n", filename);
    struct Inode *iptr = new Inode;
    iptr->filename = filename;
    //creates and writes Inode
    Inode_Create(inum, type, iptr); 

    string buffer = "hello world - Test File_Create";
    const char * bufwrite = buffer.c_str();
    //sprintf(buffer, "inode_%d %x", iptr->inum, &iptr); //Xing - works when buffer is initialized
    // e.g. if I do char *buffer = "my contents";  things are fine
    

    logAddress logAddress1;
    int flag;
   	flag = Log_Write(1, 2, 50, (void *)bufwrite, logAddress1); //Xing -- replace with Inode
    if (flag) {
        printf("Error: Failed Log_Write test\n");
    } else {
        printf("Success, wrote to log\n");
    }

    return 0;
}

/* File_Write
Assigns the block pointers of the Inode with inum "inum"
*/
int File_Write(int inum, int offset, int length, const char* buffer) {

    logAddress ladd;
    int flag;
    flag = Log_Write(inum, offset, length, (void *)buffer, ladd); //Xing -- replace with Inode
    if (flag) {
        printf("Error: Failed File_Write test\n");
    } else {
        printf("Success, wrote to log\n");
    }


    // Retrieve inode

    // Retrieve file

    // Modify file at it's offset

    // Record the changed time
    // time_t rawtime;
    // time ( &rawtime );
    // this_inode.time_of_last_change = localtime( &rawtime );

    return 0;
}

/* File_Read
Reads the contents of the blocks that the Inode "inum" points to
*/
int File_Read(int inum, int offset, int length, char* buffer) {

    // Consult inode map to get disk address for inode inum

    // Read inode into memory (if not already in buffer cache)
    // and find appropriate data pointer

    // Read the data block that is referenced by the data pointer
    logAddress ladd;
    ladd.segmentNo = 1;
    ladd.blockNo = 3;
    int flag;
    
    flag = Log_read(ladd, length, buffer);
    if (flag) {
        printf("Error: File_Read\n");
    }
    printf("Read from flash: %s\n", buffer);


    return 0;
}

// int File_Free(int inum) {

// 	printf("--File for inum-%d freed\n", inum);
//     return 0;
// }

int Test_File_Read() {
    
    int inum = GLOBAL_INUM;
    int offset = 3;
    int length = 50;
    char rbuffer[50];
    
    if (File_Read(inum, offset, length, rbuffer) == 0) {
        printf("SUCCESS -- File_Read worked\n");
    } else {
        printf("TEST FAILED -- Test_File_Write\n");
    }

    return 0;
}

int Test_File_Write() {
    int inum = GLOBAL_INUM;
    int offset = 4;
    int length = 20;
    string sbuffer = "katy morning";
    const char *buffer = sbuffer.c_str();
    if (File_Write(inum, offset, length, buffer) == 0) {
        printf("SUCCESS -- File_Write worked\n");
    } else {
        printf("TEST FAILED -- Test_File_Write\n");
    }
    return 0;
}


// Test_File_Create makes an Inode but writes "hello world - Test_File_Create" to log
int Test_File_Create() { 
    int inum = GLOBAL_INUM;
    int type = 0;
    if (File_Create(inum, type) == 0) {
        printf("SUCCESS -- File_Create worked\n");
    } else {
        printf("TEST FAILED -- Test_File_Create\n");
    }
    return 0;
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

	// Create the ifile
	Test_File_Create(); 
    Test_File_Write();
    Test_File_Read();


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
