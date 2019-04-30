#include <stdio.h>
#include <iostream>
#include <ctime>
#include "../include/cfile.h"

#include <vector>
using namespace std;
//00 is false (but means things ran normally)
// 1 is true

#define GLOBAL_INUM 0
#define IFILE_INUM 0
#define TYPE_F 0
#define TYPE_D 1
#define SIZEOF_INODE sizeof(struct Inode)
#define SEGMENT_THRESHOLD 3

struct Ifile {
    vector<struct Inode> data; //location of Inode == inum, should be vector<Inode>
};

struct IfileWrite {
 	struct Inode data[6];
    int size;
};

//struct Ifile LFSlog[1]; //representing writing the ifile's metadata
struct Inode IfileMetadata; //representing writing files to the ifile
struct Ifile IfileArray;
struct IfileWrite IfileWrite; 


struct Inode Get_Ifile() {
	return IfileMetadata;
	//return LFSlog[IFILE_INUM];
}

int File_Get(int inum, struct Inode *node) {
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
		memcpy(node,&IfileArray.data[inum],sizeof(struct Inode));
	//	return IfileArray.data[inum];
		return 0;
	}
	

	//return 1;
}


int File_Naming(int inum, const char *path,char *filename,struct stat *stbuf){

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
		return 0;
	}

}
int Put_Ifile(struct Inode *ifiledata) {
	IfileMetadata = *ifiledata;
	//LFSlog[IFILE_INUM] = *ifile;
	//printf("**PUT: New contents of ifile: \n");
	//Show_Ifile_Contents();
	return 0;
}

struct Inode initInode(int inum) {
	struct Inode inode;
	inode.inum = inum;
	return inode;
}

// int Get_Inode(int inum, struct Inode* inode) {
// 	printf("***GET: get inode %d\n", inum);
// 	struct Inode ifile = Get_Ifile();
// 	printf("ifile size: %d\n", ifile.size);
// 	if (inum <= 10) {
// 		printf("---- got this inum %d for inode %d\n", IfileArray.data[inum].inum, inum);
// 		inode = &(IfileArray.data[inum]);
// 	} else {
// 		printf("Error: cannot get inum %d from ifile (size %d)\n", inum, 10 );
// 		//return 1;
// 	}
// 	// if (inum <= 10) {
// 	// 	printf("---- got this inum %d for inode %d\n", ifile.data[inum].inum, inum);
// 	// 	return ifile.data[inum];
// 	// } else {
// 	// 	printf("Error: cannot get inum %d from ifile (size %d)\n", inum, 10 );
// 	// 	//return 1;
// 	// }
// 	return 0;
// }

int Put_Inode(int inum, struct Inode* iptr) {
	struct Inode ifile = Get_Ifile();  // Get_Ifile function do not need paras
	IfileArray.data.push_back(*iptr);
	ifile.size = IfileArray.data.size() - 1;
	Put_Ifile(&ifile);
	//printf("Putting inode %d data, block %d seg %d\n", iptr->inum, iptr->Block1Ptr.blockNo, iptr->Block1Ptr.segmentNo);
	//ifile.data[inum] = *iptr;
	return 0;
}

void Show_Ifile_Contents() {
	printf("############ start Show_Ifile_Contents ############\n");
 	//struct Ifile ifile = Get_Ifile();
 	//struct Inode ifile = Get_Ifile();
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
	// struct Ifile ifile = Get_Ifile();
	//struct Inode* in = &IfileArray.data[inum];

	struct Inode in = IfileArray.data[inum];
	//struct Inode in = Get_Inode(inum);
	printf("%d %d %c %c  %7d  %s %s\n", in.permissions, in.nlink, in.owner, in.group, in.size, in.mtime, in.filename);
}

void WriteIfileToLog() {
	printf("Begin writing ifile to log...\n");
	//// Copy over IfileArray into tempIwrite ///
	struct IfileWrite *tempIwrite = (struct IfileWrite *)malloc(sizeof(struct IfileWrite));
	int a = IfileArray.data.size();
	
	tempIwrite->size = 2;
	printf("HERE %d\n", tempIwrite->size);
	for (int i=0; i<IfileArray.data.size(); i++) {
		if (i<9) {
			printf("Add to tempIwrite %d \n", i);
			tempIwrite->data[i] = IfileArray.data[i];
			//printf("%d %d %s %c  %c   %7d  %s\n", tempIwrite.data[i].permissions, tempIwrite.data[i].nlink, tempIwrite.data[i].filename, tempIwrite.data[i].owner, tempIwrite.data[i].group, tempIwrite.data[i].size, tempIwrite.data[i].mtime);		
		}
	}
	printf("tempIwrite size %d\n", tempIwrite->size);
	///////// Write Ifile contents using File ////////
	/*int newSize = BLOCK_SIZE-200;
	void* content = &(tempIwrite);
	File_Write(IFILE_INUM, 0, newSize, (void *)content);
	//File_Write(inum, 0, 10, (void *) buffer)
	void* rcontent[newSize];

	struct IfileWrite iwrite;
	File_Read(IFILE_INUM, 0, newSize, (void *)rcontent);

	memcpy(&iwrite, rcontent, newSize);
	printf("Size of ifile: %d\n", IfileArray.data.size());
	for (int i=0; i<IfileArray.data.size(); i++) {
		printf("rcontent: %d\n", iwrite.data[i].inum);
	}*/
	
	////////// Write Ifile contents using Log ////////
	//logAddress oldAdd;
	logAddress newAdd;

	int oldSize = 1;
	int newSize = BLOCK_SIZE - 200; //sizeof(IfileWrite);
	void* content = &(tempIwrite);
	
	if (!Log_Write(IFILE_INUM, 0, newSize, &tempIwrite, &newAdd)) {
		printf("Wrote Ifile to block %d , segment %d\n", newAdd.blockNo, newAdd.segmentNo);
	}

	/////// Store Ifile location /////////
	// struct Inode ifile = Get_Ifile();
	// ifile.Block1Ptr = newAdd;
	// Put_Ifile(&ifile);
	IfileMetadata.Block1Ptr = newAdd;

	//struct Ifile IfileArray2; 
	//struct IfileWrite iwrite;
	void* rcontent[newSize];
	Log_read(newAdd, newSize, rcontent);
	printf("Log_read completed, size: %d, read in %u\n", newSize, rcontent);
	//memcpy(&IfileArray2, rcontents, newSize); //sizeof(struct Ifile));
	//memcpy(&iwrite, rcontent, newSize);
	struct IfileWrite* iwrite = (struct IfileWrite *)malloc(sizeof(struct IfileWrite));
	printf("Size of IfileWrite: %d    size of newSize%d\n", sizeof(struct IfileWrite), newSize);
	memcpy(&iwrite, rcontent, sizeof(struct IfileWrite*));
	//iwrite = (struct IfileWrite*) rcontent;
	printf("iwrite.size = %d\n", (int) iwrite->size);

	int numFiles =(int) iwrite->size;// iwrite.size; //IfileArray.data.size();
	//printf("Size of IfileArray2: %d\n", numFiles);
	for (int i=0; i<numFiles; i++) {
		
		struct Inode in = iwrite->data[i];
		printf("%d %d %s %c  %c   %7d  %s\n", in.permissions, in.nlink, in.filename, in.owner, in.group, in.size, in.mtime);
	
	}
	
	// printf("Call Log_recordIfile \n");
	// newSize = numFiles;
	// if(!Log_recordIfile( &oldAdd, &newAdd, oldSize, newSize)){
	// 	printf("Saved Ifile to block %d, segment %d\n", newAdd.blockNo, newAdd.segmentNo);
	// }
	struct logAddress oldAddress;
	Log_GetIfleAddress(&oldAddress, 1);
	struct logAddress newAddress = newAdd;
	printf("oldAddres: %d %d  newAddress: %d %d  \n", oldAddress.blockNo, oldAddress.segmentNo, newAddress.blockNo, newAddress.segmentNo);
	int oldSz = 1;
	int newSz = 1;
	Log_CheckPoint(&oldAddress, &newAddress, oldSz, newSz);
	// printf("Call Log_recordIfile \n");
	// newSize = numFiles;
	// if(!Log_recordIfile( &oldAdd, &newAdd, oldSize, newSize)){
	// 	printf("Saved Ifile to block %d, segment %d\n", newAdd.blockNo, newAdd.segmentNo);
	// }
	
	
	return;
}

int File_Create(int inum, int type) {
	int checkpointflag = 0;
	if (inum == IFILE_INUM) 
	{
		// First check if ifile already exists
		struct logAddress ifileAdd;
		if (Log_GetIfleAddress(&ifileAdd, 1) == 1){
			// None exists
			struct Inode metadata = Get_Ifile();
			metadata.Block1Ptr = ifileAdd;
			printf("##Creating new ifile at block %d, segment %d\n", ifileAdd.blockNo, ifileAdd.segmentNo);
		} else {
			// Already exists, do Log Read to get ifile
			printf("##Ifile already exists at block %d, segment %d\n", ifileAdd.blockNo, ifileAdd.segmentNo);
			


			// int newSize = BLOCK_SIZE - 200; //sizeof(IfileWrite);
			// void* rcontent[newSize];
			// Log_read(ifileAdd, newSize, rcontent);
			// printf("Log_read completed (block %d seg %d), size: %d, read in %u\n", ifileAdd.blockNo, ifileAdd.segmentNo, newSize, rcontent);
			// //memcpy(&IfileArray2, rcontents, newSize); //sizeof(struct Ifile));
			// //memcpy(&iwrite, rcontent, newSize);
			// struct IfileWrite* iwrite = (struct IfileWrite *)malloc(sizeof(struct IfileWrite));
			// printf("Size of IfileWrite: %d    size of newSize%d\n", sizeof(struct IfileWrite), newSize);
			// memcpy(&iwrite, rcontent, sizeof(struct IfileWrite*));
			// //iwrite = (struct IfileWrite*) rcontent;
			// printf("iwrite inum1 = %d\n", (int) iwrite->data[1].inum);
			// printf("iwrite.size = %d\n", (int) iwrite->size);

 		     char ifilecontents[BLOCK_SIZE-200];
			 memset(ifilecontents,0,BLOCK_SIZE-200);
			 Log_read(ifileAdd, BLOCK_SIZE-200, ifilecontents);
		   //  IfileArray.data.resize(100);
		     struct IfileWrite *tmpifle;
		     tmpifle=(struct IfileWrite *)ifilecontents;
		     //memcpy(&IfileArray, ifilecontents, BLOCK_SIZE-200);
			 IfileMetadata.size = IfileArray.data.size();
		//	 printf("Size of IfileArray: %d\n", IfileArray.size);
			// auto it=tmpifle->data.begin();
		 // //   for(int i=0;i<tmpifle->data.size();i++){
		 // 	 cout<<"it ="<<(*it).inum<<endl;
		    	cout<<"ifile "<<(tmpifle->data[0]).inum << " " << (tmpifle->data[0]).owner<<endl;
		    	cout<<"ifle "<<(tmpifle->data[1]).inum<< " " << (tmpifle->data[1]).owner <<endl;
		  //  }
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
   
	   	// convert now to string form
	   	char* timestring = ctime(&now);
	   	timestring[strlen(timestring)-1]= 0;
	   	dummyInode.mtime = timestring;
	   	dummyInode.atime = timestring;
	   	dummyInode.ctime = timestring;
		//Write the inode
		int block = 0; //start of file
		int length = sizeof(dummyInode);
		unsigned char *contents[sizeof(struct Inode)];
		memcpy(contents, &dummyInode, sizeof( struct Inode));

		struct logAddress logAdd;
		printf("dummyInode ----%d, %d, %c, %lu\n", dummyInode.inum, dummyInode.in_use, dummyInode.owner, dummyInode.mtime);
		Log_Write(inum, block, length, (void *) contents, &logAdd);

		struct Inode rinode; 
		unsigned char *rcontents[sizeof(struct Inode)];
		Log_read(logAdd, sizeof(struct Inode), rcontents);
		memcpy(&rinode, rcontents, sizeof(struct Inode));
		printf("Rinode (wrote and read inode)----%d, %d, %c, %s\n", rinode.inum, rinode.in_use, rinode.owner, rinode.atime);
		
		//Write the metadata of the ifile
		//LFSlog[IFILE_INUM] = ifile; //delete this, use above line

		Put_Inode(inum, &dummyInode);
		//IfileArray.data.push_back(dummyInode);
		//Put_Ifile(&ifile); 
		//PutTheMainIfile(&ifile);
		printf("     IFile %d created at block %d  segment %d\n", inum, logAdd.blockNo, logAdd.segmentNo);
		if (logAdd.segmentNo % SEGMENT_THRESHOLD == 0) {
				printf("********* SEGMENT_THRESHOLD REACHED *****\n");
				checkpointflag = 1;
		}
		if (checkpointflag) WriteIfileToLog();
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
		if (type == TYPE_F) inode.nlink = 1;
		if (type == TYPE_D) inode.nlink = 2;
	    // current date/time based on current system
	   	time_t now = time(0);
   
	   	// convert now to string form
	   	char* timestring = ctime(&now);
	   	timestring[strlen(timestring)-1]= 0;
	   	inode.mtime = timestring;
	   	inode.atime = timestring;
	   	inode.ctime = timestring;


		int block = 0; //start of file
		int length = sizeof(inode);

		unsigned char *contents[sizeof(struct Inode)];
		memcpy(contents, &inode, sizeof( struct Inode));
		struct logAddress logAdd;
		printf("Inode ----%d, %d, %c, %s\n", inode.inum, inode.in_use, inode.owner, inode.mtime);
		Log_Write(inum, block, length, (void *) contents, &logAdd);

		struct Inode rinode; 
		unsigned char *rcontents[sizeof(struct Inode)];
		Log_read(logAdd, sizeof(struct Inode), rcontents);
		memcpy(&rinode, rcontents, sizeof(inode));
		printf("Rinode (wrote and read inode)----%d, %d, %c, %s\n", rinode.inum, rinode.in_use, rinode.owner, rinode.atime);
	

		// Update ifile metadata
		Put_Inode(inum, &inode);
		if (logAdd.segmentNo % SEGMENT_THRESHOLD == 0) {
				printf("********* SEGMENT_THRESHOLD REACHED *****\n");
				checkpointflag = 1;
		}
		if (checkpointflag) WriteIfileToLog();

	}

	return 0;
}

int File_Write(int inum, int offset, int length, void* buffer) {

	int checkpointflag = 0;

	// Convert offset to blocks
	int block = offset/BLOCK_SIZE;
	printf("Writing at block %d within file %d...\n", block, inum);
	int numBlocks = 1 + (((offset+length) - 1) / BLOCK_SIZE);
	printf("Expected space required to write: %d blocks (for %d bytes at %d offset and block size %d) \n", numBlocks, length, offset, BLOCK_SIZE);

	struct Inode fileinode;
	fileinode = IfileArray.data[inum];//Get_Inode(inum);
	printf(" -- Inode info: inum %d at block %d  segment %d\n", fileinode.inum, fileinode.Block1Ptr.blockNo, fileinode.Block1Ptr.segmentNo);
	printf(" --      file size: %d\n", fileinode.size);
	int rsize = fileinode.size;
	struct Inode ifile = Get_Ifile();
	printf(" -- IFile info: size %d\n", ifile.size);
	
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
		if (!Log_read(pRead, rsize, rbuf)) { // Retrieve the previous contents
			
			printf("Rbuf %s\n", rbuf);
			char* rPoint = rbuf;
			printf("rPoint %s\n", rPoint);
			if (offset+length < BLOCK_SIZE) {
				printf("file debug read log \n");
				printf("Current contents: %s \n", rbuf);
				memcpy(writebuf, rbuf, rsize);
				printf("0Writebufffffff   %s\n", writebuf);
				//writebuf += offset;
				if (offset+length < wsize) { // We're overwriting within file
					//Do NOT need the \0 char for the new stuff
					memcpy(writebuf+offset, buffer, length-1);
				} else {
					//We're writing past the current end of the file so 
					// use length so that we include a final \0
					memcpy(writebuf+offset, buffer, length);
				}
				
				printf("Buffer: %s offset %d wsize %d\n", buffer, offset, length);
				printf("1Writebufffffff   %s\n", writebuf);
				//writebuf += length - 1;
				
				rPoint += offset + length - 1;
				printf("rPoint %s\n", rPoint);
				//memcpy(&writebuf[offset+length], rPoint, wsize-offset-length); //BLOCK_SIZE-offset-length);
				//printf("2Writebufffffff %s\n", writebuf);
				memcpy(content, writebuf, wsize);
				printf("content: %s, wsize: %d\n", content, wsize);
				if (!Log_Write(inum, 0, wsize, (void *) content, &dataAdd)) {
					//ifile.data[inum].Block1Ptr = dataAdd;
					fileinode.Block1Ptr = dataAdd;
					fileinode.size = wsize;
					fileinode.numBlocks = numBlocks;
					if ((pRead.blockNo != dataAdd.blockNo) && (pRead.segmentNo != dataAdd.segmentNo)) {
						printf("!!!!!!  We have a dead block at seg %d, block %d!!!!!!\n", pRead.segmentNo, pRead.blockNo);
						if (!Log_writeDeadBlock(inum, pRead, dataAdd)){
							printf("Block %d in segment %d was successfully marked as dead\n", pRead.blockNo, pRead.segmentNo);
						} else {
							printf("Error in File_Write: dead block not handled properly\n");
						}

					}
				} else {
					printf("File: write error, cannot write %s\n", writebuf);
				}

				// Calling checkpoint
				if (dataAdd.segmentNo % SEGMENT_THRESHOLD == 0) {
					printf("********* SEGMENT_THRESHOLD REACHED *****\n");
					checkpointflag = 1;

				}
				//fileinode.Block1Ptr = dataAdd;
				//fileinode.size = wsize
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

		//std::vector<struct logAddress> otherBlocks;
		struct logAddress otherBlocks[numBlocks];
		//if (numBlocks > 4) struct logAddress otherBlocks[numBlocks-4];
		printf("size of otehrBlocksArray: %d, size of logAddress: %d\n", sizeof(otherBlocks), sizeof(logAddress));
		// Write blocks
		for (int i=1; i<=numBlocks; i++) {
			printf(" Loop: i=%d\n",i);
			bytesLeft = length-((i-1)*BLOCK_SIZE); 
			if (bytesLeft <= 0) bytesLeft = length;
			int j = (i-1)*BLOCK_SIZE;
			printf("Bytes left: %d\n", bytesLeft);

			if ((i > 4) && (bytesLeft > 0)) { // We're out of direct pointers
				printf("STARTING ARRAY OF POINTERS...\n");
				memcpy(writebuf, buffer+j, bytesLeft);
				if (!Log_Write(inum, 1, bytesLeft, (void *) writebuf, &dataAdd)) {
					printf("HERE, i-5=%d\n",i-5);
					otherBlocks[i-5]=dataAdd;
					//otherBlocks.push_back(dataAdd);
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
			else if (bytesLeft > BLOCK_SIZE) {
		//		memcpy(writebuf, &buffer[j], BLOCK_SIZE);   //DONE--FIXME: danger use buff[j], not alway be a array, use point +offset
				memcpy(writebuf, buffer+j, BLOCK_SIZE);
				if (!Log_Write(inum, 1, BLOCK_SIZE, (void *) writebuf, &dataAdd)) {
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
		
		//First write so length == file size
		fileinode.size = length;
		fileinode.numBlocks = numBlocks;
	}

	// Update the inode time
	time_t now = time(0);
	char* timestring = ctime(&now);
	fileinode.mtime = timestring;
	fileinode.atime = timestring;

	// Write the inode data
	//Put_Inode(inum, &fileinode);
	IfileArray.data[inum] = fileinode;


	printf("We Put_Inode for inum %d at block %d and segment %d \n", inum, fileinode.Block1Ptr.blockNo, fileinode.Block1Ptr.segmentNo);
	
	struct Inode check_inode = IfileArray.data[inum];//Get_Inode(inum);
	printf("Check_inode data for inum %d and block %d\n", check_inode.inum, check_inode.Block1Ptr.blockNo);
	Show_Ifile_Contents();
	if (checkpointflag) WriteIfileToLog(); 
	return 0;
}

int File_Read(int inum, int offset, int length, void * buffer) {
	printf("FILE READ for INODE %d\n", inum);
	
	struct Inode iptr = IfileArray.data[inum];//Get_Inode(inum);
	int numBlocks = 1 + (((offset+length) - 1) / BLOCK_SIZE); //For now, read in everything
	printf("Read in %d blocks (length: %d)for inode %d == %d\n", numBlocks, length, iptr.inum, inum);

	
	struct logAddress ladd;
	
	//int amtToRead = length+offset;
	//if (amtToRead > BLOCK_SIZE) amtToRead = BLOCK_SIZE;
	//char content[amtToRead];
	char totalRead[length];
	int offsetHere = offset / BLOCK_SIZE + 1;
	printf("OFFSET %d (BLOCK_SIZE %d), so offsetHere %d\n", offset, BLOCK_SIZE,offsetHere );

	if (numBlocks >= 1) {
		int amtToRead1 = length+offset;
		printf("amtToRead1: %d, BLOCK_SIZE: %d\n", amtToRead1, BLOCK_SIZE);
		if (amtToRead1 > BLOCK_SIZE) amtToRead1 = BLOCK_SIZE;
		char content1[amtToRead1];
		printf("NUMBLOCKS ?? %d\n", numBlocks);
		ladd.blockNo = iptr.Block1Ptr.blockNo;
		ladd.segmentNo = iptr.Block1Ptr.segmentNo;
		printf("Read for section1: segmentNo=%d  block number=%d \n",ladd.segmentNo,ladd.blockNo);

		printf("amtToRead1: %d\n", amtToRead1);
		if (!Log_read(ladd, amtToRead1, content1)) {
			printf("file read %s \n", content1);
			//if (offsetHere == 1) { // Use the offset here
			//	printf("offset: %d    amtToRead %d -- offset\n", offset, amtToRead1);
				memcpy(buffer, content1+offset, amtToRead1-offset);
			//} else {
			//	memcpy(buffer, content1, amtToRead1);
			//}
		} else {
			printf("File_Read: error with Log_Read of file %d\n", inum);
		}
		printf("BUFFER (BLOCK1)%s\n", buffer);
	}
	if (numBlocks >= 2) {
		int amtToRead2 = length - BLOCK_SIZE + offset;
		if (amtToRead2 > BLOCK_SIZE) amtToRead2 = BLOCK_SIZE;
		char content2[amtToRead2];
		printf("NUMBLOCKS ?? %d\n", numBlocks);
		ladd.blockNo = iptr.Block2Ptr.blockNo;
		ladd.segmentNo = iptr.Block2Ptr.segmentNo;
		printf("Read (%d) for section2: segmentNo=%d  block number=%d \n",amtToRead2, ladd.segmentNo,ladd.blockNo);

		
		if (!Log_read(ladd, amtToRead2, content2)) {
			printf("file read cont2: %s   buf: %s\n", content2, buffer);
	//		memcpy(&buffer[BLOCK_SIZE-offset], content2, amtToRead2); // DONE--FIXEME: buffer is point, point+offset can point the address
		    
		    memcpy(buffer+BLOCK_SIZE-offset, content2, amtToRead2);
			
		} else {
			printf("File_Read: error with Log_Read of file %d\n", inum);
		}
	}
	if (numBlocks >= 3) {
		int amtToRead3 = length - 2*BLOCK_SIZE + offset;
		if (amtToRead3 > BLOCK_SIZE) amtToRead3 = BLOCK_SIZE;
		char content3[amtToRead3];
		ladd.blockNo = iptr.Block3Ptr.blockNo;
		ladd.segmentNo = iptr.Block3Ptr.segmentNo;
		printf("Read for section3: segmentNo=%d  block number=%d \n",ladd.segmentNo,ladd.blockNo);

		
		if (!Log_read(ladd, amtToRead3, content3)) {
			printf("file read %s\n", content3);
		//	memcpy(&buffer[BLOCK_SIZE*2], content3, amtToRead3); // DONE-- FIXME: buffer is point, point+offset can point the address
			memcpy(buffer+BLOCK_SIZE*2-offset, content3, amtToRead3); 
			
		} else {
			printf("File_Read: error with Log_Read of file %d\n", inum);
		}
	}
	if (numBlocks >= 4) { //KATY FIX
		int amtToRead4 = length - 3*BLOCK_SIZE - offset;
		if (amtToRead4 > BLOCK_SIZE) amtToRead4 = BLOCK_SIZE;
		char content4[amtToRead4];
		ladd.blockNo = iptr.Block4Ptr.blockNo;
		ladd.segmentNo = iptr.Block4Ptr.segmentNo;
		printf("Read for section4: segmentNo=%d  block number=%d \n",ladd.segmentNo,ladd.blockNo);

		
		if (!Log_read(ladd, amtToRead4, content4)) {
			printf("file read %s\n", content4);
		//	memcpy(&buffer[BLOCK_SIZE*3], content4, amtToRead4); // DONE--FIXME: buffer is point, point+offset can point the address
			memcpy(buffer+BLOCK_SIZE*3-offset, content4, amtToRead4); 
			
		} else {
			printf("File_Read: error with Log_Read of file %d\n", inum);
		}
	}
	if (numBlocks >= 5) {
		printf("READING FROM MORE THAN 4 BLOCKS\n");
		
		for (int i=5; i<= numBlocks; i++) {
			int amtToRead5 = length - (i-1)*BLOCK_SIZE;
			if (amtToRead5 > BLOCK_SIZE) amtToRead5 = BLOCK_SIZE;
			char content5[amtToRead5];

			struct logAddress extraBlockLogAdds[numBlocks];
			
			ladd.blockNo = iptr.OtherBlocksPtr.blockNo;
			ladd.segmentNo = iptr.OtherBlocksPtr.segmentNo;
			if(!Log_read(ladd, numBlocks*sizeof(logAddress), extraBlockLogAdds)) {
				printf("content5 Add: %s %d %d \n\n", extraBlockLogAdds, extraBlockLogAdds[i-5].blockNo, extraBlockLogAdds[i-5].segmentNo);
				if (!Log_read(extraBlockLogAdds[i-5], amtToRead5, content5)){
					printf("content5: %s\n", content5);
					memcpy(buffer+BLOCK_SIZE*(i-1)-offset, content5, amtToRead5);
				}
				

			}


		}
	}
	//memcpy(buffer, &totalRead[offset], length-offset);

//	buffer[length-1] = '\0';  //DONE-- FIXME: buffer is not a char point
	printf("BUFFER: %s\n", buffer);
	printf("It's fine...\n");

	return 0;
} // end File_Read

int File_Free(int inum) {
	struct Inode inode = IfileArray.data[inum];//Get_Inode(inum);
	inode.in_use = 0; //Inode is no longer in use

	printf("!!!!!!  We have a dead block at seg %d, block %d!!!!!!\n", inode.Block1Ptr.segmentNo, inode.Block1Ptr.blockNo);
	struct logAddress dataAdd;
	dataAdd.blockNo = 0;
	dataAdd.segmentNo = 0;
	if (Log_writeDeadBlock(inum, inode.Block1Ptr, dataAdd)){ //Katy CHANGE THIS WHEN FIXED TO !Log_write...
		printf("Block %d in segment %d was successfully marked as dead\n", inode.Block1Ptr.blockNo, inode.Block1Ptr.segmentNo);
	} else {
		printf("Error in File_Write: dead block not handled properly\n");
	}
	int numBlocks = 1 + ((inode.size - 1) / BLOCK_SIZE); 
	for (int i=1; i<=numBlocks; i++){
		if (i == 1) inode.Block1Ptr = dataAdd;
		if (i == 2) inode.Block2Ptr = dataAdd;
		if (i == 3) inode.Block3Ptr = dataAdd;
		if (i == 4) inode.Block4Ptr = dataAdd;
		else inode.OtherBlocksPtr = dataAdd;
	}
	IfileArray.data[inum] = inode;



}



void File_Destroy(){
	//TODO
	//WriteIfileToLog();
	//Make a checkpoint
	//Log_Destroy();
	return;
}

int Change_Permissions(int inum, int permissions) {
	// struct Inode fileinode = Get_Inode(inum);

	// // Update the inode data
	// fileinode.permissions = permissions;

	// // Write the inode data
	// struct Ifile write_ifile = Get_Ifile();
	// write_ifile.data[inum] = fileinode;
	// Put_Ifile(&write_ifile);
	struct Inode inode = IfileArray.data[inum];
	inode.permissions = permissions;
	IfileArray.data[inum] = inode;
	return 0;
}

int Change_Owner(int inum, char owner) {
	// struct Inode fileinode = Get_Inode(inum);
	
	// // Update the inode data
	// fileinode.owner = owner;

	// // Write the inode data
	// struct Ifile write_ifile = Get_Ifile();
	// write_ifile.data[inum] = fileinode;
	// Put_Ifile(&write_ifile);
	struct Inode inode = IfileArray.data[inum];
	inode.owner = owner;
	IfileArray.data[inum] = inode;
	return 0;
}

int Change_Group(int inum, char group, int groupLength) {
	// printf("Group: %s \n", group);
	// struct Inode fileinode = Get_Inode(inum);
	
	// // Update the inode data
	// fileinode.group = group;

	// // Write the inode data
	// struct Ifile write_ifile = Get_Ifile();
	// write_ifile.data[inum] = fileinode;
	// Put_Ifile(&write_ifile);
	struct Inode inode = IfileArray.data[inum];
	inode.group = group;
	IfileArray.data[inum] = inode;
	return 0;
}


int Test_File_Create(int inum) {
	printf("############ begin Test_File_Create %d ############\n",inum);
	int type = TYPE_D;

	if (File_Create(inum, type) == 0) {
		printf("SUCCESS -- File Create worked\n");
	} else {
		printf("FAILED -- File Create of inum %d\n", inum);
	}

	//struct Ifile* ifile = &(LFSlog[0]); 
	// struct Ifile ifile = Get_Ifile();
	// printf("Ifile contents (%d): \n", ifile.size);
 //    for (int i=0; i<ifile.size; i++) {
 //        //struct Inode thisInode = ifile->data[i];
 //        struct Inode thisInode = ifile.data[i];
 //        printf("inum: %d has data at block %d and seg %d \n", thisInode.inum, thisInode.Block1Ptr.blockNo, thisInode.Block1Ptr.segmentNo);
 //    }
    printf("############ end Test_File_Create %d ############\n",inum);
	return 0;
}

int Test_File_Write(int inum) {
	char *buffer = "aaaaaaaaaa";
	if (File_Write(inum, 0, 10, (void *) buffer)){
		printf("Error in Test_File_Write\n");
	}
}

int initFile(int size) {
	 // Create ifile inode, add it to the ifile data structure,
    // and write the ifile data structure to disk

    init("FuseFileSystem",size);  

    // Create the Ifile 
    File_Create(IFILE_INUM, TYPE_F);
	return 2;
}



void test1F(){
	printf("*******************File layer test 1F simple small write and read ******************************\n");
	char *buf ="Hello LFS, welcome to CSC 545 OS class"; //38
	inum num=1;
	int offset = 0;
	int length = 39;
	if(!File_Write(num, offset, length, (void*)buf)){
		//Show_Ifile_Contents();
	    char bufR[length];
	    if(!File_Read(num, offset, length, (void*)bufR)){
	    	if(strcmp(buf,bufR)!=0){
				printf("Fail:  write string %s does not match read string %s \n",buf,bufR);
			}else{
				printf("**************Success    test 1F pass*******************************\n");
			}
	    }
	}
}

void test2F(){
	printf("*******************File layer test 2F second simple small write and read ******************************\n");
	Test_File_Create(2);
	char* buf="Hello"; //30 chars Hello writing the second thing
	inum num=2;
	int offset = 0;
	int length = 6;
	if(!File_Write(num, offset, length, (void *) buf)){
		printf("WROTE %s with length %d\n", buf, strlen(buf));
	    char bufR[length];
	    if(!File_Read(num, 0, length, (void *) bufR)){
	    	//printf("Read done %s \n", (char*)buf);
	    	if(strcmp(buf,bufR)!=0){
				printf("Fail:  write string %s does not match read string %s \n",buf,bufR);
			}else{
				printf("**************Success    test 2F pass*******************************\n");
			}
	    }
	}
}

void test3F(){
	printf("*******************File layer test 3F simple big write and read ******************************\n");
	Test_File_Create(3);
	int size=BLOCK_SIZE*5 -1;   
	char buf[size];
	for(int i=0;i<(size);i++){
		buf[i]='a'+i%26;
	}
	buf[size-1] = '\0';
	int num=3;

	printf("\t About to write: %s  =======\n", buf);
	if(!File_Write(num, 0, size, buf)){
	    char bufR[size];
	    if(!File_Read(num, 0, size, bufR)){
	    	if(strcmp(buf,bufR)!=0){
				printf("Fail:  write string %s does not match read string \n", bufR);
			}else{
				printf("**************Success    test 3F pass*******************************\n");
			}
	    }
	}
}

void test4F(){
	printf("*******************File layer test 4F overwrite  ******************************\n");
	char *buf= "aaaaaaaaa";//"First write, welcome to CSC 545 OS class"; //40
	inum num=4;
	int offset=1;
	Test_File_Create(num);
	if(!File_Write(num, 0, 10, (void*)buf)){
		char *bufchange= "bb"; //"Second write LFS, rewrite string........"; //40
		File_Write(num, 0, 3, (void*)bufchange);
	    char bufR[10];
	    if(!File_Read(num, 0, 10, (void*)bufR)){
	    	char *bufresult = "bbaaaaaaa";
	    	if(strcmp(bufresult,bufR)!=0){
				printf("Fail test 6:  read string %s does not match expected read string %s\n",bufR, bufresult);
			}else{
				printf("**************Success    test 4F  pass*******************************\n");
			}
	    }
	}

}

void test5F(){
	printf("*******************File layer test 5F concat  ******************************\n");
	char *buf= "hello";
	inum num=5;
	int offset=1;
	Test_File_Create(num);
	if(!File_Write(num, 0, 6, (void*)buf)){
		char *bufchange= " world"; 
		offset = 5;
		File_Write(num, offset, 7, (void*)bufchange);
	    char bufR[12];
	    if(!File_Read(num, 0, 12, (void*)bufR)){
	    	char *bufresult = "hello world";
	    	if(strcmp(bufresult,bufR)!=0){
				printf("Fail test 5F:  read string %s does not match expected read string %s\n", bufR, bufresult);
			}else{
				printf("**************Success    test 5F  pass*******************************\n");
			}
	    }
	}

}

void test6F(){
	printf("*******************File layer test 6F offset read  ******************************\n");
	char *buf="hello world, this is a test of a very long offset read that covers two blocks."; //78
	int length = 79;
	inum num=6;
	Test_File_Create(num);
	int nooffset = 0;
	if(!File_Write(num, nooffset, length, (void*)buf)){
		int offset = 13;
	    char bufR[length-offset];
	    char *expect="world, this is a test of a very long offset read that covers two blocks.";
	    if(!File_Read(num, offset, length-offset, (void*)bufR)){
	    	if(strcmp(expect,bufR)!=0){
				printf("Fail test 6:  read string %s does not match expect string %s \n",bufR, expect);
			}else{
				printf("**************Success    test 6F  pass*******************************\n");
			}
	    }
	}
}

void test7F() {
	printf("*******************File layer test 7F dead block  ******************************\n");
	char *buf= "aaaaaaaa";//"First write, welcome to CSC 545 OS class"; //40
	int num=7;
	int offset=0;
	Test_File_Create(num);
	Show_Ifile_Contents();
	if(!File_Write(num, offset, 10, (void*)buf)){
		printf("File's current block %d, segment %d\n", IfileArray.data[num].Block1Ptr.blockNo, IfileArray.data[num].Block1Ptr.segmentNo);
		File_Free(num);
		printf("File's  freed  block %d, segment %d\n", IfileArray.data[num].Block1Ptr.blockNo, IfileArray.data[num].Block1Ptr.segmentNo);
		Show_Ifile_Contents();
		Test_File_Create(num+1);
		Test_File_Write(num+1);
		Show_Ifile_Contents();
	}
}

void test8F() {
	printf("**************File layer test 8F checkpoint ************\n");
	File_Create(1, TYPE_F);
	char* buffer = "test checkpoint";
	File_Write(1, 0, 16, (void*)buffer);
	Change_Permissions(1, 771);
	char* directory = "dir/";
	char* filename = "fname";
	struct stat* stbuf;
	File_Naming(1, directory, filename, stbuf);

	Print_Inode(1);
	WriteIfileToLog();

//	Log_destroy();

	return;
}

void test9F(){
	printf("***********Test 9F: File_Naming ************\n");
	File_Create(1, TYPE_F);
	Show_Ifile_Contents();
	char* directory = "dir/";
	char* filename = "fname";
	struct stat* stbuf;
	File_Naming(1, directory, filename, stbuf);
	Show_Ifile_Contents();
	if (!strcmp(IfileArray.data[1].filename, filename)) {
		printf("Success: filename is now %s\n", IfileArray.data[1].filename );
	} else {
		printf("Error: File_Naming didn't change the filename.\n");
	}

	return;
}

void test10F() {
	printf("***********Test 10F: File_Get ************\n");
	Inode in;
	File_Get(1, &in);
	printf("%d %d %c %c  %7d  %s %s\n", in.permissions, in.nlink, in.owner, in.group, in.size, in.mtime, in.filename);
	Print_Inode(1);
	printf("^^^^ If match, then File_Get works properly\n");
	return;
}

void TestPermissions() {
	int inum = 3;
	int permissions = 771;
	Change_Permissions(inum, permissions);
	printf("per own grp sz  time \n");
	Print_Inode(inum);

	struct Inode inode = IfileArray.data[inum];//Get_Inode(inum);
	if (inode.permissions != permissions) {
		printf("Fail: old permissions were not changed (old: %d, new: %d).\n ", inode.permissions, permissions);
	} else {
		printf("************* Success   TestPermissions pass (%d == %d) **********\n", inode.permissions, permissions);
	}
	return;
}

void TestOwner() {
	int inum = 3;
	char owner = 'k';
	Change_Owner(inum, owner);
	printf("per own grp sz  time \n");
	Print_Inode(inum);

	struct Inode inode = IfileArray.data[inum];//Get_Inode(inum);
	if (inode.owner != owner) {
		printf("Fail: old owner was not changed (old: %c, new: %c).\n ", inode.owner, owner);
	} else {
		printf("************* Success   TestOwner pass (%c == %c) **********\n", inode.owner, owner);
	}
	return;
}

void TestGroup() {
	int inum = 3;
	char group = 'c';
	int groupLength = 1;
	Change_Group(inum, group, groupLength);
	printf("per own grp sz  time \n");
	Print_Inode(inum);

	struct Inode inode = IfileArray.data[inum]; //Get_Inode(inum);
	if (inode.group != group) {
		printf("Fail: old group was not changed (old: %c, new: %c).\n ", inode.group, group);
	} else {
		printf("************* Success   TestOwner pass (%c == %c) **********\n", inode.group, group);
	}
	return;
}

// read and write Ifile test
// 
//struct Ifile IfileArray;
// struct IfileWrite IfileWrite; 
//
void test10(){
   initFile(size);
    struct IfileWrite writeTest;
    test8F();
    int i;
    for( i=0;i<IfileArray.data.size()&&i<6;i++){
    	writeTest.data[i]=IfileArray.data[i];

    }
   writeTest.size=i;
   logAddress newAdd;
   Log_Write(IFILE_INUM, 0, BLOCK_SIZE-100, &writeTest, &newAdd);
   Log_CheckPoint(&newAdd,&newAdd,1,1);
   char ifilecontents[BLOCK_SIZE-100];
   memset(ifilecontents,0,BLOCK_SIZE-100);
   Log_read(newAdd, BLOCK_SIZE-100, ifilecontents);
   cout<<" ifle  segmentNo="<<newAdd.segmentNo<<"  blockNo="<<newAdd.blockNo<<endl;

   struct IfileWrite *tmp=(struct IfileWrite *)ifilecontents;
   for(int i=0;i<tmp->size;i++){
   	  cout<<"read inode "<<i<<"  group "<<tmp->data[i].group<<" inum="<<tmp->data[i].inum<<endl;
   }
   	Log_destroy();
}

void test11(){

	init("FuseFileSystem",4);  
	logAddress newAdd;
	newAdd.segmentNo=1;
	newAdd.blockNo=5;
   char ifilecontents[BLOCK_SIZE-100];
   memset(ifilecontents,0,BLOCK_SIZE-100);
   Log_read(newAdd, BLOCK_SIZE-100, ifilecontents);
   cout<<" ifle  segmentNo="<<newAdd.segmentNo<<"  blockNo="<<newAdd.blockNo<<endl;

   struct IfileWrite *tmp=(struct IfileWrite *)ifilecontents;
   for(int i=0;i<tmp->size;i++){
   	  cout<<"read inode "<<i<<"  group "<<tmp->data[i].group<<" inum="<<tmp->data[i].inum<<endl;
   }
}

int main(){
	printf("Begin cfile layer, creating ifile (and its inode)...\n");
	int size = 4;
  // 	initFile(size);
   	// test9F();
   	// test10F();

    // Print_Inode(1);
    //test1F(); 
   // test8F();
  // 	test10();
    	test11();
}

