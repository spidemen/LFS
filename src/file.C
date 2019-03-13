#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <flash.h>
#include "log.h"
#include <time.h>
#include "file.h"


// static const char *hello_str = "Hello World!\n";
// static const char *hello_path = "/hello";
// static const char *link_path = "/link";
//static const int BUFFER_LENGTH = 16; // dunno why I have 16

int Create_Ifile() {
    FILE *fptr;
    fptr = fopen(".ifile", "w");
    if (fptr == NULL) {
        printf("Error: couldn't create .ifile\n");
        exit(1);
    }
    fclose(fptr);

    return 0;
}

int Write_To_Ifile(struct Inode *iptr) {
    FILE *fptr;
    fptr = fopen(".ifile", "w");
    if (fptr == NULL) {
        printf("Error: couldn't create .ifile\n");
        exit(1);
    }
    fprintf(fptr,"%d %x", iptr->inum, iptr);
    fclose(fptr);
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
    iptr->inum = inum;
    iptr->type = type; //
    iptr->in_use = 1;

    iptr->owner = "user";
    iptr->permissions = 0775 | 0664; //default for file (0) is 664, directory (1) is 775
    iptr->offset = 0;

    struct tm * time_of_last_change;
    time_t rawtime;
    time ( &rawtime );
    iptr->time_of_last_change = localtime( &rawtime );
    
    printf( "Time created: %s", asctime( iptr->time_of_last_change));
    
    
    return 0;
}




int Change_File_Permissions(int inum, int permissions) {
    // Make inode
    struct Inode this_inode;

    // Retrieve inode
    Get_Inode(inum, &this_inode);
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



int File_Create(int inum, int type) {
    // Need to create the inode for the file
    int i=0;
    char filename[10];
    sprintf(filename, "file_%d", i); 
    int wearLimit = 100;
    int blocks = 100;
    u_int block = 1;
    u_int length = 10;
    void* buffer[10];
    //logAddress logAddress1 = NULL;
    // Need to create an empty file 
    //Log_Write(i, block, length, buffer, NULL); //Xing

    // Make an Inode for the file
    struct Inode *iptr = NULL;
    Inode_Create(inum, type, iptr);

    FILE *fptr;
    fptr = fopen(filename, "w");
    if(fptr == NULL) {
      printf("Error: couldn't create file.\n");   
      exit(1);             
    }
    char* filecontents = "made my first file\n";

    fprintf(fptr,"%s",filecontents);
    fclose(fptr);

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
	printf("Begin file layer...\n");

    Create_Ifile();
  
    struct Inode myInode;
    struct Inode *iptr;
    iptr = &myInode;

    Inode_Create(0, 22, iptr);//&myInode);
    printf("MyInode %s\n", iptr->time_of_last_change);
    Write_To_Ifile(iptr);

    printf("Inode address: %x\n", iptr);
    
    // struct Inode *iptr2 = NULL;
    // unsigned long iptrAdd;
    // iptrAdd = Get_Inode(0, iptr2); //issues 
    // iptr2 = &iptrAdd;
    // ("Should be able to get Inode info: %d\n", iptr2->inum);


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
