#ifndef _LOG_H
#define _LOG_H
#include <iostream>
#include <time.h>
#include <map>
#include <unordered_map>
#include <ctime> 
#include <vector>
using namespace std;

#define  N 10 
int generateSegmentNo=1;
string filename;
struct logAddress{
	u_int blockNo;
	int segmentNo;
};

struct Block{
	int blockNo;
	bool aLive;
	int  blockUse;
	char  data[FLASH_BLOCK_SIZE];   // size in cache
	int  offset=0; 
};

struct Segment{
	int segmentNo;
    bool  inUse=false;
    int   liveByte;
	time_t   modifiedTime=time(NULL);
    int  totalBlock; 
    vector<Block> block;      
};

// use one segment to hold metadata for file system
struct metadata{
	int blocksize;
	int segmentsize;
	int segments;
	map<int,Segment> segmentTable;
	int checkpointStart;     // start block of checkpoint
};

typedef int inum;
vector<pair<inum, Segment> > MRC;   // implement N most access segment policy
#endif