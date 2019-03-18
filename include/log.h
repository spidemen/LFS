#ifndef _LOG_H
#define _LOG_H


#include <iostream>
#include <time.h>
#include <map>
#include <unordered_map>
#include <ctime> 
#include <vector>
#include <string.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <flash.h>
using namespace std;

#define  N 4
typedef int inum;
typedef int segmentNo;

#define SUPERBLOCK	2
#define FLASH_SECTOR_SIZE 512
#define FLASH_SECTORS_PER_BLOCK 16

#define BLOCK_SIZE (FLASH_SECTOR_SIZE*4)

//int generateBlockNo=0;
// char *filename="FuseFileSystem";
struct logAddress{
	u_int blockNo=0;
	int segmentNo=0;
};


// class LOG {
// 	int init(char *fileSystemName);
// 	int Log_Write(inum num, u_int block, u_int length, void *buffer, logAddress &logAddress1);
// 	int Log_read(logAddress logAddress1, u_int length, void * buffer);
// 	int Log_free(logAddress logAddress1,u_int length);
// private:
// 	char *filename="FuseFileSystem";
// 	int generateBlockNo=0;
// 	int blocksize=2;
// //	int N=4;
// 	int currentsector;
// 	int currentSegment;
// };


struct Block{
	int blockNo;
	bool aLive;
	int  blockUse;
	char  data[BLOCK_SIZE];   // size in cache
	int  offset=0; 
};

struct SegmentSummary{
	int segmentNo=1;
    bool  inUse=false;
    int   liveByte;
	time_t   modifiedTime=time(NULL);
    int  totalBlock=4; 
    map<inum,int> tables;   // inum associated with block No
};
struct Data{
	map<int,Block> data;
};
struct Segment{
	SegmentSummary *summary=new SegmentSummary;
	map<int,Block> data ;   // pair block number and Block structure
	Data *pdata=new Data;
};

// use one segment to hold metadata for file system
struct metadata{
	int blocksize;
	int segmentsize;
	int segments;
	int limit;
	int currentsector;
	map<segmentNo,SegmentSummary> segmentUsageTable;
	int checkpointStart;     // start block of checkpoint
};

// for now, just put all the function here, later on will put all of them into a class
int init(char *fileSystemName);
int Log_Write(inum num, u_int block, u_int length, void *buffer, logAddress &logAddress1);
int Log_read(logAddress logAddress1, u_int length, void * buffer);
int Log_free(logAddress logAddress1,u_int length);
#endif