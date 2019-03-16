#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <flash.h>
#include "log.h"
static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";
static const char *link_path = "/link";

using namespace std;

Segment *segmentCache=new Segment;
int startsector;
vector< pair<int, Segment > > MRC;   // implement N most access segment policy

vector< pair<int, Segment > > MRCTest;
metadata *pmetadata=new metadata;

void testWrite(char *buf,int start){
//	char *filename="test.txt";
	u_int blocks=100;
    Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
    if(f!=NULL){
    	cout<<"total block for this file "<<blocks<<" the len of buf ="<<strlen(buf)<<endl;
    	 segmentCache->summary->segmentNo++; 
    	 SegmentSummary *summary=new SegmentSummary;
    	 memcpy(summary,segmentCache->summary,sizeof(SegmentSummary));
    	 	Block B;
			B.blockNo=5;
			char *buffer="tstssgagaga";
			memcpy(B.data,buffer,BLOCK_SIZE);
		//	cout<<"write data. "<<B.data<<endl;
	//			tmp=tmp-BLOCK_SIZE;
		 Data *pdata=new Data;

		 // segmentCache->data.clear();
		 // segmentCache->data.insert(pair<int,Block>(5,B));
		 // segmentCache->data.insert(pair<int,Block>(6,B));
		 // segmentCache->data.insert(pair<int,Block>(7,B));
		 // segmentCache->data.insert(pair<int,Block>(8,B));
		  segmentCache->pdata->data.clear();
		 segmentCache->pdata->data.insert(pair<int,Block>(5,B));
		 segmentCache->pdata->data.insert(pair<int,Block>(6,B));
		 segmentCache->pdata->data.insert(pair<int,Block>(7,B));
		 segmentCache->pdata->data.insert(pair<int,Block>(8,B));
		 pdata->data=segmentCache->pdata->data;
	//	 memcpy(pdata,segmentCache->pdata,sizeof(segmentCache->pdata));
    	  void *buf=(void *)&segmentCache->data;
    	if(Flash_Write(f, start, 6, (void*)pdata)){
    		cout<<"Write Flash Error: canont write "<<buf<<" to flash  total block "<<1<<endl;
    	}else{
    		cout<<"Success Write Flash "<<endl;
    	}
    	  delete summary;
    	  Flash_Close(f);
    	
    }
  
}
void testRead(int start){
//	char *filename="test.txt";
	u_int blocks=100;
	char buf[3072];
    Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
    if(f!=NULL){
    	cout<<"total block for this file "<<blocks<<endl;
    	if(Flash_Read(f, start, 6, (void*)buf)){
    		cout<<"Read Flash Error: canont read startsector "<<start<<" to flash  total block "<<1<<endl;
    	}else{
    	    Data *pdata=(Data*)buf;
    		map<int,Block> p=pdata->data;
    	//	memcpy(&p,buf,1024);
    		//(map<int,Block> *)buf;
    		auto it=p.find(8);
    	//	cout<<"Success Read  Flash   buf="<<buf<<endl;
    		cout<<"Success Read  Flash   SegmentNo="<<it->second.blockNo<<endl;
 			 auto it2=p.begin();
			 int i=0;
			 while(it2!=p.end()&&i<10){
		        cout<<"block number "<<it2->first<<" block No "<<it2->second.blockNo<<endl;
				it2++; i++;
			 }
    	}
    }
    Flash_Close(f);
}

bool DoesFileExist (char *filename) {
    if (FILE *file = fopen(filename, "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

int init(char *fileSystemName){
	 if(!DoesFileExist(fileSystemName)){
	 	 string cmd="";
	 	 cmd=cmd+"./mklfs -b 4 -l 4 "+fileSystemName;
	 	 system(cmd.c_str()); 
    }
	u_int blocks;
    Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
    if(f!=NULL){
        char buf[SUPERBLOCK*FLASH_SECTOR_SIZE];
        if(Flash_Read(f,0,SUPERBLOCK,buf)){
            cout<<"Init Error Cannot read first two sector of file system"<<endl;
        }else{
          //  pmetadata= (metadata *)buf;
            memcpy(pmetadata,buf,SUPERBLOCK*FLASH_SECTOR_SIZE);
            segmentCache->summary->totalBlock=pmetadata->segmentsize;
            blocksize=pmetadata->blocksize;
      //      startsector=segmentCache->summary->totalBlock*2*blocksize;  // first segment hold metafile system, second segment hold ifile data
            startsector=pmetadata->currentsector;
            #define BLOCK_SIZE (FLASH_SECTOR_SIZE*blocksize)
            cout<<"  metadata  segment size in unit blocks "<<pmetadata->segmentsize<<endl;
            // cout<<"metadata   total  segments  "<<p->segments<<endl;
            // cout<<"metadata   wearlimit ="<<p->limit<<endl;
        }
    }
	 return 1;
	// startsector=0;

}

int Log_Write(inum num, u_int block, u_int length, void *buffer, logAddress &logAddress1){
		u_int blocks;
	    Flash f=Flash_Open(filename,FLASH_ASYNC,&blocks);
	    Data *pdata=new Data;
	    SegmentSummary *summary=new SegmentSummary;
		if(segmentCache->pdata->data.size()==segmentCache->summary->totalBlock){  // segment full, write the disk 
			  
			   if(f!=NULL){
			   	    int totalSector=blocksize;
			   	//     SegmentSummary *summary=new SegmentSummary;
    				 memcpy(summary,segmentCache->summary,sizeof(SegmentSummary));
			   		if(Flash_Write(f, startsector,totalSector, (void*)summary)) {    // write segment summary into disk
			   			cout<<"LogWrite  Error: canont segment summary inum="<<num<<"   fileblock "<<block<<endl;
			   			return 1;
			   		}else{

			   			cout<<"Segment Summart: Success write flash. startsector "<<startsector<<" totalSector="<<totalSector<<endl;
			   		    startsector=startsector+totalSector;
			   	//	    Data *pdata=new Data;
			   			pdata->data.insert(segmentCache->pdata->data.begin(),segmentCache->pdata->data.end());
			   		    void *buf=(void *)pdata;
			   		  //  void *buf=(void *)&segmentCache->data;
			   		    totalSector=(segmentCache->summary->totalBlock-1)*blocksize;
			   		    if(Flash_Write(f, startsector,totalSector, (void*)buf)) {     // write segment data into disk
			   		    	cout<<"LogWrite  Error: segment data  inum="<<num<<"   fileblock "<<block<<endl;
			   				return 1;	
			   		    }else{

			   		    	cout<<"Segment Data: Success write flash. startsector "<<startsector<<" totalSector="<<totalSector<<endl;
			   		  		startsector=startsector+totalSector; 
			   		    	Segment  p1;
			   				p1.data=segmentCache->data;
			   				p1.summary=segmentCache->summary;
			   				while(MRC.size()>=N){
			   					MRC.erase(MRC.begin());
			   					cout<<"log write remove front segment "<<endl;
			   				}
			   		      	MRC.push_back(pair<int,Segment>(segmentCache->summary->segmentNo,p1));
			   		 //      	// write segment summay into metadata
			   		 //    	Flash_Close(f);
			   		 //      	Flash f=Flash_Open(filename,FLASH_ASYNC,&blocks);
			   		 //      	SegmentSummary psummary;
			   		 // //     	memcpy(&psummary,summary,sizeof(SegmentSummary));
			   		 //      	pmetadata->currentsector=startsector;
			   		 //     // pmetadata->segmentUsageTable.insert(pair<int,SegmentSummary>(segmentCache->summary->segmentNo,psummary));
			   		 //      	if(Flash_Write(f, 0, 2, (void*)pmetadata)){
				     //            cout<<"Error: cannot write metadata to the flash "<<endl;
				     //            return 1;
				     //        }
				     //        Flash_Close(f);
			   		        segmentCache->summary->segmentNo++;   // empty segment , increase segment number for next write
			   		        segmentCache->summary->modifiedTime=time(NULL);
			   	            segmentCache->data.clear();
			   		        segmentCache->pdata->data.clear();
			   		        segmentCache->summary->tables.clear();
			   		     //     char buf1[3072];
			   		    	// if(!Flash_Read(f,tmp,totalSector,buf1)){
			   		    	//     Data *pdata1=(Data *)buf1;
			   		    	//     map<int,Block> sData=pdata1->data;
				      	  //  	    auto it2=sData.begin();
				      	  //  	    int i=0;
				      	  //  	    while(it2!=sData.end()&&i<10){
				      	  //  	    	cout<<"block number "<<it2->first<<" block No "<<it2->second.blockNo<<endl;
				      	  //  	    	it2++; i++;
				      	  //  	    }
				      	  //  	     auto it=sData.find(5);
				      	  //  	     if(it!=sData.end())
				      	  //  	     cout<<"Success find 2 Flash   blockNo="<<it->second.blockNo<<endl;
			   		    	// }else{
			   		    	// 	cout<<"cannot do log read test "<<endl;
			   		    	// }
			   		        //  delete pdata,summary;
			   		    }
			   		}
			   	//	delete summary;
			   }else{
			   	 cout<<"Error: Fail to open log file  "<<filename<<endl;
      	 		 return 1;	
			   }
		}
		
		// write data to memory
		int tmp=length;
		 if(tmp>0){
				generateBlockNo++;
				Block B;
				B.blockNo=generateBlockNo;
				memcpy(B.data,buffer,BLOCK_SIZE);
				cout<<"write data. "<<B.data<<"  "<<BLOCK_SIZE<<endl;
				tmp=tmp-BLOCK_SIZE;
			//	segmentCache->data.insert(pair<int,Block>(generateBlockNo,B));
				segmentCache->pdata->data.insert(pair<int,Block>(generateBlockNo,B));
				segmentCache->summary->tables.insert(pair<inum,int>(num,generateBlockNo));
		}
		logAddress1.segmentNo=segmentCache->summary->segmentNo;
		logAddress1.blockNo=generateBlockNo;
		Flash_Close(f);
 //		delete pdata,summary;
		return 0;

		
}
int Log_read(logAddress logAddress1, u_int length, void * buffer){
    
      if(segmentCache->summary->segmentNo==logAddress1.segmentNo){    // check current segment cache
      		if(length<=BLOCK_SIZE){
      		 	memcpy(buffer,segmentCache->data[logAddress1.blockNo].data,length);
      		 //	cout<<"segment cache  "<<(char*)buffer<<endl;
      		 }else{
      		 	memcpy(buffer,segmentCache->data[logAddress1.blockNo].data,BLOCK_SIZE);
      		 }
      	    return 0;
      }
      vector<pair<int,Segment> >::iterator it=MRC.begin();
  //  find(MRC.begin(),MRC.end(),logAddress1.SegmentNo);
      while(it!=MRC.end()){				// check Segment queue
      	 if(it->first==logAddress1.segmentNo) break;
      	 it++;
      }
      if(it!=MRC.end()){
      		 Segment p=it->second;
      		 if(length<=BLOCK_SIZE){
      		 	auto block=p.pdata->data.find(logAddress1.blockNo);
      		 	if(block!=p.pdata->data.end()){
      		 	//	cout<<"find  segment "<<logAddress1.segmentNo<<" in the queue "<<endl;
      		 		memcpy(buffer,block->second.data,length);
      		 	}else{
      		 		cout<<"Error: segment No= "<<logAddress1.segmentNo<<" do not have blockNo "<<logAddress1.blockNo<<endl;
      		 		return 1;
      		 	}
      		 }else{
      		 	memcpy(buffer,p.data[logAddress1.blockNo].data,BLOCK_SIZE);
      		 }
      		 MRC.erase(it);
      		 MRC.push_back(pair<int,Segment>(logAddress1.segmentNo,p));

      } else{
      		int  StartSector=(logAddress1.segmentNo-1+segmentCache->summary->totalBlock*2)*blocksize;
      	//	int  StartSector=(logAddress1.segmentNo-1)*blocksize;
            int  TotalSector=(segmentCache->summary->totalBlock-1)*blocksize;
      		u_int blocks;
      		char buf[BLOCK_SIZE];
      		Flash f=Flash_Open(filename,FLASH_ASYNC,&blocks);
	        if(f!=NULL){
	      	   if(Flash_Read(f,StartSector,blocksize,buf)){
	      	   	   cout<<"LogRead Segment Error: startsector ="<<StartSector<<"  total sector "<<blocksize
	      	   	   <<"canont read  segmentNo= "<<logAddress1.segmentNo<<" blockNo ="<<logAddress1.blockNo<<endl;
	      	   	   return 1;
	      	   }else{
	      	   	    SegmentSummary *summary=(SegmentSummary *)buf;
	      	   	    StartSector=StartSector+blocksize;
	      	   	    char buf1[TotalSector*FLASH_SECTOR_SIZE];
	      	   	    if(Flash_Read(f,StartSector,TotalSector,buf1)) {
	      	   	    	cout<<"LogRead Segment Error: startsector ="<<StartSector<<"  total sector "<<TotalSector
	      	   	 	  <<"canont read  segmentNo= "<<logAddress1.segmentNo<<" blockNo ="<<logAddress1.blockNo<<endl;
	      	   	 	  return 1;
	      	   	    }else{
	      	   	 	    Data *pdata=(Data*)buf1;
		      	   	    map<int,Block> sData=pdata->data;
		      	   	    auto it=sData.find(logAddress1.blockNo);
		      	   	    if(it!=sData.end()){
			      	   		if(length<=BLOCK_SIZE){
		      		 			memcpy(buffer,it->second.data,length);
		      				 }else{
		      					memcpy(buffer,it->second.data,BLOCK_SIZE);
		      		 		 }
	      		 	    }else{
	      		 	    	cout<<"Error read from disk: segment No= "<<logAddress1.segmentNo<<" do not have blockNo "<<logAddress1.blockNo<<endl;
	      		 	    	cout<<"LogRead Segment startsector ="<<StartSector<<"  total sector "<<TotalSector
	      	   	 	        <<"  segmentNo= "<<logAddress1.segmentNo<<" blockNo ="<<logAddress1.blockNo<<endl;
	      		 	    	return 1;
	      		 	    }
	      		 		Segment p;
	      		 		p.summary=summary;
	      		 		p.pdata=pdata;
		      	   		while(MRC.size()>=N){
				   			MRC.erase(MRC.begin());
				   		}
		      	   		MRC.push_back(pair<int,Segment>(logAddress1.segmentNo,p));
		      	        cout<<"Log: Success Read  data  segmentNo"<<logAddress1.segmentNo<<" blockNO ="<<logAddress1.blockNo<<endl;
		      	    //    delete summary,pSegment;
		      	        return 0;
		      	    }
	      	   }
	      	 Flash_Close(f);
	      }else{
	      	 cout<<"Error: Fail to open Flash file  "<<filename<<endl;
	      	 return 1;
	      }
	  }

	  return 0;
}
int Log_free(logAddress logAddress1,u_int length){
      u_int blocks;
      Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
      if(f!=NULL){
      		int startblock=logAddress1.blockNo;
      		int blocks=length/FLASH_BLOCK_SIZE+1;
      		if(Flash_Erase(f,startblock,blocks)){
      			cout<<"Error: Fail to erase blocks segmentNo ="<<logAddress1.segmentNo<<" blcokNo ="<<logAddress1.blockNo<<endl;
      			return 1;
      		}else{
      			cout<<"Success earse blocks  "<<logAddress1.segmentNo<<" blcokNo ="<<logAddress1.blockNo<<endl;
      			return 0;
      		}
      }else{
      		 cout<<"Error: Fail to open log file  "<<filename<<endl;
      		 return 1;
      }
}

void TestLogWrite(){
	 string buf="test log write and read: hello LFS";
	 logAddress address;
	  int  flag;
	 for(int i=0;i<21;i++){
	 	 buf="test log write and read: hello LFS th "+to_string(i)+"ith write";
	 	 const char *bufwrite=buf.c_str();
		  flag=Log_Write(1, 2, 50, (void*)bufwrite, address);
		 if(flag) {
		 	cout<<"Error: Fail Pass log write test"<<endl;
		 }else{
		 	cout<<"Success write a log   segmentNo="<<address.segmentNo<<" blockNO=. "<<address.blockNo<<endl;
		 }
	}
//	 testRead(2);
	 char readbuf[50];
	 address.segmentNo=1;
	 address.blockNo=3;
	 flag=Log_read(address, 50, (void*)readbuf);
	 if(flag){
	 	cout<<"Error: Fail pass log read test "<<endl;
	 }
	 cout<<"read from flash. segmentNo="<<address.segmentNo<<"  "<<readbuf<<endl;
	 flag=Log_read(address, 50, (void*)readbuf);
	 cout<<"2nd read from flash. segmentNo="<<address.segmentNo<<"  "<<readbuf<<endl;
}
int main(int argc, char *argv[])
{
	 cout<<"hell World"<<endl;
     init("FuseFileSystem");
   //  testWrite("This is the first time to use flash libraray 1 l\n",2);
   //  testWrite("This is the first time to use flash libraray 2 l\n",2);
 //    testWrite("This is the first time to use flash libraray 3 l\n",4);
 //    testWrite("This is the first time to use flash libraray 4 l\n",6);
 //    testRead(0);
      TestLogWrite();
	//  delete segmentCache;
    return 1;
}
