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


int Write_To_Ifile(struct Inode *iptr) {
	char *filename = (*iptr).filename;
    char* buffer = "writing ifile";
    int inum = (*iptr).inum;
    u_int blocks = 100;
    Flash f = Flash_Open(filename, FLASH_ASYNC, &blocks);
    
    if (f == NULL) {
        printf("----Error: couldn't get Flash handle to .ifile\n");
        exit(1);
    }

    char buf[200];
    sprintf(buf, "%d|%s", inum, filename);

    if(Flash_Write(f, 0, 1, (void*)buf)){ //if returns 1, error //Xing params
		printf("----Write error: cannot write inode %d -- %s to flash \n", iptr->inum, iptr->filename);
	}else{
		printf("--Successful Flash write. \n");
	}
    Flash_Close(f);
    return 0;
}

unsigned long Get_Inode(int inum, struct Inode *iptr) { // all inodes are stored in ifile (file at root directory (like testdir))
   	u_int blocks = 100;
    // Call down into memory to get inode with inum
    Flash f = Flash_Open("./.ifile", FLASH_ASYNC, &blocks);
    if (f == NULL) {
        printf("----Error: couldn't get Flash handle to .ifile\n");
        exit(1);
    }
    printf("--Trying to get contents of ifile\n");
    char *buf;
    u_int sector = 0; // starting offset, in sectors
    u_int count = 1; // # of sectors to read
    if(Flash_Read(f, 0, 1, (void*)buf)){ //if returns 1, error //Xing params
		printf("----Read error: cannot read inode %d -- %s to flash \n", iptr->inum, iptr->filename);
	}else{
		printf("--Successful Flash read. \n");
	}
    Flash_Close(f);

    printf("--Buffer contents: %s", buf);



    unsigned long ul;

    // while (fgets(line, sizeof line, fptr) != NULL) {
    //     if (count == inum) {
    //         printf("File line: %s", line);
    //         sscanf(line, "%lx", &ul);
    //         printf("--The ul %x and the pointer %x\n", &ul, ptr);
    //         ptr = (struct Inode *) &ul;
    //         printf("--The ul %x and the pointer %x\n", &ul, ptr);
    //         printf("Ptr's stuff %d\n", (*ptr).inum); //Xing
    //     } 
    //     count++;
    // }
    
    // fclose(fptr);
    return ul;
}

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

/* Creates an Inode and records it
*/
int File_Create(int inum, int type) {
    // Need to create the inode for the file
    printf("((Entered File_Create( %d, %d)\n", inum, type);
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

    char buffer[20];
    sprintf(buffer, "inode_%d %x", iptr->inum, &iptr); //Xing - works when buffer is initialized
    // e.g. if I do char *buffer = "my contents";  things are fine
    
    printf("--My buffer contents: %s\n", buffer);
    // char *buffer;
    // sprintf(buffer, "%d %x", iptr->inum, *iptr);
    // logAddress *logAddress1;
   	// Log_Write(inum, block, length, buffer, &logAddress1); //Xing
   	char *flashname = "myflash";
   	u_int blocks = 100;
   	Flash f = Flash_Open(flashname, FLASH_ASYNC, &blocks);
   	if (f != NULL) {
   		printf("We have a Flash handle\n");
   	}

   	u_int sector = inum;
   	u_int count = 1;
   	if (Flash_Write(f, sector, count, buffer) == 0) {
   		printf("Inode %d for file was written properly\n", iptr->inum);
   		printf("-- Contents written: %s\n", buffer);
   	} else {
   		printf("Uh oh with Flash_Write\n");
   	}

    Flash_Close(f);
    return 0;
}

/* File_Write
Assigns the block pointers of the Inode with inum "inum"
*/
int File_Write(int inum, int offset, int length, char* buffer) {
    char *filename = ".ifile";
    u_int blocks=100;
    Flash f = Flash_Open(filename, FLASH_ASYNC, &blocks);
    
    if (f == NULL) {
        printf("Error: couldn't get Flash handle to .ifile\n");
        exit(1);
    }

    char buf[20];
    sprintf(buf, "%d|", inum);

    logAddress *logAddress1;

    u_int sector = inum;
    u_int count = inum;
    if(Flash_Write(f, sector, count, (void*)buf)){ //if returns 1, error //Xing params
		printf("Write error: cannot write inode %d to flash \n", inum);
	}else{
		printf("Successful Flash write. \n");
	}
    Flash_Close(f);
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

    char *flashname = "myflash";
    u_int blocks = 100;
    Flash f = Flash_Open(flashname, FLASH_ASYNC, &blocks);
   	if (f == NULL) {
   		printf("----We have an issue getting the Flash handle\n");
   	}

   	u_int sector = inum;
    u_int count = 1;
    if (Flash_Read(f, sector, count, buffer) == 0) {
    	printf("--We can read: %s\n", buffer);
    } else {
    	printf("----No reading :(\n");
    }	

    Flash_Close(f);
    return 0;
}

int File_Free(int inum) {

	printf("--File for inum-%d freed\n", inum);
    return 0;
}


int main(int argc, char *argv[])
{
	printf("Begin file layer...\n");
	char *flashname = "myflash";
	u_int wearLimit = 100;
	u_int blocks = 100;
	Flash_Create(flashname, wearLimit, blocks);


	// Create the ifile
	File_Create(IFILE_INUM, TYPE_F); 

	// Check that the ifile was created properly (inode made and written in self)
	int offset = 0;
	int length = 10;
	char rbuf[10];
	printf("Start reading inum: %d\n", IFILE_INUM);
	File_Read(IFILE_INUM, offset, length, rbuf);

	// printf("\n\n Begin file tests...\n");
	offset = 10;
	length = 10;
	int thisFile = GLOBAL_INUM;
	printf("Create this new file: %d\n", thisFile);
	File_Create(thisFile, TYPE_F);
	printf("File_Create completed. Inode %d exists. \n", thisFile);
	//char *wbuf;
	//File_Write(thisFile, offset, length, wbuf);
	//printf("File_Write completed. Inode %d was writen. \n", thisFile);

	File_Free(IFILE_INUM); //Xing

    //Change_File_Permissions(int inum, int permissions);
    //Change_File_Owner(int inum);
    //Change_File_Group(int inum);*/



    return 1;
}
