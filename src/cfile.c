#include <stdio.h>
#include "../include/cfile.h"

//00 is false (but means things ran normally)
// 1 is true

#define GLOBAL_INUM 0
#define IFILE_INUM 0
#define TYPE_F 0
#define TYPE_D 0
#define SIZEOF_INODE sizeof(struct Inode)

struct Ifile LFSlog[1]; //representing writing the ifile's data
struct Ifile ifile; //representing writing files to the ifile

struct Ifile Get_Ifile() {
	
	return LFSlog[IFILE_INUM];
}

int Put_Ifile(struct Ifile ifile) {
	LFSlog[IFILE_INUM] = ifile;
	return 0;
}

int Get_Inode(int inum, struct Inode *iptr) {
	// struct Ifile ifile;
	// Get_Ifile(&ifile);
	// int ifileSize = ifile.size;
	if (inum <= 10) {
		iptr = &(ifile.data[inum]);
	} else {
		printf("Error: cannot get inum %d from ifile (size %d)\n", inum, 10 );
		return 1;
	}
	return 0;
}

int Put_Inode(int inum, struct Inode iptr) {
	ifile.data[inum] = iptr;
	return 0;
}


int File_Create(int inum, int type) {
	if (inum == IFILE_INUM) 
	{
		
		struct Inode dummyInode = {
			.inum = IFILE_INUM,
			.type = type,
			.in_use = 1,					// true 1, false 0
    		.time_of_last_change = (unsigned long)time(NULL),
    		.owner = 'u',
    		.permissions = 777,
    		.Block1Ptr = {
    			.blockNo = 0,
    			.segmentNo = 0
    		}
		};
		struct logAddress dummyLogAdd = { .blockNo = 0, .segmentNo = 0};
		struct Ifile ifile = {
			.inum = inum,
			.size = 1, 	//number of files currently in the ifile
			.addresses[0] = dummyLogAdd,
			.data[0] = dummyInode
		};


		
		//fprintf(stdout, "Time created (s): %lu\n", (unsigned long)time(NULL)); 
		//printf("File created at %lu\n", dummyInode.time_of_last_change);
		int block = 0; //start of file
		int length = sizeof(ifile);
		struct logAddress logAdd;
		Log_Write(IFILE_INUM, block, length, (void *) &ifile, &logAdd);
		LFSlog[IFILE_INUM] = ifile; //delete this, use above line
		printf("     File %d created at block %d  segment %d\n", inum, logAdd.blockNo, logAdd.segmentNo);
		
	}
	else 
	{
		struct Inode inode = {
			.inum = inum,
			.type = type,
			.in_use = 1,					// true 1, false 0
    		.time_of_last_change = (unsigned long)time(NULL),
    		.owner = 'u',
    		.permissions = 777,
    		.Block1Ptr = {
    			.blockNo = 0,
    			.segmentNo = 0
    		}
		};

		int block = 0; //start of file
		int length = sizeof(inode);
		struct logAddress logAdd;
		Log_Write(inum, block, length, (void *)&inode, &logAdd); 
		//ifile[inum-1] = inode; //delete this, use above line

		// Update ifile metadata
		
//		struct Ifile ifile = Get_Ifile();
		int i = ifile.size;
		ifile.data[inum] = inode;
		ifile.size++;
		Put_Ifile(ifile);
		printf("     File %d created at block %d  segment %d\n", inum, logAdd.blockNo, logAdd.segmentNo);
		
	}

	return 0;
}

int File_Write(int inum, int offset, int length, char* buffer) {

	int flag;

	// Convert offset to blocks
	int block = offset/BLOCK_SIZE;
	printf("Writing at block %d within the file...\n", block);

	struct Inode fileinode;
	Get_Inode(inum, &fileinode);
	printf(" -- Inode info: inum %d at block %d  segment %d\n", fileinode.inum, fileinode.Block1Ptr.blockNo, fileinode.Block1Ptr.segmentNo);
	struct Ifile ifile = Get_Ifile();
	printf(" -- IFile info: size %d\n", ifile.size);

	struct logAddress pRead = fileinode.Block1Ptr;
	char rbuf[BLOCK_SIZE];
	char content[BLOCK_SIZE];
	memset(content, '0x00', BLOCK_SIZE);
	char* writebuf=content;
	struct logAddress dataAdd;

	if (pRead.segmentNo!=0) {
		if (!Log_read(pRead, BLOCK_SIZE, rbuf)) { //add
			// Case 1: we only need to read one block
			if (offset+length < BLOCK_SIZE) {
				 printf("file debug read log \n");
				char* rPoint = rbuf;
				memcpy(writebuf, rbuf, offset);
				writebuf += offset;
				memcpy(writebuf, buffer, length);
				writebuf += length - 1;
				rPoint = rPoint + offset + length - 1;
				memcpy(writebuf, rPoint, BLOCK_SIZE-offset-length);

				if (!Log_Write(inum, 1, length, (void *) content, &dataAdd)) {
					ifile.data[inum].Block1Ptr = dataAdd;
				} else {
					printf("File: write error, cannot write %s\n", writebuf);
				}
			}
		} else {
				printf("File: read error, cannot read inum %d\n", inum);
		}
	} else {
		// The segment number is 0
		 printf("file debug write\n");
		if (offset + length < BLOCK_SIZE) {
			writebuf += offset;
			memcpy(writebuf, buffer, length);
			if (!Log_Write(inum, 1, length, (void *) writebuf, &dataAdd)) {
				ifile.data[inum].Block1Ptr = dataAdd;
			 printf("write file debug segmentNo=%d  block number=%d \n",dataAdd.segmentNo,dataAdd.blockNo);
			} else {
				printf("File: write error, cannot write %s\n", writebuf);
			}
		}

	}

	// Update the inode data
	fileinode.size += length;
	fileinode.block_number =pRead.blockNo; //What is inode.block_number?
	fileinode.time_of_last_change = (unsigned long)time(NULL);

	// Write the inode data
	Put_Inode(inum, fileinode);



	return 0;
}
int File_Read(int inum, int offset, int length, char* buffer) {
	
	//Log_Read
	
	struct Inode iptr;
	Get_Inode(inum, &iptr);
	iptr=ifile.data[inum];
	printf("Read in inode %d\n", iptr.inum);

	struct logAddress ladd;
	ladd.blockNo = iptr.Block1Ptr.blockNo;
	ladd.segmentNo = iptr.Block1Ptr.segmentNo;
	printf("file debug segmentNo=%d  block number=%d \n",ladd.segmentNo,ladd.blockNo);

	char content[length];
	if (!Log_read(ladd, length, content)) {
		memcpy(buffer, content, length);
	} else {
		printf("File_Read: error with Log_Read of file %d\n", inum);
	}

	return 0;
}

int File_Free(int inum) {
	struct Inode inode;
	Get_Inode(inum, &inode);
	inode.in_use = 0; //Inode is no longer in use

}




int Test_File_Create(int inum) {
	printf("############ begin Test_File_Create ############\n");
	int type = TYPE_F;

	if (File_Create(inum, type) == 0) {
		printf("SUCCESS -- File Create worked\n");
	} else {
		printf("FAILED -- File Create of inum %d\n", inum);
	}

	//
	struct Ifile* ifile = &(LFSlog[0]); //delete this and replace with above line

	printf("Ifile contents (%d): \n", ifile->size);
    for (int i=0; i<=ifile->size; i++) {
        struct Inode thisInode = ifile->data[i];
        printf("inum: %d has data at block %d and seg %d \n", thisInode.inum); //, thisInode.Block1Ptr.blockNo, thisInode.Block1Ptr.segmentNo);
    }
	return 0;
}

int initFile() {
	 // Create ifile inode, add it to the ifile data structure,
    // and write the ifile data structure to disk

    init("FuseFileSystem");  

    // Create the Ifile 
 //   File_Create(IFILE_INUM, TYPE_F);

	return 0;
}

void test1F(){
	printf("*******************File layer test 4 simple small write and read ******************************\n");
	char *buf="Hello LFS, welcome to CSC 545 OS class";
	inum num=1;
	if(!File_Write(num, 0, 40, (void*)buf)){
	    char bufR[40];
	    if(!File_Read(num, 0, 40, (void*)bufR)){
	    	if(strcmp(buf,bufR)!=0){
				printf("Fail:  write string %s does not match read string %s \n",buf,bufR);
			}else{
				printf("**************Success    test 4 pass*******************************\n");
			}
	    }
	}
}
int main(){
	printf("Begin cfile layer, creating ifile (and its inode)...\n");
   initFile();
    Test_File_Create(1);
   // Test_File_Create(2);
    test1F();
    // int size = 10;
    // char buffer[BLOCK_SIZE];
    // memcpy(buffer, "abcdefghij", ( size * sizeof('a')));
    // File_Write(1, 0, size, buffer);


    // File_Read(1, 0, size, buffer); 

	return 0;
}