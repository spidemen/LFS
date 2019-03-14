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

static const int TYPE_F = 0;
static const int TYPE_D = 1;

int Write_To_Ifile(struct Inode *iptr) {
	char *filename = ".ifile";
	u_int blocks=100;
    Flash f = Flash_Open(filename, FLASH_ASYNC, &blocks);
    
    if (f == NULL) {
        printf("Error: couldn't get Flash handle to .ifile\n");
        exit(1);
    }

    char buf[200];
    sprintf(buf, "%d|", iptr->inum);

    if(Flash_Write(f, 0, 1, (void*)buf)){ //if returns 1, error //Xing params
		printf("Write error: cannot write inode %d -- %s to flash \n", iptr->inum, iptr->filename);
	}else{
		printf("Successful Flash write. \n");
	}
    Flash_Close(f);

    return 0;
}

unsigned long Get_Inode(int inum, struct Inode *ptr) { // all inodes are stored in ifile (file at root directory (like testdir))
    
    // Call down into memory to get inode with inum
    FILE *fptr;
    fptr = fopen(".ifile", "rb");
    if (fptr == NULL) {
        printf("Error: couldn't create .ifile\n");
        exit(1);
    }

    int count = 0;
    char line[16]; 
    unsigned long ul;

    while (fgets(line, sizeof line, fptr) != NULL) {
        if (count == inum) {
            printf("File line: %s", line);
            sscanf(line, "%lx", &ul);
            printf("--The ul %x and the pointer %x\n", &ul, ptr);
            ptr = (struct Inode *) &ul;
            printf("--The ul %x and the pointer %x\n", &ul, ptr);
            printf("Ptr's stuff %d\n", (*ptr).inum); //Xing
        } 
        count++;
    }
    
    fclose(fptr);

    
    // ptr = (struct Inode *) &ul; //(void *) (uintptr_t) ul;
    // printf("Value at that address: %x\n", *ptr);
    struct kt
    {   
        int value;
        char letter;
    };
    // //int var = 722;
    // struct kt ktstruct;
    // ktstruct.value = 722;
    // ktstruct.letter = 'a';

    // struct kt *kp;
    // kp = &ktstruct;

    // printf("Address of KT variable: %x\n", &ktstruct);
    //  printf("Add stored in kp var: %x \n", kp);
    //  printf("Value of *ip variable: %d\n", (*kp).value);
    return ul;
}

int Inode_Create(int inum, int type, struct Inode *iptr) {

	printf("inum %d, type %d\n", inum, type);
    iptr->inum = inum;
    iptr->type = type; //
    iptr->in_use = 1;
    iptr->owner = "user";
    iptr->permissions = 0775 | 0664; //default for file (0) is 664, directory (1) is 775

    struct tm * time_of_last_change;
    time_t rawtime;
    time ( &rawtime );
    iptr->time_of_last_change = localtime( &rawtime );
    printf( "Inode %d created at: %s", inum, asctime( iptr->time_of_last_change));
    
    iptr->Block1Ptr = NULL;
    iptr->Block2Ptr = NULL;
    iptr->Block3Ptr = NULL;
    iptr->Block4Ptr = NULL;
    iptr->OtherBlocksPtr = NULL;

    Write_To_Ifile(iptr);
    printf("Inode %s written to ifile\n", iptr->filename);
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

int File_Create(int inum, int type) {
    // Need to create the inode for the file
    char *filename;
    if (inum == 0) { //ifile
    	filename = ".ifile";
    } else {
    	sprintf(filename, "file_%d", inum); 
    }
    
    int wearLimit = 100;
	int blocks = 100;
	u_int block = 1;
	u_int length = 10;
    
    // Create the file in the Flash layer
    Flash_Create(filename, wearLimit, blocks);

    // Make an Inode for the file
    printf("Make Inode for %s...\n", filename);
    struct Inode *iptr = new Inode;
    iptr->filename = filename;
    Inode_Create(inum, type, iptr);
    
    // char *buffer;
    // sprintf(buffer, "%d %x", iptr->inum, *iptr);
    // logAddress *logAddress1;
   	// Log_Write(inum, block, length, buffer, &logAddress1); //Xing
    
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

int File_Read(int inum, int offset, int length, char* buffer) {

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
	printf("Begin file layer...\n");
	int inum = 0;
    File_Create(inum, TYPE_F);
    inum++;
  
  	printf("\n\n Begin file tests...\n");
    File_Create(inum, TYPE_F);
    int offset = 0;
    int length = 12;
    char* buffer = "hello katy";
    File_Write(inum, offset, length, buffer);
    File_Read(inum, offset, length, buffer);


    //Change_File_Permissions(int inum, int permissions);
    //Change_File_Owner(int inum);
    //Change_File_Group(int inum);






    
    // Create a file
    /*int isFile = 1;
	File_Create(1,isFile);
	printf("File created\n");

	char buffer[BUFFER_LENGTH]; //none type pointer later
    for (int i=0; i<10; i++) {
        buffer[i] = 'k';
    }
    buffer[10] = '\0';
    
    File_Write(1, 1, 1, buffer);
    printf("File written.\n");
    
    File_Read(1, 1, 1, 1);
    cout << "File read. ";
    
    File_Free(1);
    cout << "File freed. \n";*/

    return 1;
}
