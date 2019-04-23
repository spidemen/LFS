

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


struct SegmentSummary{
	int segmentNo;
    bool  alive=true;
    int   deadblock=0;
  	time_t  modifiedTime;
  	long  age;
    int  totalBlock; 
    int aliveByte=0;
    long costBenefitRate=0.0;
    multimap<inum,int> tables;   // inum associated with block No
    map<int,int> blocksByte;
};


struct metadata{

	int blocksize;
	int segmentsize;
	int segments;
	int limit;
	int currentsector;
	int currentBlockNumber;
	int currentSegmentNumber;
	char filename[FILENAMESIZE];
	map<segmentNo,SegmentSummary> segmentUsageTable;
	map<int, bool> blocksStatus;  // state of each block alive or dead
	map<segmentNo,SegmentSummary> reUsedTable;
	struct logAddress *checkPointRegion;
	time_t  checkPiontTime;
	int checkPointsize;
};

multimap<inum,int> tables;  
map<int,int> blocksByte;
map<int, bool> blocksStatus;
map<segmentNo,SegmentSummary> segmentUsageTable;

char *filename="FuseFileSystem";
int blocksize=4;
int generateBlockNo=0;
int startsector;
int  cache=4;
int  cleanSegment=0;
struct Segment *segmentCache=NULL;
struct Segment MRA[N];
struct metadata *pmetadata=NULL;

bool DoesFileExist (char *filename) {
	FILE *fp=fopen(filename, "r");
    if (fp!=NULL) {
        fclose(fp);
        return true;
    } else {
        return false;
    }   
}

 int init(char *fileSystemName,int cachesize=4){

    cout<<"C++ compiler test  init function "<<endl;
       // init data struct 
	segmentCache=(struct Segment*)malloc(sizeof(struct Segment));
	pmetadata=(struct metadata*)malloc(sizeof(struct metadata));
	segmentCache->summary=(struct SegmentSummary*)malloc(sizeof(struct SegmentSummary));
//	segmentCache->summary=new SegmentSummary;

	for(int i=0;i<N;i++){
		MRA[i].used=false;
	}
//	 if(!DoesFileExist(fileSystemName)){    // comment for testing file layer
	// 	 string cmd="";
	 	 char cmd[50];
	 	 memset(cmd,0,50);
	 	 strcat(cmd,"./mklfs -b 2 -l 32 ");
	 	 strcat(cmd,fileSystemName);
	 	// cmd=cmd+"./mklfs -b 4 -l 4 "+fileSystemName;
	 	// system(cmd.c_str()); 
	 	 system(cmd); 
 //   }		
 								// commnet for testing file layer
	u_int blocks;
    Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
    cache=cachesize;
    if(f!=NULL){
        char buf[SUPERBLOCK*FLASH_SECTOR_SIZE];
        if(Flash_Read(f,0,SUPERBLOCK,buf)){
          //  cout<<"Init Error Cannot read first two sector of file system"<<endl;
            printf("Init Error Cannot read first two sector of file system \n");
            return 0;
        }else{
            pmetadata= (struct metadata *)buf;
         //   memcpy(pmetadata,buf,SUPERBLOCK*FLASH_SECTOR_SIZE);
            segmentCache->summary->totalBlock=pmetadata->segmentsize;
            blocksize=pmetadata->blocksize;
      //      startsector=segmentCache->summary->totalBlock*2*blocksize;  // first segment hold metafile system, second segment hold ifile data
            startsector=pmetadata->currentsector;
            #define BLOCK_SIZE (FLASH_SECTOR_SIZE*blocksize)
            #define BLOCK_NUMBER (segmentCache->summary->totalBlock-1)
            #define N  cache 
 
			segmentCache->dataB=(struct Block*)malloc(sizeof(struct Block)*BLOCK_NUMBER);

			segmentCache->summary->segmentNo=pmetadata->currentSegmentNumber;
			segmentCache->currenIndex=0;
			generateBlockNo=pmetadata->currentBlockNumber;

	//		pmetadata->blocksStatus[1]=true;
	//	    pmetadata->blocksStatus.insert(pair<int,bool>(1,true));
            // segmentCache->head=(struct lData*)malloc(sizeof(struct lData));
            // segmentCache->head->next=NULL;
            printf("init blocksize per sector %d   segment size (blocks) %d  startsector =%d \n",blocksize,pmetadata->segmentsize,startsector);
            printf(" BLOCKSIXE= %d   BLOCKNUMBER=%d\n",BLOCK_SIZE,BLOCK_NUMBER);
        }
    }
	 return 1;
}

int Log_writeDeadBlock(inum num,struct logAddress oldAddress,struct logAddress newAddress){
	  // 
	  pmetadata->segmentUsageTable[oldAddress.segmentNo].deadblock++;
	//  pmetadata->segmentUsageTable[oldAddress.segmentNo].aliveByte-=pmetadata->segmentUsageTable[oldAddress.segmentNo].blocksByte[oldAddress.blockNo];
	//  pmetadata->blocksStatus[oldAddress.blockNo]=false;
	  long u=1-(pmetadata->segmentUsageTable[oldAddress.segmentNo].deadblock/pmetadata->segmentUsageTable[oldAddress.segmentNo].totalBlock);
	  pmetadata->segmentUsageTable[oldAddress.segmentNo].costBenefitRate=((1-u)*pmetadata->segmentUsageTable[oldAddress.segmentNo].age)/(1+u);
	  // check segment dead or not
	  if(pmetadata->segmentUsageTable[oldAddress.segmentNo].aliveByte==0|| 
	  	pmetadata->segmentUsageTable[oldAddress.segmentNo].deadblock==pmetadata->segmentUsageTable[oldAddress.segmentNo].totalBlock){
	  	pmetadata->segmentUsageTable[oldAddress.segmentNo].alive=false;
	    pmetadata->reUsedTable.insert(pair<segmentNo,SegmentSummary>(oldAddress.segmentNo,pmetadata->segmentUsageTable[oldAddress.segmentNo]));
	    cleanSegment++;
	    // cleaning
	    if(cleanSegment==THREADSHOLD){
	       int  segmentStartSector=(oldAddress.segmentNo+1)*segmentCache->summary->totalBlock*blocksize;
	       int startblock=segmentStartSector/FLASH_SECTORS_PER_BLOCK;
	       int  totalErase=segmentCache->summary->totalBlock/FLASH_SECTORS_PER_BLOCK;
	       if(segmentStartSector%FLASH_SECTORS_PER_BLOCK!=0){  // default setting make block size or segment  modual by FLASH_SECTORS_PER_BLOCK
	       	// hard to implement, would waster a lot of space
	       }
	       u_int blocks;
	       Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
	       if(f!=NULL){
	      		if(Flash_Erase(f,startblock,totalErase))
	      		   { printf("cannot erase startblock(flash ) %d  totalBlock=%d \n",startblock,totalErase); }
	  		} else{
	  			printf("Flash Open Error \n");
	  		}

	    }
	  }

	  return 1;
}

// checkpoint 

 int Log_recordIfile(struct logAddress *oldAdrress,struct logAddress *newAdress, int  oldSize, int newSize){
 		  pmetadata->checkPointRegion=(struct logAddress*)malloc(sizeof(struct logAddress)*(newSize+1));
 		    // store ifile
 		  memcpy(pmetadata->checkPointRegion,newAdress,sizeof(struct logAddress)*newSize);
 		  // update checkpoint time
 	      pmetadata->checkPiontTime=time(NULL);
 	      pmetadata->checkPointsize=newSize+1;

 	     int  totalErase=segmentCache->summary->totalBlock/FLASH_SECTORS_PER_BLOCK;
 	     u_int blocks;
	     Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
	     if(f!=NULL){
 	    	 if(Flash_Erase(f,0,totalErase))
 	    	   { printf("cannot erase block\n"); } 
	 	    	if(Flash_Write(f, 0, segmentCache->summary->totalBlock, (void*)pmetadata)){
	                cout<<"Error: cannot write metadata to the flash "<<endl;
	                return 1;
	            }else{
	                cout<<"Success checkpoint time "<<time(NULL)<<endl;
	                return 0;
	            }

 	 	  } else{
 	 	  	printf("Flash Open Error \n");
 	 	  	return 1;
 	 	  }

 }

int Log_Write(inum num, u_int block, u_int length, void *buffer, struct logAddress *logAddress1){
		u_int blocks;
		
	    Flash f=Flash_Open(filename,FLASH_ASYNC,&blocks);
	 //   struct SegmentSummary *summary=(struct SegmentSummary*)malloc(sizeof(struct SegmentSummary));
	    SegmentSummary *summary=new SegmentSummary;
		if(segmentCache->currenIndex>0&&segmentCache->currenIndex%BLOCK_NUMBER==0){  // segment full, write the disk 
			  
			   if(f!=NULL){
			   	    int totalSector=blocksize;
			   	    segmentCache->summary->modifiedTime=time(NULL);
    		    	memcpy(summary,segmentCache->summary,sizeof(struct SegmentSummary));
    		    	summary->tables.clear();
    				summary->tables.insert(tables.begin(),tables.end());
    				tables.clear();
    				summary->blocksByte.clear();
    				summary->blocksByte.insert(blocksByte.begin(),blocksByte.end());
    				blocksByte.clear();
			   		if(Flash_Write(f, startsector,totalSector, (void*)summary)) {    // write segment summary into disk
			   			printf("LogWrite  Error: canont write segment summary inum= %d   fileblock  =%d  startsector=%d \n",num,block,startsector);
			   			return 1;
			   		}else{
			   		    // write segment into segment usagetable
			   		     segmentUsageTable.insert(pair<segmentNo,SegmentSummary>(segmentCache->summary->segmentNo,*summary));
			   		   //  pmetadata->segmentUsageTable.insert(pair<segmentNo,SegmentSummary>(segmentCache->summary->segmentNo,*summary));
			   	
			   		  // write data to disk
			   		   startsector=startsector+totalSector;
			   		   struct Block *tmp=(struct Block*)malloc(sizeof(struct Block)*BLOCK_NUMBER);
			   		   memcpy(tmp,segmentCache->dataB,sizeof(struct Block)*BLOCK_NUMBER);
			   		   totalSector=(segmentCache->summary->totalBlock-1)*blocksize;
			   		    
			   		    if(Flash_Write(f, startsector,totalSector, (void*)tmp)) {     // write segment data into disk
			   		    	printf("LogWrite  Error: canont segment data inum= %d  fileblock  =%d  startsector=%d \n",num,block,startsector);
			   				return 1;	
			   		    }else{

			   		  		startsector=startsector+totalSector; 
			   		  		// implement N most recently used access segment
			   		    	struct Segment  p1;
			   		    	p1.dataB=(struct Block*)malloc(sizeof(struct Block)*BLOCK_NUMBER);
			   				memcpy(p1.dataB,segmentCache->dataB,sizeof(struct Block)*BLOCK_NUMBER);
			   				p1.summary=(struct SegmentSummary*)malloc(sizeof(struct SegmentSummary));
			   				memcpy(p1.summary,segmentCache->summary,sizeof(struct SegmentSummary));
			   				p1.used=true;
			   				if(MRA[N-1].used){
			   					 for(int i=1;i<N;i++){
			   					 	 MRA[i-1]=MRA[i];
			   					 }
			   					 MRA[N-1]=p1;
			   				}else{
			   					for(int i=0;i<N;i++){
			   						if(MRA[i].used) continue;
			   						MRA[i]=p1;
			   						break;
			   					}
			   				}

			   			// reset segment data
			   		        segmentCache->summary->segmentNo++;   // empty segment , increase segment number for next write
			   		        memset(segmentCache->dataB,0,sizeof(struct Block)*BLOCK_NUMBER);
			   		        segmentCache->summary->tables.clear();
			   		        segmentCache->summary->aliveByte=0;
			   		        segmentCache->summary->blocksByte.clear();
							cout<<"Sector point: Success write flash. startsector "<<startsector<<" totalSector="<<totalSector<<endl;
			   		    }
			   		}
			   	  Flash_Close(f);

			   }else{
			  // 	 cout<<"Error: Fail to open log file  "<<filename<<endl;
			   	 printf("Error: Fail to open log file  =%s \n",filename);
      	 		 return 1;	
			   }
		}

		// write data to memory
		int tmp=length;
		 if(tmp>0){
	      		    generateBlockNo++;
					int blockIndex=segmentCache->currenIndex%BLOCK_NUMBER;
					segmentCache->dataB[blockIndex].data=(char *)malloc(BLOCK_SIZE*sizeof(char));
					memcpy(segmentCache->dataB[blockIndex].data,buffer,BLOCK_SIZE);
					segmentCache->dataB[blockIndex].blockNo=generateBlockNo;
					segmentCache->dataB[blockIndex].datasize=length;
					segmentCache->dataB[blockIndex].modifiedTime=time(NULL);
					segmentCache->summary->aliveByte+=length;
					segmentCache->summary->age=(long)time(NULL);
			//		segmentCache->summary->tables.insert(pair<inum,int>(num,generateBlockNo));
		//			segmentCache->summary->blocksByte.insert(pair<int,int>(generateBlockNo,length));
					tables.insert(pair<inum,int>(num,generateBlockNo));
					blocksByte.insert(pair<int,int>(generateBlockNo,length));
		     		blocksStatus.insert(pair<int,bool>(generateBlockNo,true));
			//		pmetadata->blocksStatus=blocksStatus;
			//	    printf("blockNumber=%d  index=%d  currentindex=%d\n", segmentCache->dataB[blockIndex].blockNo,blockIndex,segmentCache->currenIndex);
					segmentCache->currenIndex++;
      		    
		}
		logAddress1->segmentNo=segmentCache->summary->segmentNo;
		logAddress1->blockNo=generateBlockNo;

		printf(" logadress segmentNo= %d  blockNo=%d  blockIndex=%d \n",logAddress1->segmentNo,logAddress1->blockNo,segmentCache->currenIndex-1);
		return 0;

		
}
int Log_read(struct logAddress logAddress1, u_int length, void * buffer){
    
      if(segmentCache->summary->segmentNo==logAddress1.segmentNo){    // check current segment cache
      	    int i;
      	    for(i=0;i<segmentCache->summary->totalBlock;i++){
      	 //   	 printf("test blockNo %d i=%d\n ",segmentCache->dataB[i].blockNo,i);
      	    	 if(segmentCache->dataB[i].blockNo==logAddress1.blockNo) break;
      	    }
      		if(length<=BLOCK_SIZE){
      		 	memcpy(buffer,segmentCache->dataB[i].data,length);
      		// 	printf("i= %d  content =%s \n", i,segmentCache->dataB[i].data);
      		
      		 }else{
      		 	memcpy(buffer,&segmentCache->dataB[i].data,BLOCK_SIZE);
      		 }
      	    return 0;
      }
      int index=0;
      for(index=0;index<N;index++){
      	//	printf("index=%d \n",index);
      	    if(MRA[index].used&&MRA[index].summary->segmentNo==logAddress1.segmentNo) break;
      }
      if(index<=N-1){
      		   struct Segment p=MRA[index];

      		    char *block=NULL;
      		    int i;
      		 	for(i=0;i<N;i++){
      		 		if(p.dataB[i].blockNo==logAddress1.blockNo) {
      		 			block=p.dataB[i].data;
      		 			break;
      		 		}
      		 	}
      		 	if(i<=N-1){
      		
      		 		printf("find segment %d in the queue \n",logAddress1.segmentNo);
      		 		if(length<=BLOCK_SIZE){
      		 		//	  memcpy(buffer,block->second.data,length);
      		 			  memcpy(buffer,block,length);
      		 		} else{
      		 		//	 memcpy(buffer,block->second.data,BLOCK_SIZE);
      		 			 memcpy(buffer,block,BLOCK_SIZE);
      		 		}
      		 	}else{
      		 	    printf("Error: segment No=  =%d   do not have blockNo  =%d \n",logAddress1.segmentNo, logAddress1.blockNo);
      		 		return 1;
      		 	}
      		 for(i=index;i<N-1;i++){
      		 	MRA[i]=MRA[i+1];
      		 	if(!MRA[i].used) break;
      		 }
      		 if(!MRA[i].used) MRA[i]=p;
      		 else if(i==N-2){
      		 	MRA[i+1]=p;
      		 }

      } else{	// read from flash 
      		int  StartSector=(logAddress1.segmentNo)*segmentCache->summary->totalBlock*blocksize;
      	//	int  StartSector=(logAddress1.segmentNo-1)*blocksize;
            int  TotalSector=(segmentCache->summary->totalBlock-1)*blocksize;
      		u_int blocks;
      		char buf[BLOCK_SIZE];
      	    printf("read from disk : startsector =%d  total sector  =%d  canont read  segmentNo= %d  blockNo =%d \n",StartSector,TotalSector,logAddress1.segmentNo,logAddress1.blockNo);
      		Flash f=Flash_Open(filename,FLASH_ASYNC,&blocks);
	        if(f!=NULL){
	      	   if(Flash_Read(f,StartSector,blocksize,buf)){
	      	   	   printf("LogRead Segment Error: startsector =%d  total sector  =%d  canont read  segmentNo= %d  blockNo =%d \n",StartSector,blocksize,logAddress1.segmentNo,logAddress1.blockNo);
	      	   	   return 1;
	      	   }else{
	      	   	    struct  SegmentSummary *summary=(struct SegmentSummary *)buf;
	      	   	    StartSector=StartSector+blocksize;
	      	   	    char buf1[TotalSector*FLASH_SECTOR_SIZE];
	      	   	    if(Flash_Read(f,StartSector,TotalSector,buf1)) {
	      	   	 	  printf("LogRead Segment Error: startsector  =%d  total sector  =%d  canont read   segmentNo= %d  blockNo =%d \n",StartSector,TotalSector,logAddress1.segmentNo,logAddress1.blockNo);
	      	   	 	  return 1;
	      	   	    }else{ 
	      	   	 	  struct  Block *pdata=(struct Block *) buf1;
	      	   	 	   int i;
	      	   	 	for( i=0;i<N;i++){
	      	   	 	//    	 printf(" read from flash i=%d blockNo=%d \n",i,pdata[i].blockNo);	
	      	   	 	    	 if(pdata[i].blockNo==logAddress1.blockNo){
	      	   	 	    	 	break;
	      	   	 	    	 }
      	   				 }
      	   				if(i<=N-1){
			      	   		if(length<=BLOCK_SIZE){
			      	   			printf("i=%d   content  %s\n",i,pdata[i].data );
		      		 			memcpy(buffer,pdata[i].data,length);
		      				 }else{
		      					memcpy(buffer,pdata[i].data,BLOCK_SIZE);
		      		 		 }
	      		 	    }else{
	      		 	       printf(" Error read from disk: segment No= %d  do not have blockNo= %d \n",logAddress1.segmentNo, logAddress1.blockNo);
	      	   	 	       printf("LogRead Segment Error: startsector  =%d  total sector  =%d  segmentNo= %d  blockNo =%d \n",StartSector,TotalSector,logAddress1.segmentNo,logAddress1.blockNo);
	      		 	       return 1;
	      		 	    }
	      		 		struct Segment p;
	      		 		p.summary=(struct SegmentSummary*)malloc(sizeof(struct SegmentSummary));
			   			memcpy(p.summary,summary,sizeof(struct SegmentSummary));
	      		 	//	p.summary=summary;
	      		 		p.dataB=(struct Block*)malloc(sizeof(struct Block)*BLOCK_NUMBER);;
	      		 		memcpy(p.dataB,pdata,sizeof(struct Block)*BLOCK_NUMBER);

	      		 		for(int i=1;i<N;i++){
			   				MRA[i-1]=MRA[i];
			   			}
			   			MRA[N-1]=p;
		      	        return 0;
		      	    }
	      	   }
	      	 Flash_Close(f);
	      }else{
	      	 printf("Error: Fail to open Flash file %s\n", filename);
	      	 return 1;
	      }
	  }

	  return 0;
}
int Log_free(struct logAddress logAddress1,u_int length){
      u_int blocks;
      Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
      if(f!=NULL){
      	    int  StartSector=(logAddress1.segmentNo-1+segmentCache->summary->totalBlock)*blocksize;
      		int startblock=StartSector/FLASH_SECTORS_PER_BLOCK;
      		int blocks=length/FLASH_BLOCK_SIZE+1;
      		if(Flash_Erase(f,startblock,blocks)){
      		//	cout<<"Error: Fail to erase blocks segmentNo ="<<logAddress1.segmentNo<<" blcokNo ="<<logAddress1.blockNo<<endl;
      			printf("Error: Fail to erase blocks segmentNo = %d  blcokNo = %d \n", logAddress1.segmentNo,logAddress1.blockNo );
      			return 1;
      		}else{
      //			printf("Error: Fail to erase blocks segmentNo = %d  blcokNo = %d \n", logAddress1.segmentNo,logAddress1.blockNo );
      			return 0;
      		}
      }else{
      	//	 cout<<"Error: Fail to open log file  "<<filename<<endl;
      		 return 1;
      }
}

void test1(char *cat){
//	printf("*******************Log layer test 1 simple small write and read ****************************** \n");
	char  buf[50]="Hello LFS, welcome to CSC 545 OS class";
	strcat(buf,cat);
//	char  *buf="Hello LFS, welcome to CSC 545 OS class";
	inum num=1;
	struct logAddress address;
	if(!Log_Write(num, 1, 40,(void*)buf,&address)){
		char bufR[40];
		if(!Log_read(address, 40,(void *)bufR)){
			printf("return logadress segmentNo= %d  blockNo=%d \n",address.segmentNo,address.blockNo);
			if(strcmp(buf,bufR)!=0){
				printf("Fail:  write string  %s does not match read string %s \n",buf,bufR);
			}else{
		//		printf("**************Success    test 1 pass*******************************\n ");
			}
		}
	}
}
void test2(int b){
	char buf1[1];
	for(int i=0;i<=b*BLOCK_NUMBER;i++){
		buf1[0]='a'+i;
		test1(buf1);
	}
	struct logAddress address;
	address.segmentNo=1;
	address.blockNo=3;
	char *buf="Hello LFS, welcome to CSC 545 OS classc";
	char bufR[40];
	if(!Log_read(address, 40,(void *)bufR)){
			printf("return logadress segmentNo= %d  blockNo=%d \n",address.segmentNo,address.blockNo);
			if(strcmp(buf,bufR)!=0){
				printf("Fail:  write string  %s does not match read string %s \n",buf,bufR);
			}else{
				printf("**************Success    test 1 pass*******************************\n ");
			}
	}
}

void test3(){
	char *buf="test write super block ";
	startsector=160;
	int startblock=startsector/FLASH_SECTORS_PER_BLOCK;
	u_int blocks;
	Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
	Flash_Write(f, startsector, 8, (void*)buf);
	Flash_Write(f, startsector+16, 1, (void*)buf);
	char *buf2="test write update super block";
	if(Flash_Erase(f,0,11)){ printf("cannot erase block\n"); }
	if(Flash_Write(f, startsector,8, (void*)buf2)){
		printf("Fail write super blcok  startblcok= %d   FLASH_BLOCK sectors =%d\n",startblock,FLASH_SECTORS_PER_BLOCK);
	}
	Flash_Write(f, startsector+16,1, (void*)buf2);
	char bufRead[2048];
	Flash_Read(f, startsector, 4, bufRead);
	printf("read super block %s\n",bufRead);
	Flash_Read(f, startsector+16, 1, bufRead);
	printf("read super block %s\n",bufRead);
}

// void testWrite(char *buf,int start){
// //	char *filename="test.txt";
// 	u_int blocks=100;
//     Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
//     if(f!=NULL){

//     	cout<<"total block for this file "<<blocks<<" the len of buf ="<<strlen(buf)<<endl;
//     	 segmentCache->summary->segmentNo++; 
//     	 SegmentSummary *summary=new SegmentSummary;
//     	 memcpy(summary,segmentCache->summary,sizeof(SegmentSummary));
//     	 	Block B;
// 			B.blockNo=5;
// 			char *buffer="tstssgagaga";
// 			memcpy(B.data,buf,BLOCK_SIZE);
// 		//	cout<<"write data. "<<B.data<<endl;
// 	//			tmp=tmp-BLOCK_SIZE;
// 		 // Data *pdata=new Data;

// 		 // segmentCache->data.clear();
// 		 // segmentCache->data.insert(pair<int,Block>(5,B));
// 		 // segmentCache->data.insert(pair<int,Block>(6,B));
// 		 // segmentCache->data.insert(pair<int,Block>(7,B));
// 		 // segmentCache->data.insert(pair<int,Block>(8,B));
// 		 //  segmentCache->pdata->data.clear();
// 		 // segmentCache->pdata->data.insert(pair<int,Block>(5,B));
// 		 // segmentCache->pdata->data.insert(pair<int,Block>(6,B));
// 		 // segmentCache->pdata->data.insert(pair<int,Block>(7,B));
// 		 // segmentCache->pdata->data.insert(pair<int,Block>(8,B));
// 		 // pdata->data=segmentCache->pdata->data;
// 	//	 memcpy(pdata,segmentCache->pdata,sizeof(segmentCache->pdata));

//     	  void *buf=(void *)&segmentCache->data;
//     	  segmentCache->head->B=B;
//     	  lData *pcurrent1=(struct lData*)malloc(sizeof(struct lData));
//     	  lData *pcurrent2=(struct lData*)malloc(sizeof(struct lData));
//     	   pcurrent1->B=B;
//     	   pcurrent1->next=pcurrent2;
//     	   pcurrent2->B=B;
//     	   pcurrent2->next=NULL;
//     	   segmentCache->head->next=pcurrent1;
    	  
//     	   lData *pcurrent=segmentCache->head;
//     	    // while(pcurrent!=NULL){
//     	    //    lData *pwrite=(struct lData*)malloc(sizeof(struct lData));
//     	    // 	pwrite->B=pcurrent->B;
//     	    // 	pwrite->blockNo=pcurrent2->blockNo;
//     	    // 	pcurrent=pcurrent->next;
//     	    // 	pwrite=pwrite->next;
//     	    // 	cout<<"test "<<endl;
//     	    // }
//     	    segmentCache->dataB[0]=B;
//     	    segmentCache->dataB[1]=B;
//     	    segmentCache->dataB[2]=B;
//     	    Block *pwrite=segmentCache->dataB;
//     	if(Flash_Write(f, start, 20, (void*)pwrite)){
//     		cout<<"Write Flash Error: canont write "<<buf<<" to flash  total block "<<1<<endl;
//     	}else{
//     		cout<<"Success Write Flash "<<endl;
//     	}
//     	  delete summary;
//     	  Flash_Close(f);
    	
//     }
  
// }


// void testRead(int start){
// //	char *filename="test.txt";
// 	u_int blocks=100;
// 	int size=BLOCK_SIZE*5;
// 	char buf[size];
//     Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
//     if(f!=NULL){
//     	cout<<"total block for this file "<<blocks<<endl;
//     	if(Flash_Read(f, start, 20, (void*)buf)){
//     		cout<<"Read Flash Error: canont read startsector "<<start<<" to flash  total block "<<1<<endl;
//     	}else{
//     		   Block *pdata=(Block *) buf;
//     		   for(int i=0;i<4;i++){
//     		   	   cout<<"blockNo = "<<pdata[i].blockNo<<" content = "<<pdata[i].data<<endl;
//     		   }
    		
//     // 	    Data *pdata=(Data*)buf;
//     // 		map<int,Block> p=pdata->data;
//     // 	//	memcpy(&p,buf,1024);
//     // 		//(map<int,Block> *)buf;
//     // 		auto it=p.find(8);
//     // 	//	cout<<"Success Read  Flash   buf="<<buf<<endl;
//     // 		cout<<"Success Read  Flash   SegmentNo="<<it->second.blockNo<<endl;
//  			//  auto it2=p.begin();
// 			 // int i=0;
// 			 // while(it2!=p.end()&&i<10){
// 		  //       cout<<"block number "<<it2->first<<" block No "<<it2->second.blockNo<<endl;
// 				// it2++; i++;
// 			 // }
//     	}
//     }
//     Flash_Close(f);
// }

 int main(int argc, char *argv[])
  {
	
	 printf(" hello log layer \n");
     init("FuseFileSystem",4);
 //   test1("test hello world");
	//  test3();
      test2(3*N+1);
  //   testWrite("This is the first time to use flash libraray 1 l\n",4);
   //  testWrite("This is the first time to use flash libraray 2 l\n",2);
 //    testWrite("This is the first time to use flash libraray 3 l\n",4);
 //    testWrite("This is the first time to use flash libraray 4 l\n",6);
   //     testRead(4);
  //    TestLogWrite();
	//  delete segmentCache;
    return 1;
}
