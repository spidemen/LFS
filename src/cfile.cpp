#include <stdio.h>
#include <sys/stat.h>
#include <iostream>
#include <ctime>
#include "../include/cfile.h"

#include <vector>
using namespace std;
//00 is false (but means things ran normally)
// 1 is true


#define SIZEOF_INODE sizeof(struct Inode)
#define SEGMENT_THRESHOLD 3
#define ARRAY_SIZE 4

#define MAX_SIZE 800

struct Ifile {
    vector<struct Inode> data; //location of Inode == inum, should be vector<Inode>
};

struct IfileWrite {
 	struct Inode data[ARRAY_SIZE];
    int size;
};

struct Ifile IfileArray;
struct IfileWrite IfileWrite; 


int File_Get(int inum, struct Inode *node) {
	//printf(" File get %d from Ifile of size %d\n", inum, IfileArray.data.size());
	int availInodes = IfileArray.data.size();
	if (inum > availInodes) {
		// Asking for an Inum that we don't have
		printf("Error: Inode %d does not exist (out of bounds, %d)\n", inum, availInodes);
		return 3;
	} else {
		if (IfileArray.data[inum].in_use == 0) {
			// Inode did exist but is no longer in use
			printf("Error: inode %d has been deleted\n");
			return 2;
		}
		memcpy(node,&IfileArray.data[inum],sizeof(struct Inode));
	//	return IfileArray.data[inum];
		return 0;
	}
	

	//return 1;
}


int File_Naming(int inum, const char *path, const char *filename,struct stat *stbuf){

	int availInodes = IfileArray.data.size();
	if (inum > availInodes) {
		// Asking for an Inum that we don't have
		printf("Error: Inode %d does not exist (out of bounds, %d)\n", inum, availInodes);
		return 3;
	} else {
		if (IfileArray.data[inum].in_use == 0) {
			// Inode did exist but is no longer in use
			printf("Error: inode %d has been deleted\n");
			return 2;
		}
		printf("Returning the inode %d...\n", inum);

     	memcpy(IfileArray.data[inum].path,path,50);
     	memcpy(IfileArray.data[inum].filename,filename,50);

   		cout << "Named file: " << IfileArray.data[inum].path << " " << IfileArray.data[inum].filename <<endl;
		return 0;
	}

}


struct Inode initInode(int inum) {
	struct Inode inode;
	inode.inum = inum;
	return inode;
}



int Put_Inode(int inum, struct Inode* iptr) {
	if (inum < IfileArray.data.size()) {
		IfileArray.data[inum] = *iptr;
	} else {
		IfileArray.data.push_back(*iptr);
	}
	IfileWrite.size = IfileArray.data.size();

	printf("Putting inode %d data, block %d seg %d\n", iptr->inum, iptr->Block1Ptr.blockNo, iptr->Block1Ptr.segmentNo);

	return 0;
}

void Show_Ifile_Contents() {
	printf("############ start Show_Ifile_Contents ############\n");
	printf("Ifile contents (%d): \n", IfileArray.data.size());
    for (int i=1; i<IfileArray.data.size(); i++) {
        //struct Inode thisInode = ifile->data[i];
        struct Inode thisInode = IfileArray.data[i];
        if (thisInode.in_use) {
	        printf("inum: %d (size: %d) has data1 at block %d and seg %d \n", thisInode.inum, thisInode.size, thisInode.Block1Ptr.blockNo, thisInode.Block1Ptr.segmentNo);
	     	if (thisInode.Block2Ptr.blockNo != 0) { 
	     		printf(  "                       data2 at block %d and seg %d \n", thisInode.Block2Ptr.blockNo, thisInode.Block2Ptr.segmentNo);
	     	}
	     	if (thisInode.Block3Ptr.blockNo != 0) { 
	     		printf(  "                       data3 at block %d and seg %d \n", thisInode.Block3Ptr.blockNo, thisInode.Block3Ptr.segmentNo);
	     	}
	     	if (thisInode.Block4Ptr.blockNo != 0) { 
	     		printf(  "                       data4 at block %d and seg %d \n", thisInode.Block4Ptr.blockNo, thisInode.Block4Ptr.segmentNo);
	     	}
	     	if (thisInode.OtherBlocksPtr.blockNo != 0) { 
	     		printf(  "               and more data at block %d and seg %d \n", thisInode.OtherBlocksPtr.blockNo, thisInode.OtherBlocksPtr.segmentNo);
	     	}
     	}
        
    }
    printf("############ end Show_Ifile_Contents ############\n");
	return;
}

void Print_Inode(int inum) {
	struct Inode in = IfileArray.data[inum];
	printf("%s %d %c %c  %7d  %s %s\n", in.permissions, in.nlink, in.owner, in.group, in.size, in.mtime, in.filename);
}


void WriteIfle(){
	struct IfileWrite writeTest;
    int i=0;
    int arraySize=0;
    if(IfileArray.data.size()%ARRAY_SIZE==0){
    	arraySize=IfileArray.data.size();
    }else{
    	arraySize=IfileArray.data.size()+1;
    }
    logAddress *newAdd=(struct logAddress *) malloc(sizeof(struct logAddress)*arraySize);
    int startIndex=0;
    int oldsize=1;
     logAddress tmp,oldAdd;
    for(i=0;i<IfileArray.data.size();i++){
    	writeTest.data[i%ARRAY_SIZE]=IfileArray.data[i];
        if(i%ARRAY_SIZE==(ARRAY_SIZE-1)){
           // writeTest.size=4;
        	writeTest.size= ARRAY_SIZE;
        	printf("Checkpoint: index i=%d writing to ckpt size %d \n", i,writeTest.size);
           Log_Write(IFILE_INUM, 0, BLOCK_SIZE-100, &writeTest, &tmp);
          cout<<" Ifilw write  "<<startIndex+1<<"segmentNo ="<<tmp.segmentNo<<" blockNo "<<tmp.blockNo<<endl;
           newAdd[startIndex++]=tmp;
           //Log_GetIfleAddress(&oldAdd, oldsize);
           oldsize = Log_GetIfleAddress(&oldAdd, oldsize);
           Log_CheckPoint(&oldAdd,newAdd,oldsize,startIndex);
  
    	   memset(&writeTest,0,sizeof(IfileWrite));
        }

    } 

   if(i%ARRAY_SIZE!=0){
    writeTest.size= IfileArray.data.size()%ARRAY_SIZE;//(i)%4;
 
   logAddress newAddress;
   Log_Write(IFILE_INUM, 0, BLOCK_SIZE-100, &writeTest, &newAddress);
    cout<<" Ifilw write "<<startIndex+1<<" segmentNo ="<<newAddress.segmentNo<<" blockNo "<<newAddress.blockNo<<endl;
   newAdd[startIndex++]=newAddress;
   //Log_GetIfleAddress(&oldAdd, oldsize);
 
   Log_CheckPoint(&oldAdd,newAdd,oldsize,startIndex);
   oldsize = Log_GetIfleAddress(&oldAdd, oldsize);

   cout << "after  Number of times parts of Ifle were written "<< startIndex<< " oldsize"<<oldsize<<endl;
  }
}

int File_Create(int inum, int type) {
	int checkpointflag = 0;
	if (inum == IFILE_INUM) 
	{
		// First check if ifile already exists
		struct logAddress ifileAdd[20];
		int ifilesize = Log_GetIfleAddress(&ifileAdd[0], 1);
		cout << " ~~~~Ifile size (in blocks) " <<ifilesize <<endl;
		if (Log_GetIfleAddress(&ifileAdd[0], 1) == 0){
			// None exists
			printf("##Creating new ifile at block %d, segment %d\n", ifileAdd[0].blockNo, ifileAdd[0].segmentNo);
		} 
		else {
			// Already exists, do Log Read to get ifile
			printf("##Ifile already exists starting (%d blocks) at block %d, segment %d\n", ifilesize, ifileAdd[0].blockNo, ifileAdd[0].segmentNo);
			for (int i=0; i<ifilesize; i++) {
				printf("  get ifile part from block %d seg %d \n", ifileAdd[i].blockNo, ifileAdd[i].segmentNo);
				char ifilecontents[BLOCK_SIZE-100];
			 	Log_read(ifileAdd[i], BLOCK_SIZE-100, ifilecontents); 
		     	struct IfileWrite *tmpifile;
		     	tmpifile=(struct IfileWrite *)ifilecontents;

			 	cout<<" read Ifile size "<<tmpifile->size<<endl;
		     	for(int i=0;i<tmpifile->size;i++){ //KATY only works if i<4
		     		//if (i%ARRAY_SIZE == ARRAY_SIZE-1)
		     		cout<<"degug Read Ifle  inum="<<tmpifile->data[i].inum<<" owner "<<tmpifile->data[i].owner<<endl;
		     		IfileArray.data.push_back(tmpifile->data[i]);
		     		printf("pushed back %d\n",i);
		     	}	
	      	}
	      	printf("done reading %d inodes\n", IfileArray.data.size());
		     

		     // write Ifle 	
		  	return 0;
		}
		struct Inode dummyInode = initInode(inum);    // DONE -- FIXME: C++ can assign default vaule when define a data structure , take a look at log.cpp 
		//dummyInode.inum = IFILE_INUM;
		dummyInode.type = type;
		char ifilename[7] = ".ifile";
		memcpy(dummyInode.filename, ifilename, 7);
		if (type == TYPE_F) dummyInode.nlink = 1;
		if (type == TYPE_D) dummyInode.nlink = 2;
		// current date/time based on current system
	   	time_t now = time(0);
	   	dummyInode.mtime = time(&now);
	   	dummyInode.atime = time(&now);
	   	dummyInode.ctime = time(&now);

	   	dummyInode.owner = getuid();
	   	dummyInode.group = getgid();
	   	dummyInode.size = 0;

		//Write the inode
		int block = 0; //start of file
		int length = sizeof(dummyInode);
		unsigned char *contents[sizeof(struct Inode)];
		memcpy(contents, &dummyInode, sizeof( struct Inode));

		struct logAddress logAdd;
		printf("dummyInode ----%d\n", dummyInode.inum);
		Log_Write(inum, block, length, (void *) contents, &logAdd);
		
		Put_Inode(inum, &dummyInode);

		printf("     IFile %d created at block %d  segment %d\n", inum, logAdd.blockNo, logAdd.segmentNo);
		if (logAdd.segmentNo % SEGMENT_THRESHOLD == 0) {
				printf("********* SEGMENT_THRESHOLD REACHED *****\n");
				checkpointflag = 1;
		}
		if (checkpointflag) WriteIfle();
	}
	else 
	{	
		printf("inum %d   ifilearray.data.size %d\n", inum, IfileArray.data.size());
		if (inum < IfileArray.data.size()) {
			printf("ERROR: File_Create -- inum %d already exists.\n", inum);
			return 1; 
		}
		struct Inode inode = initInode(inum);  // DONE-- FIXME: C++ can assign default vaule when define a data structure , take a look at log.cpp 
		//inode.inum = inum;
		inode.type = type;
		if (type == TYPE_F) {
			inode.nlink = 1;
			inode.permissions = S_IFREG | 0774;
			printf("F _-Permissions/mode %d\n", inode.permissions);
		}
		if (type == TYPE_D) {
			inode.nlink = 2;
			inode.permissions = S_IFDIR | 0700;
			printf("D _-Permissions/mode %d\n", inode.permissions);
		}
	    // current date/time based on current system
	   	time_t now = time(0);
	   	inode.mtime = time(&now);
	   	inode.atime = time(&now);
	   	inode.ctime = time(&now);

	   	inode.owner = getuid();
	   	inode.group = getgid();
	   	inode.size = 0;


		int block = 0; //start of file
		int length = sizeof(inode);

		unsigned char *contents[sizeof(struct Inode)];
		memcpy(contents, &inode, sizeof( struct Inode));
		struct logAddress logAdd;
		printf("Inode ----%d   inode size: %d\n", inode.inum, sizeof(inode));
		Log_Write(inum, block, length, (void *) contents, &logAdd);
	
		Put_Inode(inum, &inode);
		printf("    We put inode %d and so size of Ifile is %d\n", inum, IfileArray.data.size());
		if (logAdd.segmentNo % SEGMENT_THRESHOLD == 0) {
				printf("********* SEGMENT_THRESHOLD REACHED *****\n");
				checkpointflag = 1;
		}
		if (checkpointflag) WriteIfle();

	}


	return 0;
}

int File_Write(int inum, int offset, int length, void* buffer) {

	int checkpointflag = 0;

	// Convert offset to blocks
	int block = offset/MAX_SIZE;
	printf("Writing at block %d within file %d...\n", block, inum);
	int numBlocks = 1 + (((offset+length) - 1) / MAX_SIZE);
	printf("Expected space required to write: %d blocks (for %d bytes at %d offset and block size %d) \n", numBlocks, length, offset, MAX_SIZE);

	struct Inode fileinode;
	printf("IfileArray.data.size()) %d\n", IfileArray.data.size());
	fileinode = IfileArray.data[inum];//Get_Inode(inum);
	printf(" -- Inode info: inum %d at block %d  segment %d\n", fileinode.inum, fileinode.Block1Ptr.blockNo, fileinode.Block1Ptr.segmentNo);
	printf(" --      file size: %d\n", fileinode.size);
	int rsize = fileinode.size;
	
	struct logAddress pRead = fileinode.Block1Ptr;
	printf("logAddress to read from: block %d   segment %d\n", pRead.blockNo, pRead.segmentNo);
	struct logAddress dataAdd;

	int wsize = length+offset;
	if (rsize > wsize) wsize = rsize; // Read is larger than write
	char rbuf[rsize];
	char content[wsize];
	memset(content, '0x00', wsize);
	char* writebuf=content;

	if (pRead.segmentNo!=0) { 
		printf("REWRITING CONTENTS OF A FILE\n");
		// The segment number IS NOT 0 so the file DOES have contents
		//if (!Log_read(pRead, rsize, rbuf)) { // Retrieve the previous contents
		if (!File_Read(inum, 0, rsize, rbuf)) {	
			printf("Rsize: %d   Wsize: %d    \n", rsize, wsize);
			printf("Current contents: %s \n", rbuf);
			memcpy(writebuf, rbuf, rsize);
			
			if (offset+length < wsize) { //overwriting within file, don't need \0
				memcpy(writebuf+offset, buffer, length-1);
			} else { //writing past the end, include a final \0
				memcpy(writebuf+offset, buffer, length);
			}
			
			printf("Buffer: %s offset %d wsize %d\n", buffer, offset, wsize);
			int bytesLeft = wsize;

			struct logAddress otherBlocks[numBlocks];
			for (int i=1; i<=numBlocks; i++) {
				printf(" Loop: i=%d\n",i);
				bytesLeft = wsize-((i-1)*MAX_SIZE); 
				if (bytesLeft <= 0) bytesLeft = wsize;
				int j = (i-1)*MAX_SIZE;
				printf("Bytes left: %d\n", bytesLeft);

				if ((i > 4) && (bytesLeft > 0)) { // We're out of direct pointers
					printf("STARTING ARRAY OF POINTERS...\n");
					memcpy(writebuf, buffer+j, bytesLeft);
					if (!Log_Write(inum, 1, bytesLeft, (void *) writebuf, &dataAdd)) {
						if ((otherBlocks[i-5].segmentNo != dataAdd.segmentNo) &&
								otherBlocks[i-5].segmentNo > 0) {
								Log_writeDeadBlock(inum, otherBlocks[i-5], dataAdd);
							}
						otherBlocks[i-5]=dataAdd;
		 				printf("\n\t---write file section/blockptr:%d, segmentNo=%d  block number=%d \n",i,dataAdd.segmentNo,dataAdd.blockNo);
					} else {
						printf("File: write error for section %d, cannot write %s\n", i, writebuf);
					}

					if (i == numBlocks) {
						//We've reached the end, check the array
						printf("---- Added %d additional blocks\n", numBlocks-4); //otherBlocks.size());
						// Convert otherBlocks vector into void*
						int sizeOfVector = sizeof(otherBlocks);
						printf("---- Size of vector (space): %d\n", sizeOfVector);
						Log_Write(inum, 1, sizeOfVector, (void *) otherBlocks, &dataAdd);
						if ((fileinode.OtherBlocksPtr.segmentNo != dataAdd.segmentNo) &&
								fileinode.OtherBlocksPtr.segmentNo > 0) {
								Log_writeDeadBlock(inum, fileinode.OtherBlocksPtr, dataAdd);
							}
						//Save the array's pointer
						fileinode.OtherBlocksPtr = dataAdd;
					}

				}
				// We have more than 1 block of data to write
				else if (bytesLeft > MAX_SIZE) {
			//		memcpy(writebuf, &buffer[j], MAX_SIZE);   //DONE--FIXME: danger use buff[j], not alway be a array, use point +offset
					memcpy(writebuf, buffer+j, MAX_SIZE);
					if (!Log_Write(inum, 1, MAX_SIZE, (void *) writebuf, &dataAdd)) {
						if (i == 1) {
							if ((fileinode.Block1Ptr.segmentNo != dataAdd.segmentNo) &&
								fileinode.Block1Ptr.segmentNo > 0) {
								Log_writeDeadBlock(inum, fileinode.Block1Ptr, dataAdd);
							}
							fileinode.Block1Ptr = dataAdd;
						}
						if (i == 2) {
							if ((fileinode.Block2Ptr.segmentNo != dataAdd.segmentNo) &&
								fileinode.Block2Ptr.segmentNo > 0) {
								Log_writeDeadBlock(inum, fileinode.Block2Ptr, dataAdd);
							}
							fileinode.Block2Ptr = dataAdd;
						}
						if (i == 3) {
							if ((fileinode.Block3Ptr.segmentNo != dataAdd.segmentNo) &&
								fileinode.Block3Ptr.segmentNo > 0) {
								Log_writeDeadBlock(inum, fileinode.Block3Ptr, dataAdd);
							}
							fileinode.Block3Ptr = dataAdd;
						}
						if (i == 4) {
							if ((fileinode.Block4Ptr.segmentNo != dataAdd.segmentNo) &&
								fileinode.Block4Ptr.segmentNo > 0) {
								Log_writeDeadBlock(inum, fileinode.Block4Ptr, dataAdd);
							}
							fileinode.Block4Ptr = dataAdd;
						}
		 				printf("***%s\n---write file section/blockptr:%d, segmentNo=%d  block number=%d \n",writebuf,i,dataAdd.segmentNo,dataAdd.blockNo);
					} else {
						printf("File: write error for section %d, cannot write %s\n", i, writebuf);
					}
				} 
				
				// We have less than 1 block left
				else if (bytesLeft > 0){
					printf("less than a block left: %d\n", bytesLeft);
				//	memcpy(writebuf, &buffer[j], bytesLeft); //DONE--FIXME: danger use buff[j], not alway be a array, use point +offset
					memcpy(writebuf, buffer+j, bytesLeft);
					if (!Log_Write(inum, 1, bytesLeft, (void *) writebuf, &dataAdd)) {
						// For each pointer,  if we can clean dead blocks
						if (i == 1) {
							if ((fileinode.Block1Ptr.segmentNo != dataAdd.segmentNo) &&
								fileinode.Block1Ptr.segmentNo > 0) {
								Log_writeDeadBlock(inum, fileinode.Block1Ptr, dataAdd);
							}
							fileinode.Block1Ptr = dataAdd;
						}
						if (i == 2) {
							if ((fileinode.Block2Ptr.segmentNo != dataAdd.segmentNo) &&
								fileinode.Block2Ptr.segmentNo > 0) {
								Log_writeDeadBlock(inum, fileinode.Block2Ptr, dataAdd);
							}
							fileinode.Block2Ptr = dataAdd;
						}
						if (i == 3) {
							if ((fileinode.Block3Ptr.segmentNo != dataAdd.segmentNo) &&
								fileinode.Block3Ptr.segmentNo > 0) {
								Log_writeDeadBlock(inum, fileinode.Block3Ptr, dataAdd);
							}
							fileinode.Block3Ptr = dataAdd;
						}
						if (i == 4) {
							if ((fileinode.Block4Ptr.segmentNo != dataAdd.segmentNo) &&
								fileinode.Block4Ptr.segmentNo > 0) {
								Log_writeDeadBlock(inum, fileinode.Block4Ptr, dataAdd);
							}
							fileinode.Block4Ptr = dataAdd;
						}

		 				printf("\n\t---write file section/blockptr:%d, segmentNo=%d  block number=%d \n",i,dataAdd.segmentNo,dataAdd.blockNo);
					} else {
						printf("File: write error for section %d, cannot write %s\n", i, writebuf);
					}	
				}
				if (dataAdd.segmentNo % SEGMENT_THRESHOLD == 0) {
					printf("********* SEGMENT_THRESHOLD REACHED *****\n");
					checkpointflag = 1;
				}	
			}
		} else {
				printf("File: read error, cannot read inum %d\n", inum);
		}
	} else {
		// The segment number is 0, the file has no contents (this is first write to file)
		printf("WRITING NEW CONTENT INTO FILE\n");

		printf("We need %d blocks for this write...\n", numBlocks);
		printf("buffer2 %s\n", buffer);
		int bytesLeft = length;

		struct logAddress otherBlocks[numBlocks];
		printf("size of otehrBlocksArray: %d, size of logAddress: %d\n", sizeof(otherBlocks), sizeof(logAddress));
		
		for (int i=1; i<=numBlocks; i++) {
			printf(" Loop: i=%d\n",i);
			bytesLeft = length-((i-1)*MAX_SIZE); 
			if (bytesLeft <= 0) bytesLeft = length;
			int j = (i-1)*MAX_SIZE;
			printf("Bytes left: %d\n", bytesLeft);

			if ((i > 4) && (bytesLeft > 0)) { // We're out of direct pointers
				printf("STARTING ARRAY OF POINTERS...\n");
				memcpy(writebuf, buffer+j, bytesLeft);
				if (!Log_Write(inum, 1, bytesLeft, (void *) writebuf, &dataAdd)) {
					otherBlocks[i-5]=dataAdd;
	 				printf("\n\t---write file section/blockptr:%d, segmentNo=%d  block number=%d \n",i,dataAdd.segmentNo,dataAdd.blockNo);
				} else {
					printf("File: write error for section %d, cannot write %s\n", i, writebuf);
				}

				if (i == numBlocks) {
					//We've reached the end, check the array
					printf("---- Added %d additional blocks\n", numBlocks-4); //otherBlocks.size());
					// Convert otherBlocks vector into void*
					int sizeOfVector = sizeof(otherBlocks);
					printf("---- Size of vector (space): %d\n", sizeOfVector);
					//struct logAddress* otherBlocksArray = otherBlocks.data();
					Log_Write(inum, 1, sizeOfVector, (void *) otherBlocks, &dataAdd);

					//Save the array's pointer
					fileinode.OtherBlocksPtr = dataAdd;
				}

			}
			// We have more than 1 block of data to write
			else if (bytesLeft > MAX_SIZE) {
		//		memcpy(writebuf, &buffer[j], BLOCK_SIZE);   //DONE--FIXME: danger use buff[j], not alway be a array, use point +offset
				memcpy(writebuf, buffer+j, MAX_SIZE);
				if (!Log_Write(inum, 1, MAX_SIZE, (void *) writebuf, &dataAdd)) {
					if (i == 1) fileinode.Block1Ptr = dataAdd;
					if (i == 2) fileinode.Block2Ptr = dataAdd;
					if (i == 3) fileinode.Block3Ptr = dataAdd;
					if (i == 4) fileinode.Block4Ptr = dataAdd;
	 				printf("***%s\n---write file section/blockptr:%d, segmentNo=%d  block number=%d \n",writebuf,i,dataAdd.segmentNo,dataAdd.blockNo);
				} else {
					printf("File: write error for section %d, cannot write %s\n", i, writebuf);
				}
			} 
			
			// We have less than 1 block left
			else if (bytesLeft > 0){
				printf("less than a block left: %d\n", bytesLeft);
			//	memcpy(writebuf, &buffer[j], bytesLeft); //DONE--FIXME: danger use buff[j], not alway be a array, use point +offset
				memcpy(writebuf, buffer+j, bytesLeft);
				if (!Log_Write(inum, 1, bytesLeft, (void *) writebuf, &dataAdd)) {
					if (i == 1) fileinode.Block1Ptr = dataAdd;
					if (i == 2) fileinode.Block2Ptr = dataAdd;
					if (i == 3) fileinode.Block3Ptr = dataAdd;
					if (i == 4) fileinode.Block4Ptr = dataAdd;
	 				printf("\n\t---write file section/blockptr:%d, segmentNo=%d  block number=%d \n",i,dataAdd.segmentNo,dataAdd.blockNo);
				} else {
					printf("File: write error for section %d, cannot write %s\n", i, writebuf);
				}
			}
			if (dataAdd.segmentNo % SEGMENT_THRESHOLD == 0) {
				printf("********* SEGMENT_THRESHOLD REACHED *****\n");
				checkpointflag = 1;
			}
		}
		fileinode.size = length;
		fileinode.numBlocks = numBlocks;
	}

	// Update the inode time
	time_t now = time(0);
	fileinode.mtime = time(&now);
	fileinode.atime = time(&now);

	// Write the inode data
	Put_Inode(inum, &fileinode); //KATY the segment fault line
	//IfileArray.data[inum] = fileinode;S


	printf("We Put_Inode for inum %d at block %d and segment %d \n", inum, fileinode.Block1Ptr.blockNo, fileinode.Block1Ptr.segmentNo);
	
	struct Inode check_inode = IfileArray.data[inum];//Get_Inode(inum);
	//struct Inode check_inode;
	//File_Get(inum, &check_inode);
	printf("Check_inode data for inum %d and block %d\n", check_inode.inum, check_inode.Block1Ptr.blockNo);
	Show_Ifile_Contents();
	if (checkpointflag) WriteIfle(); 
	return 0;
}

int File_Read(int inum, int offset, int length, void * buffer) {
	printf("FILE READ for INODE %d\n", inum);
	
	struct Inode iptr = IfileArray.data[inum];//Get_Inode(inum);
	int numBlocks = 1 + (((offset+length) - 1) / MAX_SIZE); //For now, read in everything
	printf("Read in %d blocks (length: %d)for inode %d == %d\n", numBlocks, length, iptr.inum, inum);

	
	struct logAddress ladd;
	
	//int amtToRead = length+offset;
	//if (amtToRead > BLOCK_SIZE) amtToRead = BLOCK_SIZE;
	//char content[amtToRead];
	char totalRead[length];
	int offsetHere = offset / MAX_SIZE + 1;
	printf("OFFSET %d (MAX_SIZE %d), so offsetHere %d\n", offset, MAX_SIZE,offsetHere );

	if (numBlocks >= 1) {
		int amtToRead1 = length+offset;
		printf("amtToRead1: %d, MAX_SIZE: %d\n", amtToRead1, MAX_SIZE);
		if (amtToRead1 > MAX_SIZE) amtToRead1 = MAX_SIZE;
		char content1[amtToRead1];
		printf("NUMBLOCKS ?? %d\n", numBlocks);
		ladd.blockNo = iptr.Block1Ptr.blockNo;
		ladd.segmentNo = iptr.Block1Ptr.segmentNo;
		printf("Read for section1: segmentNo=%d  block number=%d \n",ladd.segmentNo,ladd.blockNo);

		printf("amtToRead1: %d\n", amtToRead1);
		if (!Log_read(ladd, amtToRead1, content1)) {
			printf("file read %s \n", content1);
				memcpy(buffer, content1+offset, amtToRead1-offset);
		
		} else {
			printf("File_Read: error with Log_Read of file %d\n", inum);
		}
		printf("BUFFER (BLOCK1)%s\n", buffer);
	}
	if (numBlocks >= 2) {
		int amtToRead2 = length - MAX_SIZE + offset;
		if (amtToRead2 > MAX_SIZE) amtToRead2 = MAX_SIZE;
		char content2[amtToRead2];
		printf("NUMBLOCKS ?? %d\n", numBlocks);
		ladd.blockNo = iptr.Block2Ptr.blockNo;
		ladd.segmentNo = iptr.Block2Ptr.segmentNo;
		// BUG on next line: see "Read (800) for section2: segmentno12090382930"
		printf("Read (%d) for section2: segmentNo=%d  block number=%d \n",amtToRead2, ladd.segmentNo,ladd.blockNo);

		
		if (!Log_read(ladd, amtToRead2, content2)) {
			printf("file read cont2: %s   buf: %s\n", content2, buffer);
	//		memcpy(&buffer[BLOCK_SIZE-offset], content2, amtToRead2); // DONE--FIXEME: buffer is point, point+offset can point the address
		    
		    memcpy(buffer+MAX_SIZE-offset, content2, amtToRead2);
			
		} else {
			printf("File_Read: error with Log_Read of file %d\n", inum);
		}
	}
	if (numBlocks >= 3) {
		int amtToRead3 = length - 2*MAX_SIZE + offset;
		if (amtToRead3 > MAX_SIZE) amtToRead3 = MAX_SIZE;
		char content3[amtToRead3];
		ladd.blockNo = iptr.Block3Ptr.blockNo;
		ladd.segmentNo = iptr.Block3Ptr.segmentNo;
		printf("Read for section3: segmentNo=%d  block number=%d \n",ladd.segmentNo,ladd.blockNo);

		
		if (!Log_read(ladd, amtToRead3, content3)) {
			printf("file read %s\n", content3);
		//	memcpy(&buffer[BLOCK_SIZE*2], content3, amtToRead3); // DONE-- FIXME: buffer is point, point+offset can point the address
			memcpy(buffer+MAX_SIZE*2-offset, content3, amtToRead3); 
			
		} else {
			printf("File_Read: error with Log_Read of file %d\n", inum);
		}
	}
	if (numBlocks >= 4) { //KATY FIX
		int amtToRead4 = length - 3*MAX_SIZE - offset;
		if (amtToRead4 > MAX_SIZE) amtToRead4 = MAX_SIZE;
		char content4[amtToRead4];
		ladd.blockNo = iptr.Block4Ptr.blockNo;
		ladd.segmentNo = iptr.Block4Ptr.segmentNo;
		printf("Read for section4: segmentNo=%d  block number=%d \n",ladd.segmentNo,ladd.blockNo);

		
		if (!Log_read(ladd, amtToRead4, content4)) {
			printf("file read %s\n", content4);
		//	memcpy(&buffer[BLOCK_SIZE*3], content4, amtToRead4); // DONE--FIXME: buffer is point, point+offset can point the address
			memcpy(buffer+MAX_SIZE*3-offset, content4, amtToRead4); 
			
		} else {
			printf("File_Read: error with Log_Read of file %d\n", inum);
		}
	}
	if (numBlocks >= 5) {
		printf("READING FROM MORE THAN 4 BLOCKS\n");
		
		for (int i=5; i<= numBlocks; i++) {
			int amtToRead5 = length - (i-1)*MAX_SIZE;
			if (amtToRead5 > MAX_SIZE) amtToRead5 = MAX_SIZE;
			char content5[amtToRead5];

			struct logAddress extraBlockLogAdds[numBlocks];
			
			ladd.blockNo = iptr.OtherBlocksPtr.blockNo;
			ladd.segmentNo = iptr.OtherBlocksPtr.segmentNo;
			if(!Log_read(ladd, numBlocks*sizeof(logAddress), extraBlockLogAdds)) {
				printf("content5 Add: %s %d %d \n\n", extraBlockLogAdds, extraBlockLogAdds[i-5].blockNo, extraBlockLogAdds[i-5].segmentNo);
				if (!Log_read(extraBlockLogAdds[i-5], amtToRead5, content5)){
					printf("content5: %s\n", content5);
					memcpy(buffer+MAX_SIZE*(i-1)-offset, content5, amtToRead5);
				}
			}
		}
	}
	printf("BUFFER: %s\n", buffer);
	printf("It's fine...\n");

	return 0;
} // end File_Read


int File_Free(int inum) {
	printf("Start file free: inum %d\n", inum);
	struct Inode inode = IfileArray.data[inum];//Get_Inode(inum);
	inode.in_use = 0; //Inode is no longer in use

	// struct logAddress dataAdd;
	// dataAdd.blockNo = 0;
	// dataAdd.segmentNo = 0;

	// int numBlocks = 1 + ((inode.size - 1) / MAX_SIZE); 
	// for (int i=1; i<=numBlocks; i++){
	// 	if (i == 1) {
	// 		Log_writeDeadBlock(inum, inode.Block1Ptr, dataAdd);
	// 		inode.Block1Ptr = dataAdd;
	// 	}
	// 	if (i == 2) inode.Block2Ptr = dataAdd;
	// 	if (i == 3) inode.Block3Ptr = dataAdd;
	// 	if (i == 4) inode.Block4Ptr = dataAdd;
	// 	else inode.OtherBlocksPtr = dataAdd;
	// }
	IfileArray.data[inum] = inode;
	printf("End file free: inum %d\n", inum);

}


void File_Destroy(){
	//TODO
	WriteIfle();
	Log_destroy();
	return;
}





int convertInodeToStat(int inum, struct stat* s)
{
	int availInodes = IfileArray.data.size();
	if (inum > availInodes) {
		printf("Error: Inode %d does not exist (out of bounds, %d)\n", inum, availInodes);
		return 3;
	} else {
		if (IfileArray.data[inum].in_use == 0) {
			printf("Error: inode %d has been deleted\n");
			return 2;
		}
	}
	struct Inode inode = IfileArray.data[inum];
	s->st_ino = inode.inum;
	s->st_mode = inode.permissions;

	if (inode.type == TYPE_D) { 
		s->st_nlink = 2; //  directory
	} else {
		s->st_nlink = 1; // file
	}
	printf("INODE debug permissions %d -- %d \n", s->st_mode, inode.permissions);
	s->st_uid = inode.owner;
	s->st_gid = inode.group;
	//s->st_rdev = 0; //If file is character or block special
	s->st_size = inode.size;
	s->st_atime = inode.atime;
	s->st_mtime = inode.mtime;
	s->st_ctime = inode.ctime;
	s->st_blksize = BLOCK_SIZE;
	s->st_blocks = (int) inode.numBlocks;
	return 0;
}

int Change_Permissions(int inum, mode_t permissions) {

	struct Inode inode = IfileArray.data[inum];
	inode.permissions = permissions;
	IfileArray.data[inum] = inode;

	return 0;
}

int Change_Owner(int inum, uid_t owner) {

	struct Inode inode = IfileArray.data[inum];
	inode.owner = getuid();
	IfileArray.data[inum] = inode;
	return 0;
}

int Change_Group(int inum, gid_t group) {

	struct Inode inode = IfileArray.data[inum];
	inode.group = getgid();
	IfileArray.data[inum] = inode;
	return 0;
}


int initFile(int size) {
	 // Create ifile inode, add it to the ifile data structure,
    // and write the ifile data structure to disk

    init("FuseFileSystem",size);  

    // Create the Ifile 
    File_Create(IFILE_INUM, TYPE_F);
    if (IfileArray.data.size() == 1) {
		printf("Ifile initialized with size %d\n", IfileArray.data.size());
    
    } else {
    	printf("Ifile re-established with size %d\n", IfileArray.data.size());
    }
    Show_Ifile_Contents();

	return IfileArray.data.size()-1;
}



// int main(){
// 	printf("Begin cfile layer, creating ifile (and its inode)...\n");
// 	int size = 4;
//   	//initFile(size);
//   	//TestFileFree();
//   	readInIfile();
//   	//test8F(); //-- recover ifile


//   	//simple1();
//   //	simple2();
//   	//test12(); //--convert i to s
//    	//test7F(); //-- Dead segment

//    	//test5Destroy();
//    	//test6Destroy();

     
//   	 //test10();
//     //	test11();
// }

