

/******************
combination of C and C++
rewrite C++ into C for integrating with fuse

*********************/

#include "log.h"

#include <iostream>
#include <map>
#include <unordered_map>
#include <ctime>
#include <vector>
using namespace std;

struct SegmentSummary
{
	int segmentNo;
	bool alive = true;
	int deadblock = 0;
	time_t modifiedTime;
	long age;
	int totalBlock;
	int aliveByte = 0;
	long costBenefitRate = 0.0;
	//    multimap<inum,int> tables;   // inum associated with block No
	pair<int, int> tables[32];
	//  map<int,int> blocksByte;
	pair<int, int> blocksByte[32]; // blockNumber and data size
	int tablesSize = 0;
	int blocksByteSize = 0;
};

struct SegmentUsageTable
{
	int segmentNo;
	SegmentSummary summary;
};

struct metadata
{

	int blocksize;
	int segmentsize;
	int segments;
	int limit;
	int currentsector;
	int currentBlockNumber;
	int currentSegmentNumber;
	char filename[FILENAMESIZE];
	time_t checkPiontTime;
	int checkPointsize = 0;
	SegmentUsageTable segmentUsageTable[MAX_SEGMENT];
	int segementUsageSize = 0;

	//	map<segmentNo,SegmentSummary> segmentUsageTable;
	pair<int, bool> blocksStatus[MAX_BLOCK]; // state of each block alive or dead
	int blocksStatusSize = 0;
	int reUsedTable[MAX_SEGMENT / 10];
	struct logAddress checkPointRegion[20];
	int reUsedTableSize = 0;
};

multimap<inum, int> tables;
map<int, int> blocksByte;
map<int, bool> blocksStatus;
map<segmentNo, SegmentSummary> segmentUsageTable;
map<segmentNo, SegmentSummary> reUsedTable;
map<long, int, greater<long>> costBenefitTable;

char *filename = "FuseFileSystem";
int blocksize = 4;
int generateBlockNo = 0;
int startsector;
int cache = 4;
int cleanSegment = 0;
struct Segment *segmentCache = NULL;
struct Segment MRA[N];
struct metadata *pmetadata = NULL;

bool DoesFileExist(char *filename)
{
	FILE *fp = fopen(filename, "r");
	if (fp != NULL)
	{
		fclose(fp);
		return true;
	}
	else
	{
		return false;
	}
}

int init(char *fileSystemName, int cachesize = 4)
{

	cout << "C++ compiler test  init function " << endl;
	// init data struct
	segmentCache = (struct Segment *)malloc(sizeof(struct Segment));
	pmetadata = (struct metadata *)malloc(SUPERBLOCK * FLASH_SECTOR_SIZE);
	//	pmetadata=new metadata;
	segmentCache->summary = (struct SegmentSummary *)malloc(sizeof(struct SegmentSummary));
	//	segmentCache->summary=new SegmentSummary;

	for (int i = 0; i < N; i++)
	{
		MRA[i].used = false;
	}
	if (!DoesFileExist(fileSystemName))
	{   // comment for testing file layer
		// 	 string cmd="";
		char cmd[50];
		memset(cmd, 0, 50);
		strcat(cmd, "./mklfs -b 2 -l 32 ");
		strcat(cmd, fileSystemName);
		// cmd=cmd+"./mklfs -b 4 -l 4 "+fileSystemName;
		// system(cmd.c_str());
		system(cmd);
	}
	// commnet for testing file layer
	u_int blocks;
	Flash f = Flash_Open(filename, FLASH_ASYNC, &blocks);
	cache = cachesize;
	if (f != NULL)
	{
		char buf[SUPERBLOCK * FLASH_SECTOR_SIZE];
		if (Flash_Read(f, 0, SUPERBLOCK, buf))
		{
			//  cout<<"Init Error Cannot read first two sector of file system"<<endl;
			printf("Init Error Cannot read first two sector of file system \n");
			return 0;
		}
		else
		{
			//    pmetadata= (struct metadata *)buf;
			memcpy(pmetadata, buf, SUPERBLOCK * FLASH_SECTOR_SIZE);
			segmentCache->summary->totalBlock = pmetadata->segmentsize;
			blocksize = pmetadata->blocksize;
			//      startsector=segmentCache->summary->totalBlock*2*blocksize;  // first segment hold metafile system, second segment hold ifile data
			startsector = pmetadata->currentsector;
#define BLOCK_SIZE (FLASH_SECTOR_SIZE * blocksize)
#define BLOCK_NUMBER (segmentCache->summary->totalBlock - 1)
#define N cache

			segmentCache->dataB = (struct Block *)malloc(sizeof(struct Block) * BLOCK_NUMBER);

			segmentCache->summary->segmentNo = pmetadata->currentSegmentNumber;
			segmentCache->currenIndex = 0;
			generateBlockNo = pmetadata->currentBlockNumber;
			// init segmentUsage table
			for (int i = 0; i < pmetadata->segementUsageSize; i++)
			{
				segmentUsageTable.insert(pair<segmentNo, SegmentSummary>(pmetadata->segmentUsageTable[i].segmentNo, pmetadata->segmentUsageTable[i].summary));
			}
			for (int i = 0; i < pmetadata->blocksStatusSize; i++)
			{
				blocksStatus.insert(pair<int, bool>(pmetadata->blocksStatus[i].first, pmetadata->blocksStatus[i].second));
			}

			//    int startblock;
			//    if(startsector%FLASH_SECTORS_PER_BLOCK==0){
			//    	startblock=startsector/FLASH_SECTORS_PER_BLOCK;
			//    } else{
			//    	startblock=(startsector/FLASH_SECTORS_PER_BLOCK)+1;
			//    	startsector=startblock*FLASH_SECTORS_PER_BLOCK;
			//    }
			//    int  totalErase=(segmentCache->summary->totalBlock*blocksize)/FLASH_SECTORS_PER_BLOCK;
			// if(Flash_Erase(f,startblock,totalErase)) {cout<<"Init error: cannot erase next segment "<<endl;}
			// for(int i=0;i<pmetadata->reUsedTableSize;i++){
			// 	reUsedTable.insert(pair<segmentNo,SegmentSummary>(pmetadata->reUsedTable[i].segmentNo,pmetadata->reUsedTable[i].summary));
			// }

			printf("init blocksize per sector %d   segment size (blocks) %d  startsector =%d \n", blocksize, pmetadata->segmentsize, startsector);
			printf(" BLOCKSIXE= %d   BLOCKNUMBER=%d\n", BLOCK_SIZE, BLOCK_NUMBER);
			cout << " current sector =" << startsector << "  current blockNo=" << generateBlockNo << " currentSement=" << pmetadata->currentSegmentNumber << endl;
			cout << "usage table size =" << pmetadata->segementUsageSize << "   blocksStatus size =" << pmetadata->blocksStatusSize <<" checkpointsize="<<pmetadata->checkPointsize<< endl;
		}
	}
	return 1;
}

int Log_GetIfleAddress(struct logAddress *Adrress, int size)
{
	memcpy(Adrress, pmetadata->checkPointRegion, sizeof(struct logAddress) * (pmetadata->checkPointsize));
//	memcpy(size,&pmetadata->checkPointsize,sizeof(int));
	size = pmetadata->checkPointsize;
	if (size == 0)
	{
		cout << "checkpoint address error: No any checkpoint, check...." << endl;
		return 0; //Katy KEEP 0 IF NONE EXISTS
	}
	return size;
}
int Log_destroy()
{
	struct logAddress newaddress;
	segmentCache->currenIndex = BLOCK_NUMBER; // reset current Index so that write segment to disk
	char buf[50] = "write live data to the flash";
	Log_Write(0, 1, 50, (void *)buf, &newaddress); // write segment data to disk
    struct logAddress *newOne=(struct logAddress*)malloc(sizeof(struct logAddress)*(pmetadata->checkPointsize));
    struct logAddress *oldOne=(struct logAddress*)malloc(sizeof(struct logAddress)*(pmetadata->checkPointsize));
	memcpy(oldOne, pmetadata->checkPointRegion, sizeof(struct logAddress) * pmetadata->checkPointsize);
	memcpy(newOne, pmetadata->checkPointRegion, sizeof(struct logAddress) *pmetadata->checkPointsize);
	Log_CheckPoint(oldOne,newOne, pmetadata->checkPointsize, pmetadata->checkPointsize); // checkpoint
	cout<<"Destory Write Last live data to flash  segmentNo "<<newaddress.segmentNo<<"  blockNo= "<<newaddress.blockNo<<endl;
	return 0;
}
// cleanning , every write deadblock to check whether segment neeed to clean based on costbenefit rate
// there are two different way to do clean, one just directly clean dead segment, other way based on costbenefit rate
int Log_writeDeadBlock(inum num, struct logAddress oldAddress, struct logAddress newAddress)
{
	cout << "Attempt to clean segment " << oldAddress.segmentNo << endl;
	auto it = segmentUsageTable.find(oldAddress.segmentNo);
	if (it != segmentUsageTable.end())
	{
		it->second.deadblock++;
		int liveByteBlock = 0;
		for (int i = 0; i < it->second.blocksByteSize; i++)
		{
			if (it->second.blocksByte[i].first == oldAddress.blockNo)
			{ // cleaning byte on block
				liveByteBlock = it->second.blocksByte[i].second;
				it->second.blocksByte[i].second = 0;
				break;
			}
		}
		//	  cout<<"block no "<<oldAddress.blockNo<<"  live Byte "<<liveByteBlock<<endl;
		it->second.aliveByte -= liveByteBlock; // live data on segment
		blocksStatus[oldAddress.blockNo] = false;
		float u = 1 - (float)(it->second.deadblock / (float)(it->second.totalBlock - 1)); // segment usage rate
		it->second.costBenefitRate = ((1 - u) * it->second.age) / (1 + u);				  // cost benefit rate
																						  //   cout<<"segment "<<it->second.segmentNo<<"blockNo="<<oldAddress.blockNo<<"u="<<u<<" deadblock="<<it->second.deadblock<<" aliveByte="<< it->second.aliveByte<<endl;
		costBenefitTable.insert(pair<long, int>(it->second.costBenefitRate, it->second.segmentNo));
		//  check segment dead or not
		if (it->second.aliveByte == 0 ||
			it->second.deadblock == it->second.totalBlock - 1)
		{
			it->second.alive = false;
			reUsedTable.insert(pair<segmentNo, SegmentSummary>(oldAddress.segmentNo, it->second));
			cleanSegment++;
			// cleaning
			cout << "segment " << it->second.segmentNo << "blockNo=" << oldAddress.blockNo << " costBenefitRate =" << it->second.costBenefitRate << "  u=" << u << endl;
			if (cleanSegment == THREADSHOLD)
			{
				for (auto segmentIt : reUsedTable)
				{
					if (segmentIt.second.alive)
						continue;
					int segmentNo = segmentIt.first;
					int segmentStartSector = (segmentNo)*segmentCache->summary->totalBlock * blocksize;
					int startblock = segmentStartSector / FLASH_SECTORS_PER_BLOCK;
					int totalErase = (segmentCache->summary->totalBlock * blocksize) / FLASH_SECTORS_PER_BLOCK;

					cout << " clean segment =" << segmentNo << endl;
					if (segmentStartSector % FLASH_SECTORS_PER_BLOCK != 0)
					{   // default setting make block size or segment  modual by FLASH_SECTORS_PER_BLOCK
						// hard to implement, would waster a lot of space
					}
					u_int blocks;
					Flash f = Flash_Open(filename, FLASH_ASYNC, &blocks);
					if (f != NULL)
					{
						if (Flash_Erase(f, startblock, totalErase))
						{
							printf("cannot erase startblock(flash ) %d  totalBlock=%d \n", startblock, totalErase);
						}
						else
						{
							segmentIt.second.alive = true;
						}
					}
					else
					{
						printf("Flash Open Error \n");
					}

					Flash_Close(f);
				}
				cleanSegment = 0;
			}
		}
		// chose high rate segment to clean , ThreadShould --1 for cost benefit clean
		//  cout<<"costBenefitTable size ="<<costBenefitTable.size()<<" rate="<<it->second.costBenefitRate<<"  biggest rate "<<costBenefitTable.begin()->first<<endl;
		if (segmentUsageTable.size() > 1 && it->second.costBenefitRate == costBenefitTable.begin()->first)
		{

			auto exitSegment = reUsedTable.find(it->second.segmentNo);
			if (exitSegment == reUsedTable.end())
			{
				int segmentNo = it->second.segmentNo;
				int segmentStartSector = (segmentNo)*segmentCache->summary->totalBlock * blocksize;
				int startblock = segmentStartSector / FLASH_SECTORS_PER_BLOCK;
				int totalErase = (segmentCache->summary->totalBlock * blocksize) / FLASH_SECTORS_PER_BLOCK;
				cout << "costbenefit  clean segment =" << segmentNo << endl;
				// copy live data to the tail of log
				logAddress address;
				address.segmentNo = segmentNo;
				address.blockNo = it->second.blocksByte[0].first;
				int sizebuffer = (it->second.totalBlock - 1) * blocksize * FLASH_SECTOR_SIZE;
				char buf[sizebuffer];
				u_int blocks;
				Flash f = Flash_Open(filename, FLASH_ASYNC, &blocks);
				if (f != NULL)
				{
					if (!Log_read(address, sizebuffer, buf))
					{
						struct Block *pdata = (struct Block *)buf;

						for (int i = 0; i < it->second.blocksByteSize; i++)
						{
							//       	cout<<"blockNo="<<it->second.blocksByte[i].first<<" block live data size "<<it->second.blocksByte[i].second<<" content ="<<pdata[i].data<<endl;
							if (it->second.blocksByte[i].second != 0)
							{ // cleaning byte on block
								liveByteBlock = it->second.blocksByte[i].second;
								if (!Log_Write(1, 0, it->second.blocksByte[i].second, pdata[i].data, &address))
								{
									//	 	cout<<"costbenefit copy  block "<<it->second.blocksByte[i].first<<" live data "<<endl;
								}
								else
								{
									cout << "Error: constBenefit clean write error  segment =" << segmentNo << endl;
								}

								it->second.blocksByte[i].second = 0;
							}
						}
					}
					else
					{
						cout << "Error: constBenefit read segment error  segmentNo= " << segmentNo << endl;
					}

					// cleaning segment
					if (Flash_Erase(f, startblock, totalErase))
					{
						printf("cannot erase startblock(flash ) %d  totalBlock=%d \n", startblock, totalErase);
					}
					else
					{
						it->second.alive = true;
						reUsedTable.insert(pair<int, SegmentSummary>(it->second.segmentNo, it->second));
					}
				}
				else
				{
					printf("Flash Open Error \n");
				}

				Flash_Close(f);
			}
		}
	}
	else
	{
		cout << "Error: invalid log address, does not exit in segmentUsage tabel segmentNo=" << oldAddress.segmentNo << endl;
		return 0;
	}

	return 1;
}

// checkpoint

int Log_CheckPoint(struct logAddress *oldAdrress, struct logAddress *newAdress, int oldSize, int newSize)
{
	//  pmetadata->checkPointRegion=(struct logAddress*)malloc(sizeof(struct logAddress)*(newSize+1));
	// store ifile
	memcpy(pmetadata->checkPointRegion, newAdress, sizeof(struct logAddress) * newSize);
	// update checkpoint time
	pmetadata->checkPiontTime = time(NULL);
	pmetadata->checkPointsize = newSize;

	pmetadata->currentsector = startsector;
	pmetadata->currentBlockNumber = generateBlockNo - 1;
	pmetadata->currentSegmentNumber = segmentCache->summary->segmentNo;
	pmetadata->blocksize = blocksize;
	pmetadata->segmentsize = segmentCache->summary->totalBlock;
	pmetadata->segementUsageSize = segmentUsageTable.size();
	auto it = segmentUsageTable.begin();
	for (int i = 0; i < segmentUsageTable.size() && it != segmentUsageTable.end(); i++)
	{ // store segment usage table
		pmetadata->segmentUsageTable[i].segmentNo = it->first;
		pmetadata->segmentUsageTable[i].summary = it->second;
		//	   cout<<"i="<<i<<"  summary segmentNo "<<pmetadata->segmentUsageTable[i].summary.segmentNo<<endl;
		it++;
	}

	pmetadata->blocksStatusSize = blocksStatus.size();
	int index = 0;
	for (auto it1 : blocksStatus)
	{   // store block status table
		//	cout<<"index ="<<index<<"   "<<pmetadata->blocksStatusSize<<"  "<<it1.first<<endl;
		pmetadata->blocksStatus[index++] = {it1.first, it1.second};
	}

	int totalErase = (segmentCache->summary->totalBlock * blocksize) / FLASH_SECTORS_PER_BLOCK;
	u_int blocks;
	Flash f = Flash_Open(filename, FLASH_ASYNC, &blocks);
	if (f != NULL)
	{
		if (Flash_Erase(f, 0, totalErase))
		{
			printf("cannot erase block\n");
		} // earse block

		if (Flash_Write(f, 0, SUPERBLOCK, (void *)pmetadata))
		{ // do checkpoint
			cout << "Error: cannot write metadata to the flash " << endl;
			return 1;
		}
		else
		{
			cout << "Success checkpoint time " << time(NULL) <<"totalErase "<<totalErase<< endl;
			return 0;
		}
	}
	else
	{
		printf("Flash Open Error \n");
		return 1;
	}
	Flash_Close(f);
}

int Log_Write(inum num, u_int block, u_int length, void *buffer, struct logAddress *logAddress1)
{
	u_int blocks;

	Flash f = Flash_Open(filename, FLASH_ASYNC, &blocks);
	//   struct SegmentSummary *summary=(struct SegmentSummary*)malloc(sizeof(struct SegmentSummary));

	if (segmentCache->currenIndex > 0 && segmentCache->currenIndex % BLOCK_NUMBER == 0)
	{ // segment full, write the disk

		if (f != NULL)
		{
			int totalSector = blocksize;
			SegmentSummary *summary = new SegmentSummary;
			segmentCache->summary->modifiedTime = time(NULL);
			memcpy(summary, segmentCache->summary, sizeof(struct SegmentSummary));
			int index = 0;
			summary->tablesSize = tables.size();
			for (auto it : tables)
			{ // push data to tables <inum,  blockNO>
				summary->tables[index++] = {it.first, it.second};
			}
			tables.clear();
			index = 0;
			summary->blocksByteSize = blocksByte.size();
			for (auto it : blocksByte)
			{
				summary->blocksByte[index++] = {it.first, it.second};
			}
			blocksByte.clear();
			summary->age = (long)time(NULL);
			if (Flash_Write(f, startsector, totalSector, (void *)summary))
			{ // write segment summary into disk
				printf("LogWrite  Error: canont write segment summary inum= %d   fileblock  =%d  startsector=%d \n", num, block, startsector);
				return 1;
			}
			else
			{
				// write segment into segment usagetable
				segmentUsageTable.insert(pair<segmentNo, SegmentSummary>(segmentCache->summary->segmentNo, *summary));
				//  pmetadata->segmentUsageTable.insert(pair<segmentNo,SegmentSummary>(segmentCache->summary->segmentNo,*summary));

				// write data to disk
				startsector = startsector + totalSector;
				struct Block *tmp = (struct Block *)malloc(sizeof(struct Block) * BLOCK_NUMBER);
				memcpy(tmp, segmentCache->dataB, sizeof(struct Block) * BLOCK_NUMBER);
				totalSector = (segmentCache->summary->totalBlock - 1) * blocksize;

				if (Flash_Write(f, startsector, totalSector, (void *)tmp))
				{ // write segment data into disk
					printf("LogWrite  Error: canont segment data inum= %d  fileblock  =%d  startsector=%d \n", num, block, startsector);
					return 1;
				}
				else
				{

					startsector = startsector + totalSector;
					// implement N most recently used access segment
					struct Segment p1;
					p1.dataB = (struct Block *)malloc(sizeof(struct Block) * BLOCK_NUMBER);
					memcpy(p1.dataB, segmentCache->dataB, sizeof(struct Block) * BLOCK_NUMBER);
					p1.summary = (struct SegmentSummary *)malloc(sizeof(struct SegmentSummary));
					memcpy(p1.summary, segmentCache->summary, sizeof(struct SegmentSummary));
					p1.used = true;
					if (MRA[N - 1].used)
					{ // update cache
						free(MRA[0].dataB);
						MRA[0].dataB = NULL;
						free(MRA[0].summary);
						MRA[0].summary = NULL;
						//	cout<<"free segment "<<MRA[0].summary->segmentNo<<endl;
						for (int i = 1; i < N; i++)
						{
							MRA[i - 1] = MRA[i];
						}
						MRA[N - 1] = p1; // move most recently used on the tail of queue
					}
					else
					{
						for (int i = 0; i < N; i++)
						{
							if (MRA[i].used)
								continue;
							MRA[i] = p1;
							break;
						}
					}

					// reset segment data
					segmentCache->summary->segmentNo++; // empty segment , increase segment number for next write
					memset(segmentCache->dataB, 0, sizeof(struct Block) * BLOCK_NUMBER);
					//    segmentCache->summary->tables.clear();
					segmentCache->summary->aliveByte = 0;
					//     segmentCache->summary->blocksByte.clear();

					cout << "Sector point: Success Finish write flash. current " << startsector << " totalSector=" << totalSector << endl;
				}

				free(tmp);
				tmp = NULL;
			}

			delete summary;
			Flash_Close(f);
		}
		else
		{
			// 	 cout<<"Error: Fail to open log file  "<<filename<<endl;
			printf("Error: Fail to open log file  =%s \n", filename);
			return 1;
		}
	}

	// write data to memory
	int tmp = length;
	if (tmp > 0)
	{
		generateBlockNo++;
		int blockIndex = segmentCache->currenIndex % BLOCK_NUMBER;
		memset(segmentCache->dataB[blockIndex].data, '\0', BLOCK_SIZE - 100);
		memcpy(segmentCache->dataB[blockIndex].data, buffer, length);

		segmentCache->dataB[blockIndex].blockNo = generateBlockNo;
		segmentCache->dataB[blockIndex].datasize = length;
		segmentCache->dataB[blockIndex].modifiedTime = time(NULL);
		segmentCache->summary->aliveByte += length;
		segmentCache->summary->age = (long)time(NULL);

		tables.insert(pair<inum, int>(num, generateBlockNo));
		blocksByte.insert(pair<int, int>(generateBlockNo, length));
		blocksStatus.insert(pair<int, bool>(generateBlockNo, true));
		//		cout<<"segmentNo="<<segmentCache->summary->segmentNo<<  "block No="<<generateBlockNo<<"  content="<<segmentCache->dataB[blockIndex].data<<">"<<endl;
		//	    printf("blockNumber=%d  index=%d  currentindex=%d\n", segmentCache->dataB[blockIndex].blockNo,blockIndex,segmentCache->currenIndex);
		segmentCache->currenIndex++;
	}
	logAddress1->segmentNo = segmentCache->summary->segmentNo;
	logAddress1->blockNo = generateBlockNo;

	printf(" logadress segmentNo= %d  blockNo=%d  blockIndex=%d \n", logAddress1->segmentNo, logAddress1->blockNo, segmentCache->currenIndex - 1);
	return 0;
}
int Log_read(struct logAddress logAddress1, u_int length, void *buffer)
{

	if (segmentCache->summary->segmentNo == logAddress1.segmentNo)
	{ // check current segment cache
		int i;
		for (i = 0; i < segmentCache->summary->totalBlock; i++)
		{
			//   	 printf("test blockNo %d i=%d\n ",segmentCache->dataB[i].blockNo,i);
			if (segmentCache->dataB[i].blockNo == logAddress1.blockNo)
				break;
		}
		if (length <= BLOCK_SIZE)
		{
			memcpy(buffer, segmentCache->dataB[i].data, length);
			// 	printf("i= %d  content =%s \n", i,segmentCache->dataB[i].data);
		}
		else
		{
			memcpy(buffer, &segmentCache->dataB[i], length);
		}
		return 0;
	}
	int index = 0;

	for (index = N - 1; index >= 0; index--)
	{   // start from end of queue search (most laster on the end)
		// 	printf("index=%d SegmentNo=%d \n",index,MRA[index].summary->segmentNo);
		if (MRA[index].used && MRA[index].summary->segmentNo == logAddress1.segmentNo)
			break;
	}
	//  index=3;
	if (index >= 0)
	{
		struct Segment p = MRA[index];

		char *block = NULL;
		int i;
		for (i = 0; i < BLOCK_NUMBER; i++)
		{
			if (p.dataB[i].blockNo == logAddress1.blockNo)
			{
			//	cout<<"debug log read segmentNo "<<p.dataB[i].segmentNo<<" blockNo "<<p.dataB[i].blockNo<<endl;
				block = p.dataB[i].data;
				break;
			}
		}
		if (i <= BLOCK_NUMBER - 1)
		{

			printf("find segment %d in the queue  blockNo=%d  i=%d \n", logAddress1.segmentNo, p.dataB[i].blockNo, i);
			//	cout<<p.dataB[i+1].data<<"  "<<index<<endl;
			if (length <= BLOCK_SIZE)
			{
				//	  memcpy(buffer,block->second.data,length);
				memcpy(buffer, block, length);
			}
			else
			{
				//	 memcpy(buffer,block->second.data,BLOCK_SIZE);
				memcpy(buffer, &p.dataB[i], length);
			}
		}
		else
		{
			printf("Error: segment No=  =%d   do not have blockNo  =%d \n", logAddress1.segmentNo, logAddress1.blockNo);
			return 1;
		}

		free(MRA[index].dataB);
		free(MRA[index].summary);
		MRA[index].dataB = NULL;
		MRA[index].summary = NULL;
		for (i = index; i < N - 1; i++)
		{
			MRA[i] = MRA[i + 1];
			if (!MRA[i].used)
				break;
		}
		if (!MRA[i].used)
			MRA[i] = p;
		else if (i == N - 2)
		{
			MRA[i + 1] = p;
		}
	}
	else
	{ // read from flash
		int StartSector = (logAddress1.segmentNo) * segmentCache->summary->totalBlock * blocksize;
		int TotalSector = (segmentCache->summary->totalBlock - 1) * blocksize;
		u_int blocks;
		char buf[BLOCK_SIZE];
		printf("read from disk : startsector =%d  total sector  =%d   read  segmentNo= %d  blockNo =%d \n", StartSector, TotalSector, logAddress1.segmentNo, logAddress1.blockNo);
		Flash f = Flash_Open(filename, FLASH_ASYNC, &blocks);
		if (f != NULL)
		{
			if (Flash_Read(f, StartSector, blocksize, buf))
			{
				printf("LogRead Segment Error: startsector =%d  total sector  =%d  canont read  segmentNo= %d  blockNo =%d \n", StartSector, blocksize, logAddress1.segmentNo, logAddress1.blockNo);
				return 1;
			}
			else
			{
				struct SegmentSummary *summary = (struct SegmentSummary *)buf;
				StartSector = StartSector + blocksize;
				char buf1[TotalSector * FLASH_SECTOR_SIZE];
				if (Flash_Read(f, StartSector, TotalSector, buf1))
				{
					printf("LogRead Segment Error: startsector  =%d  total sector  =%d  canont read   segmentNo= %d  blockNo =%d \n", StartSector, TotalSector, logAddress1.segmentNo, logAddress1.blockNo);
					return 1;
				}
				else
				{
					struct Block *pdata = (struct Block *)buf1;
					int i;
					for (i = 0; i < BLOCK_NUMBER; i++)
					{
						//  	 printf(" read from flash i=%d blockNo=%d \n",i,pdata[i].blockNo);
						if (pdata[i].blockNo == logAddress1.blockNo)
						{
							break;
						}
					}
					if (i <= BLOCK_NUMBER - 1)
					{
						if (length <= BLOCK_SIZE)
						{
							printf("i=%d   content  %s\n", i, pdata[i].data);
							memcpy(buffer, pdata[i].data, length);
						}
						else
						{
							memcpy(buffer, &pdata[i], length);
						}
					}
					else
					{
						printf(" Error read from disk: segment No= %d  do not have blockNo= %d \n", logAddress1.segmentNo, logAddress1.blockNo);
						printf("LogRead Segment Error: startsector  =%d  total sector  =%d  segmentNo= %d  blockNo =%d \n", StartSector, TotalSector, logAddress1.segmentNo, logAddress1.blockNo);
						return 1;
					}
					struct Segment p;
					p.summary = (struct SegmentSummary *)malloc(sizeof(struct SegmentSummary));
					memcpy(p.summary, summary, sizeof(struct SegmentSummary));
					//	p.summary=summary;
					p.dataB = (struct Block *)malloc(sizeof(struct Block) * BLOCK_NUMBER);
					;
					memcpy(p.dataB, pdata, sizeof(struct Block) * BLOCK_NUMBER);

					free(MRA[0].dataB);
					free(MRA[0].summary);
					MRA[0].dataB = NULL;
					MRA[0].summary = NULL;
					for (int i = 1; i < N; i++)
					{
						MRA[i - 1] = MRA[i];
					}
					MRA[N - 1] = p;
					return 0;
				}
			}
			Flash_Close(f);
		}
		else
		{
			printf("Error: Fail to open Flash file %s\n", filename);
			return 1;
		}
	}

	return 0;
}
int Log_free(struct logAddress logAddress1, u_int length)
{
	u_int blocks;
	Flash f = Flash_Open(filename, FLASH_ASYNC, &blocks);
	if (f != NULL)
	{
		int StartSector = (logAddress1.segmentNo - 1 + segmentCache->summary->totalBlock) * blocksize;
		int startblock = StartSector / FLASH_SECTORS_PER_BLOCK;
		int blocks = length / FLASH_BLOCK_SIZE + 1;
		if (Flash_Erase(f, startblock, blocks))
		{
			//	cout<<"Error: Fail to erase blocks segmentNo ="<<logAddress1.segmentNo<<" blcokNo ="<<logAddress1.blockNo<<endl;
			printf("Error: Fail to erase blocks segmentNo = %d  blcokNo = %d \n", logAddress1.segmentNo, logAddress1.blockNo);
			return 1;
		}
		else
		{
			//			printf("Error: Fail to erase blocks segmentNo = %d  blcokNo = %d \n", logAddress1.segmentNo,logAddress1.blockNo );
			return 0;
		}
	}
	else
	{
		//	 cout<<"Error: Fail to open log file  "<<filename<<endl;
		return 1;
	}
}

void test1(char *cat)
{
	//	printf("*******************Log layer test 1 simple small write and read ****************************** \n");
	char buf[50] = "Hello LFS, welcome to CSC 545 OS class";
	strcat(buf, cat);
	//	char  *buf="Hello LFS, welcome to CSC 545 OS class";
	inum num = 1;
	struct logAddress address;
	if (!Log_Write(num, 1, 50, (void *)buf, &address))
	{
		//	cout<<"buf ="<<buf<<">"<<endl;
		char bufR[50];
		if (!Log_read(address, 50, (void *)bufR))
		{
			printf("return logadress segmentNo= %d  blockNo=%d \n", address.segmentNo, address.blockNo);
			if (strcmp(buf, bufR) != 0)
			{
				printf("Fail:  write string  %s does not match read string %s \n", buf, bufR);
			}
			else
			{
				//		printf("**************Success    test 1 pass*******************************\n ");
			}
		}
	}

	// Log_writeDeadBlock(num,address,address);
}
void test2(int b, struct logAddress address, char *buf)
{
	char buf1[1];
	for (int i = 0; i <= b * BLOCK_NUMBER; i++)
	{
		buf1[0] = 'a' + (i % 26);
		test1(buf1);
	}
	// struct logAddress address;
	// address.segmentNo=2;
	// address.blockNo=32;
	// char *buf="Hello LFS, welcome to CSC 545 OS classf";
	char bufR[50];
	if (!Log_read(address, 50, (void *)bufR))
	{
		printf("return logadress segmentNo= %d  blockNo=%d \n", address.segmentNo, address.blockNo);
		if (strcmp(buf, bufR) != 0)
		{
			printf("Fail:  write string  %s does not match read string %s \n", buf, bufR);
		}
		else
		{
			printf("**************Success    test 1 pass*******************************\n ");
		}
	}
}

int test3()
{

	logAddress oldAdrress, newAdress;
	logAddress address;
	address.segmentNo = 2;
	address.blockNo = 32;
	char buf[50] = "Hello LFS, welcome to CSC 545 OS classf";
	test2(1, address, buf);
	Log_CheckPoint(&oldAdrress, &newAdress, 1, 1);
	// pmetadata->currentsector=128;
	cout << " rewrite metadata " << endl;
}

// cleanning and recovery test
void test4()
{
	logAddress oldAdrress, newAdress;
	logAddress address;
	address.segmentNo = 2;
	address.blockNo = 32;
	char buf[50] = "Hello LFS, welcome to CSC 545 OS classf";
	test2(10, address, buf);
	Log_CheckPoint(&oldAdrress, &newAdress, 1, 1);
	// cleaning test
	int num = 1;
	logAddress oldAddress, newadress;
	int count = 0;
	for (auto it : segmentUsageTable)
	{
		oldAddress.segmentNo = it.first;
		for (int i = 0; i < it.second.blocksByteSize; i++)
		{
			pair<int, int> tmp = it.second.blocksByte[i];
			oldAddress.blockNo = tmp.first;
			if (tmp.first % 10 != 0)
				Log_writeDeadBlock(num, oldAddress, oldAddress);
		}
		count++;
		if (count > 2)
			break;
	}
	//    segmentCache->currenIndex=BLOCK_NUMBER;
	//    Log_Write(num, 1, 50,(void*)buf,&oldAddress);
	//    u_int blocks=100;
	//    startsector=128;
	//    segmentCache->summary->segmentNo=2;
	//    generateBlockNo=31;
	//    segmentCache->currenIndex=0;
	//    address.segmentNo=2;
	// address.blockNo=32;
	char buf1[50] = "Hello LFS, welcome to CSC 545 OS classa";
	test2(1, address, buf1);
	Log_CheckPoint(&oldAdrress, &newAdress, 1, 1);
}

void test5()
{

	char buf[50] = "Destory test";
	logAddress address;
	Log_Write(1, 1, 50, (void *)buf, &address);
	Log_destroy();
}
void test6()
{
	logAddress address;
	address.segmentNo = 1;
	address.blockNo = 1;
	char bufR[50];
	Log_read(address, 50, (void *)bufR);
	cout << "after destory  content " << bufR << endl;
}
//  int main(int argc, char *argv[])
//  {

// 	 printf(" hello log layer \n");
//       init("FuseFileSystem",4);
//  //   test1("test hello world");
// 	   test6();

//    //    test2(3);
//   //    test4();
//     return 1;
// }
