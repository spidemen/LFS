#ifndef _LOG_H
#define _LOG_H
#include <iostream>
#include <time.h>
#include <map>
#include <unordered_map>
#include <ctime> 
using namespace std;
struct logAddress{
	u_int block;
	int segment;
};

struct Segment{
	int segmentNo;
    bool  inUse=false;
    int   liveByte;
	time_t   modifiedTime=time(NULL);
    int  totalBlock;     
};
// use one segment to hold metadata for file system
struct metadata{
	int blocksize;
	int segmentsize;
	int segments;
	map<int,Segment> segmentTable;
	int checkpointStart;     // start block of checkpoint
};

struct Block{
	int blockNo;
	bool aLive;
	int  blockUse;
};
typedef int inum;
#endif