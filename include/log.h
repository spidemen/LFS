#ifndef _LOG_H
#define _LOG_H

<<<<<<< HEAD

=======
// #ifdef CPLUSPLUS
// extern "C"{
// #endif

#ifdef __cplusplus
extern "C" {
#endif
>>>>>>> ebeb226ab1ba6b6e0b7d968858be397a7dca2ad5

#include <time.h>
//#include <map>
//#include <unordered_map>
//#include <ctime> 
//#include <vector>
#include <string.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
//#include <iostream>
//#include <algorithm>
#include <flash.h>
#include <stdbool.h>
//using namespace std;

#define  N 4
typedef int inum;
typedef int segmentNo;

#define SUPERBLOCK	2
#define FLASH_SECTOR_SIZE 512
#define FLASH_SECTORS_PER_BLOCK 16

#define BLOCK_SIZE (FLASH_SECTOR_SIZE*4)

#define FILENAMESIZE 50

#define BLOCK_NUMBER  4
#define TOTALBLOCK  6
//int generateBlockNo=0;
// char *filename="FuseFileSystem";

struct logAddress{
	u_int blockNo;
	int segmentNo;
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
//	char  data[BLOCK_SIZE];   // size in cache
	char  *data;   
	int  offset; 
};

<<<<<<< HEAD
struct SegmentSummary{
	int segmentNo;
    bool  inUse;
    int   liveByte;
	time_t   modifiedTime;
    int  totalBlock; 
    int  INUM[BLOCK_NUMBER];
    int  BlockNumber[BLOCK_NUMBER];
  //  map<inum,int> tables;   // inum associated with block No
};
=======
// struct SegmentSummary{
// 	  int segmentNo;
//     bool  inUse;
//     int   liveByte;
//   	time_t   modifiedTime;
// 		int  *blockUsed;
//     int  totalBlock; 
//     int  INUM[BLOCK_NUMBER];
//     int  BlockNumber[BLOCK_NUMBER];
//   //  map<inum,int> tables;   // inum associated with block No
// };
>>>>>>> ebeb226ab1ba6b6e0b7d968858be397a7dca2ad5


struct lData{
	int blockNo;
   struct Block B;
	struct lData *next;
};

struct Segment{
	struct SegmentSummary *summary;
//	map<int,Block> data ;   // pair block number and Block structure
//	Data *pdata=new Data;
	struct lData *head;
	bool used;
	int currenIndex;
//	struct Block  dataB[BLOCK_NUMBER];
	struct Block *dataB;
};

// use one segment to hold metadata for file system
<<<<<<< HEAD
struct metadata{
	int blocksize;
	int segmentsize;
	int segments;
	int limit;
	int currentsector;
	char filename[FILENAMESIZE];
//	map<segmentNo,SegmentSummary> segmentUsageTable;
   
	int checkpointStart;     // start block of checkpoint
	int checkpointEnd;
};

// for now, just put all the function here, later on will put all of them into a class
int init(char *fileSystemName);
int Log_Write(inum num, u_int block, u_int length, void *buffer, struct logAddress *logAddress1);
int Log_read(struct logAddress logAddress1, u_int length, void * buffer);
int Log_free(struct logAddress logAddress1,u_int length);


=======
// struct metadata{
// 	int blocksize;
// 	int segmentsize;
// 	int segments;
// 	int limit;
// 	int currentsector;
// 	char filename[FILENAMESIZE];
// //	map<segmentNo,SegmentSummary> segmentUsageTable;
   
// 	int checkpointStart;     // start block of checkpoint
// 	int checkpointEnd;
// };

// for now, just put all the function here, later on will put all of them into a class
extern int init(char *fileSystemName);
extern int Log_Write(inum num, u_int block, u_int length, void *buffer, struct logAddress *logAddress1);
extern int Log_read(struct logAddress logAddress1, u_int length, void * buffer);
extern int Log_free(struct logAddress logAddress1,u_int length);
extern int Log_writeDeadBlock(inum num,struct logAddress oldAddress,struct logAddress newAddress);
extern int Log_recordIfile(struct logAddress *oldAdrress,struct logAddress *newAdress, int  oldSize, int newSize);

#ifdef __cplusplus
}
#endif
>>>>>>> ebeb226ab1ba6b6e0b7d968858be397a7dca2ad5

#endif