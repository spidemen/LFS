#include <stdio.h>
#include "../include/cfile.h"

#include <vector>
//00 is false (but means things ran normally)
// 1 is true

#define GLOBAL_INUM 0
#define IFILE_INUM 0
#define TYPE_F 0
#define TYPE_D 0
#define SIZEOF_INODE sizeof(struct Inode)

struct Ifile LFSlog[1]; //representing writing the ifile's metadata
//struct Ifile ifile; //representing writing files to the ifile

struct Ifile Get_Ifile() {
	return LFSlog[IFILE_INUM];
}

int Put_Ifile(struct Ifile *ifile) {
	LFSlog[IFILE_INUM] = *ifile;
	//printf("**PUT: New contents of ifile: \n");
	//Show_Ifile_Contents();
	return 0;
}

void Show_Ifile_Contents() {
	printf("############ start Show_Ifile_Contents ############\n");
 	struct Ifile ifile = Get_Ifile();
	printf("Ifile contents (%d): \n", ifile.size);
    for (int i=0; i<ifile.size; i++) {
        //struct Inode thisInode = ifile->data[i];
        struct Inode thisInode = ifile.data[i];
        printf("inum: %d (size: %d) has data1 at block %d and seg %d \n", thisInode.inum, thisInode.size, thisInode.Block1Ptr.blockNo, thisInode.Block1Ptr.segmentNo);
     	if (thisInode.Block2Ptr.blockNo != 22) { 
     		printf(  "            data2 at block %d and seg %d \n", thisInode.Block2Ptr.blockNo, thisInode.Block2Ptr.segmentNo);
     	}
     	if (thisInode.Block3Ptr.blockNo != 22) { 
     		printf(  "            data3 at block %d and seg %d \n", thisInode.Block3Ptr.blockNo, thisInode.Block3Ptr.segmentNo);
     	}
     	if (thisInode.Block4Ptr.blockNo != 22) { 
     		printf(  "            data4 at block %d and seg %d \n", thisInode.Block4Ptr.blockNo, thisInode.Block4Ptr.segmentNo);
     	}
     	if (thisInode.OtherBlocksPtr.blockNo != 22) { 
     		printf(  "   and more data at block %d and seg %d \n", thisInode.OtherBlocksPtr.blockNo, thisInode.OtherBlocksPtr.segmentNo);
     	}
        
    }
    printf("############ end Show_Ifile_Contents ############\n");
	return;
}

struct Inode Get_Inode(int inum) {
	printf("***GET: get inode %d\n", inum);
	struct Ifile ifile = Get_Ifile();
	printf("ifile size: %d\n", ifile.size);
	if (inum <= 10) {
		printf("---- got this inum %d for inode %d\n", ifile.data[inum].inum, inum);
		return ifile.data[inum];
	} else {
		printf("Error: cannot get inum %d from ifile (size %d)\n", inum, 10 );
		//return 1;
	}
	//return 0;
}

int Put_Inode(int inum, struct Inode *iptr) {
	struct Ifile ifile;
//	Get_Ifile(&ifile);  // Get_Ifile function do not need paras
	printf("Putting inode %d data, block %d seg %d\n", iptr->inum, iptr->Block1Ptr.blockNo, iptr->Block1Ptr.segmentNo);
	ifile.data[inum] = *iptr;
	return 0;
}

void Print_Inode(int inum) {
	struct Ifile ifile = Get_Ifile();
	struct Inode in = ifile.data[inum];
	printf("%d  %c  %s   %d  %lu\n", in.permissions, in.owner, in.group, in.size, in.time_of_last_change);
}

int File_Create(int inum, int type) {
	if (inum == IFILE_INUM) 
	{
		
		struct Inode dummyInode ;    // FIXEME: C++ can assign default vaule when define a data structure , take a look at log.cpp 
		//  = {
		// 	.inum = IFILE_INUM,
		// 	.type = type,
		// 	.in_use = 1,					// true 1, false 0
  //   		.time_of_last_change = (unsigned long)time(NULL),
  //   		.owner = 'u',
  //   		.permissions = 777,
  //   		.group = 'group',
  //   		.Block1Ptr = {
  //   			.blockNo = 22,
  //   			.segmentNo = 22
  //   		},
  //   		.Block2Ptr = {
  //   			.blockNo = 22,
  //   			.segmentNo = 22
  //   		},
  //   		.Block3Ptr = {
  //   			.blockNo = 22,
  //   			.segmentNo = 22
  //   		},
  //   		.Block4Ptr = {
  //   			.blockNo = 22,
  //   			.segmentNo = 22
  //   		},
  //   		.OtherBlocksPtr = {
  //   			.blockNo = 22,
  //   			.segmentNo = 22
  //   		}
		// };

		struct Ifile ifile ;  // FIXEME: C++ can assign default vaule when define a data structure , take a look at log.cpp 
		// = {
		// 	.inum = inum,
		// 	.size = 1, 	//number of files currently in the ifile
		// 	.addresses[0] = { 
		// 		.blockNo = 22, 
		// 		.segmentNo = 22},
		// 	.data[0] = dummyInode
		// };

		//Write the inode
		int block = 0; //start of file
		int length = sizeof(dummyInode);
		unsigned char *contents[sizeof(struct Inode)];
		memcpy(contents, &dummyInode, sizeof( struct Inode));

		struct logAddress logAdd;
		printf("dummyInode ----%d, %d, %c, %lu\n", dummyInode.inum, dummyInode.in_use, dummyInode.owner, dummyInode.time_of_last_change);
		Log_Write(inum, block, length, (void *) contents, &logAdd);

		struct Inode rinode; 
		unsigned char *rcontents[sizeof(struct Inode)];
		Log_read(logAdd, sizeof(struct Inode), rcontents);
		memcpy(&rinode, rcontents, sizeof(struct Inode));
		printf("Rinode (wrote and read inode)----%d, %d, %c, %lu\n", rinode.inum, rinode.in_use, rinode.owner, rinode.time_of_last_change);
		
		//Write the metadata of the ifile
		//LFSlog[IFILE_INUM] = ifile; //delete this, use above line
		
		Put_Ifile(&ifile); 
		
		printf("     IFile %d created at block %d  segment %d\n", inum, logAdd.blockNo, logAdd.segmentNo);
		
	}
	else 
	{
		struct Inode inode ;   // FIXEME: C++ can assign default vaule when define a data structure , take a look at log.cpp 
		// = {
		// 	.inum = inum,
		// 	.type = type,
		// 	.in_use = 1,					// true 1, false 0
  //   		.time_of_last_change = (unsigned long)time(NULL),
  //   		.owner = 'u',
  //   		.permissions = 777,
  //   		.group = 'group',
  //   		.Block1Ptr = {
  //   			.blockNo = 22,
  //   			.segmentNo = 22
  //   		},
  //   		.Block2Ptr = {
  //   			.blockNo = 22,
  //   			.segmentNo = 22
  //   		},
  //   		.Block3Ptr = {
  //   			.blockNo = 22,
  //   			.segmentNo = 22
  //   		},
  //   		.Block4Ptr = {
  //   			.blockNo = 22,
  //   			.segmentNo = 22
  //   		},
  //   		.OtherBlocksPtr = {
  //   			.blockNo = 22,
  //   			.segmentNo = 22
  //   		}
		// };

		int block = 0; //start of file
		int length = sizeof(inode);

		unsigned char *contents[sizeof(struct Inode)];
		memcpy(contents, &inode, sizeof( struct Inode));
		struct logAddress logAdd;
		printf("Inode ----%d, %d, %c, %lu\n", inode.inum, inode.in_use, inode.owner, inode.time_of_last_change);
		Log_Write(inum, block, length, (void *) contents, &logAdd);

		struct Inode rinode; 
		unsigned char *rcontents[sizeof(struct Inode)];
		Log_read(logAdd, sizeof(struct Inode), rcontents);
		memcpy(&rinode, rcontents, sizeof(inode));
		printf("Rinode (wrote and read inode)----%d, %d, %c, %lu\n", rinode.inum, rinode.in_use, rinode.owner, rinode.time_of_last_change);
		


		// printf("     File %d created at block %d  segment %d\n", inum, logAdd.blockNo, logAdd.segmentNo);
		// printf("Ifile contents (%d): \n", ifile.size);
  //   	for (int i=0; i<=ifile.size; i++) {
  //       //struct Inode thisInode = ifile->data[i];
  //       struct Inode thisInode = ifile.data[i];
  //       printf("inum: %d has data at block %d and seg %d \n", thisInode.inum, thisInode.Block1Ptr.blockNo, thisInode.Block1Ptr.segmentNo);
  //   	}

		// Update ifile metadata
		struct Ifile ifile = Get_Ifile();
		//int i = ifile.size;
		ifile.data[inum] = inode;
		ifile.size++;
		Put_Ifile(&ifile);

	}

	return 0;
}

int File_Write(int inum, int offset, int length, void* buffer) {

	int flag;

	// Convert offset to blocks
	int block = offset/BLOCK_SIZE;
	printf("Writing at block %d within file %d...\n", block, inum);
	int numBlocks = 1 + (((offset+length) - 1) / BLOCK_SIZE);
	printf("Expected space required to write: %d blocks (for %d bytes at %d offset and block size %d) \n", numBlocks, length, offset, BLOCK_SIZE);

	struct Inode fileinode;
	fileinode = Get_Inode(inum);
	printf(" -- Inode info: inum %d at block %d  segment %d\n", fileinode.inum, fileinode.Block1Ptr.blockNo, fileinode.Block1Ptr.segmentNo);
	printf(" --      file size: %d\n", fileinode.size);
	int rsize = fileinode.size;
	struct Ifile ifile = Get_Ifile();
	printf(" -- IFile info: size %d\n", ifile.size);
	
	struct logAddress pRead = ifile.data[inum].Block1Ptr;
	printf("logAddress to read from: block %d   segment %d\n", pRead.blockNo, pRead.segmentNo);
	
	
	struct logAddress dataAdd;

	int wsize = length+offset;
	if (rsize > wsize) wsize = rsize; // Read is larger than write
	char rbuf[rsize];
	char content[wsize];
	memset(content, '0x00', wsize);
	char* writebuf=content;
	// if (fileinode.size > 0) {
	// 	printf("Contents exist in inode %d (size: %d)\n", fileinode.inum, fileinode.size);
	// 	for (int i=1; i<=numBlocks; i++) {
	// 		printf("Rewrite loop: i=%d\n",i);
	// 	}
	// }


	if (pRead.segmentNo!=22) { 
		printf("REWRITING CONTENTS OF A FILE\n");
		// The segment number IS NOT 0 so the file DOES have contents
		if (!Log_read(pRead, rsize, rbuf)) { // Retrieve the previous contents
			// Case 1: we only need to read one block
			printf("Rbuf %s\n", rbuf);
			char* rPoint = rbuf;
			printf("rPoint %s\n", rPoint);
			if (offset+length < BLOCK_SIZE) {
				printf("file debug read log \n");
				printf("Current contents: %s \n", rbuf);
				memcpy(writebuf, rbuf, rsize);
				printf("0Writebufffffff   %s\n", writebuf);
				//writebuf += offset;
				memcpy(&writebuf[offset], buffer, length-1);
				printf("Buffer: %s offset %d wsize %d\n", buffer, offset, length);
				printf("1Writebufffffff   %s\n", writebuf);
				//writebuf += length - 1;
				
				rPoint += offset + length - 1;
				printf("rPoint %s\n", rPoint);
				//memcpy(&writebuf[offset+length], rPoint, wsize-offset-length); //BLOCK_SIZE-offset-length);
				//printf("2Writebufffffff %s\n", writebuf);
				memcpy(content, writebuf, wsize);
				printf("content: %s\n", content);
				if (!Log_Write(inum, 0, wsize, (void *) content, &dataAdd)) {
					ifile.data[inum].Block1Ptr = dataAdd;
				} else {
					printf("File: write error, cannot write %s\n", writebuf);
				}
				fileinode.Block1Ptr = dataAdd;
				fileinode.size = wsize;

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
		// Write blocks
		for (int i=1; i<=numBlocks; i++) {
			printf(" Loop: i=%d\n",i);
			bytesLeft = length-((i-1)*BLOCK_SIZE); 
			if (bytesLeft <= 0) bytesLeft = length;
			int j = (i-1)*BLOCK_SIZE;
			printf("Bytes left: %d\n", bytesLeft);
			// We have more than 1 block of data to write
			if (bytesLeft > BLOCK_SIZE) {
		//		memcpy(writebuf, &buffer[j], BLOCK_SIZE);   //FIXME: danger use buff[j], not alway be a array, use point +offset
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
			// We have bytesLeft and we're out of dir ptrs
			else if ((i > 4) && (bytesLeft > 0)) {
				printf("NEED MORE PTRS????????");
			}
			// We have less than 1 block left
			else if (bytesLeft > 0){
				printf("less than a block left: %d\n", bytesLeft);
			//	memcpy(writebuf, &buffer[j], bytesLeft); //FIXME: danger use buff[j], not alway be a array, use point +offset
				memcpy(writebuf, buffer+j, bytesLeft);
				if (!Log_Write(inum, 1, bytesLeft, (void *) writebuf, &dataAdd)) {
					if (i == 1) fileinode.Block1Ptr = dataAdd;
					if (i == 2) fileinode.Block2Ptr = dataAdd;
					if (i == 3) fileinode.Block3Ptr = dataAdd;
					if (i == 4) fileinode.Block4Ptr = dataAdd;
	 				printf("***\n---write file section/blockptr:%d, segmentNo=%d  block number=%d \n",i,dataAdd.segmentNo,dataAdd.blockNo);
				} else {
					printf("File: write error for section %d, cannot write %s\n", i, writebuf);
				}
				
			}

			//Clear content
			//memset(content, '0x00', BLOCK_SIZE);
			//writebuf=content;
			
		}
		//First write so length == file size
		fileinode.size = length;
	}

	// Update the inode time
	fileinode.time_of_last_change = (unsigned long)time(NULL);

	// Write the inode data
	struct Ifile write_ifile = Get_Ifile();
	write_ifile.data[inum] = fileinode;
	printf("New inode data for inum %d and block %d\n", write_ifile.data[inum].inum, write_ifile.data[inum].Block1Ptr.blockNo);
	Put_Ifile(&write_ifile);
	printf("We Put_Inode for inum %d \n", inum);
	struct Ifile check_ifile = Get_Ifile();
	printf("Checkifile data for inum %d and block %d\n", check_ifile.data[inum].inum, check_ifile.data[inum].Block1Ptr.blockNo);

	return 0;
}
int File_Read(int inum, int offset, int length, void * buffer) {
//	printf("offset %d, length %d, buffer size %d \n", offset, length, sizeof(*buffer)); //FIXEME: buff just a void point, sizeof(buffer) if char then 1


	struct Inode iptr = Get_Inode(inum);
	int numBlocks = 1 + (((offset+length) - 1) / BLOCK_SIZE);
	printf("Read in %d blocks (length: %d)for inode %d == %d\n", numBlocks, length, iptr.inum, inum);

	
	struct logAddress ladd;
	
	int amtToRead = offset+length;
	if (amtToRead > BLOCK_SIZE) amtToRead = BLOCK_SIZE;
	char content[amtToRead];
	char totalRead[length];
	int offsetHere = offset / BLOCK_SIZE + 1;
	printf("OFFSET %d (BLOCK_SIZE %d), so offsetHere %d\n", offset, BLOCK_SIZE,offsetHere );

	if (numBlocks >= 1) {
		int amtToRead = length;
		if (amtToRead > BLOCK_SIZE) amtToRead = BLOCK_SIZE;
		char content[amtToRead];
		printf("NUMBLOCKS ?? %d\n", numBlocks);
		ladd.blockNo = iptr.Block1Ptr.blockNo;
		ladd.segmentNo = iptr.Block1Ptr.segmentNo;
		printf("Read for section1: segmentNo=%d  block number=%d \n",ladd.segmentNo,ladd.blockNo);

		
		if (!Log_read(ladd, amtToRead, content)) {
			printf("file read %s \n", content);
			if (offsetHere == 1) { // Use the offset here
				memcpy(buffer, &content[offset], amtToRead-offset);
			} else {
				memcpy(buffer, content, amtToRead);
			}
		} else {
			printf("File_Read: error with Log_Read of file %d\n", inum);
		}
	}
	if (numBlocks >= 2) {
		int amtToRead2 = length - BLOCK_SIZE;
		if (amtToRead2 > BLOCK_SIZE) amtToRead2 = BLOCK_SIZE;
		char content2[amtToRead2];
		printf("NUMBLOCKS ?? %d\n", numBlocks);
		ladd.blockNo = iptr.Block2Ptr.blockNo;
		ladd.segmentNo = iptr.Block2Ptr.segmentNo;
		printf("Read for section2: segmentNo=%d  block number=%d \n",ladd.segmentNo,ladd.blockNo);

		
		if (!Log_read(ladd, amtToRead2, content2)) {
			printf("file read cont2: %s   buf: %s\n", content2, buffer);
	//		memcpy(&buffer[BLOCK_SIZE-offset], content2, amtToRead2); // FIXEME: buffer is point, point+offset can point the address
		    memcpy(buffer+BLOCK_SIZE-offset, content2, amtToRead2);
			printf("(%s %s %d %d)\n", buffer, content2, amtToRead2, BLOCK_SIZE);
			
		} else {
			printf("File_Read: error with Log_Read of file %d\n", inum);
		}
	}
	if (numBlocks >= 3) {
		int amtToRead3 = length - 2*BLOCK_SIZE;
		if (amtToRead3 > BLOCK_SIZE) amtToRead3 = BLOCK_SIZE;
		char content3[amtToRead3];
		ladd.blockNo = iptr.Block3Ptr.blockNo;
		ladd.segmentNo = iptr.Block3Ptr.segmentNo;
		printf("Read for section3: segmentNo=%d  block number=%d \n",ladd.segmentNo,ladd.blockNo);

		
		if (!Log_read(ladd, amtToRead3, content3)) {
			printf("file read %s\n", content3);
		//	memcpy(&buffer[BLOCK_SIZE*2], content3, amtToRead3); // FIXEME: buffer is point, point+offset can point the address
			memcpy(buffer+BLOCK_SIZE*2, content3, amtToRead3); 
			
		} else {
			printf("File_Read: error with Log_Read of file %d\n", inum);
		}
	}
	if (numBlocks >= 4) { //KATY FIX
		int amtToRead4 = length - 3*BLOCK_SIZE;
		if (amtToRead4 > BLOCK_SIZE) amtToRead4 = BLOCK_SIZE;
		char content4[amtToRead4];
		ladd.blockNo = iptr.Block4Ptr.blockNo;
		ladd.segmentNo = iptr.Block4Ptr.segmentNo;
		printf("Read for section4: segmentNo=%d  block number=%d \n",ladd.segmentNo,ladd.blockNo);

		
		if (!Log_read(ladd, amtToRead4, content4)) {
			printf("file read %s\n", content4);
		//	memcpy(&buffer[BLOCK_SIZE*3], content4, amtToRead4); // FIXEME: buffer is point, point+offset can point the address
			memcpy(buffer+BLOCK_SIZE*3, content4, amtToRead4); 
			
		} else {
			printf("File_Read: error with Log_Read of file %d\n", inum);
		}
	}
	//memcpy(buffer, &totalRead[offset], length-offset);

//	buffer[length-1] = '\0';  //FIXEME: buffer is not a char point
	printf("BUFFER: %s\n", buffer);
		printf("It's fine...\n");

	return 0;
} // end File_Free

int File_Free(int inum) {
	struct Inode inode = Get_Inode(inum);
	inode.in_use = 0; //Inode is no longer in use

}

int Change_Permissions(int inum, int permissions) {
	struct Inode fileinode = Get_Inode(inum);

	// Update the inode data
	fileinode.permissions = permissions;

	// Write the inode data
	struct Ifile write_ifile = Get_Ifile();
	write_ifile.data[inum] = fileinode;
	Put_Ifile(&write_ifile);
	return 0;
}

int Change_Owner(int inum, char owner) {
	struct Inode fileinode = Get_Inode(inum);
	
	// Update the inode data
	fileinode.owner = owner;

	// Write the inode data
	struct Ifile write_ifile = Get_Ifile();
	write_ifile.data[inum] = fileinode;
	Put_Ifile(&write_ifile);
	return 0;
}

int Change_Group(int inum, char* group, int groupLength) {
	printf("Group: %s \n", group);
	struct Inode fileinode = Get_Inode(inum);
	
	// Update the inode data
	fileinode.group = group;

	// Write the inode data
	struct Ifile write_ifile = Get_Ifile();
	write_ifile.data[inum] = fileinode;
	Put_Ifile(&write_ifile);
	return 0;
}


int Test_File_Create(int inum) {
	printf("############ begin Test_File_Create %d ############\n",inum);
	int type = TYPE_F;

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

int initFile(int size) {
	 // Create ifile inode, add it to the ifile data structure,
    // and write the ifile data structure to disk

    init("FuseFileSystem",size);  

    // Create the Ifile 
 //   File_Create(IFILE_INUM, TYPE_F);
	return 0;
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
	    if(!File_Read(num, 0, length, (void*)bufR)){
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
	int size=2048*4 -1;   
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
	inum num=1;
	int offset=1;
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
	inum num=1;
	int offset=1;
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
	char *buf="hello world";
	inum num=2;
	Test_File_Create(num);
	if(!File_Write(num, 0, 12, (void*)buf)){
		
	    char bufR[50];
	    char *expect="world";
	    if(!File_Read(num, 6, 12, (void*)bufR)){
	    	if(strcmp(expect,bufR)!=0){
				printf("Fail test 7:  read string %s does not match expect string %s \n",bufR, expect);
			}else{
				printf("**************Success    test 6F  pass*******************************\n");
			}
	    }
	}
}

void TestPermissions() {
	int inum = 3;
	int permissions = 771;
	Change_Permissions(inum, permissions);
	printf("per own grp sz  time \n");
	Print_Inode(inum);

	struct Inode inode = Get_Inode(inum);
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

	struct Inode inode = Get_Inode(inum);
	if (inode.owner != owner) {
		printf("Fail: old owner was not changed (old: %c, new: %c).\n ", inode.owner, owner);
	} else {
		printf("************* Success   TestOwner pass (%c == %c) **********\n", inode.owner, owner);
	}
	return;
}

void TestGroup() {
	int inum = 3;
	char group[6] = "greep";
	int groupLength = 6;
	Change_Group(inum, group, groupLength);
	printf("per own grp sz  time \n");
	Print_Inode(inum);

	struct Inode inode = Get_Inode(inum);
	if (inode.group != group) {
		printf("Fail: old group was not changed (old: %s, new: %s).\n ", inode.group, group);
	} else {
		printf("************* Success   TestOwner pass (%s == %s) **********\n", inode.group, group);
	}
	return;
}

// int main(){
// 	printf("Begin cfile layer, creating ifile (and its inode)...\n");
//    	initFile();
//    	Test_File_Create(0);
//     //Test_File_Create(1);
//     //test1F(); //stack smashing if blocksize is 50 but passes if 10
//     //Test_File_Create(2);
//     //test2F(); //seg fault if blocksize is 50, passes if 10
//     //Test_File_Create(3);
//     //test3F(); // original buffer has disappeared, passes if 10

//     //Overwrite
//     //test4F(); //overwrite original content
//     //test5F(); //concat
//     test6F(); //read offset
//     //printf("\n\n\n\n");
//     //Show_Ifile_Contents();
//     // TestOwner();
//     // TestPermissions();
//     // TestGroup();


// 	return 0;
// }