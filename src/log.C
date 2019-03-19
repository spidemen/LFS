
#include "log.h"

using namespace std;

char *filename="FuseFileSystem";
Segment *segmentCache=new Segment;
int startsector;
vector< pair<int, Segment > > MRC;   // implement N most access segment policy
int generateBlockNo=0;
metadata *pmetadata=new metadata;

int blocksize=2;

bool DoesFileExist (char *filename) {
    if (FILE *file = fopen(filename, "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

int init(char *fileSystemName){
//	 if(!DoesFileExist(fileSystemName)){    // comment for testing file layer
	 	 string cmd="";
	 	 cmd=cmd+"./mklfs -b 4 -l 4 "+fileSystemName;
	 	 system(cmd.c_str()); 
 //   }										// commnet for testing file layer
	u_int blocks;
    Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
    if(f!=NULL){
        char buf[SUPERBLOCK*FLASH_SECTOR_SIZE];
        if(Flash_Read(f,0,SUPERBLOCK,buf)){
            cout<<"Init Error Cannot read first two sector of file system"<<endl;
        }else{
            pmetadata= (metadata *)buf;
          //  memcpy(pmetadata,buf,SUPERBLOCK*FLASH_SECTOR_SIZE);
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

			   		//	cout<<"Segment Summart: Success write flash. startsector "<<startsector<<" totalSector="<<totalSector<<endl;
			   		    startsector=startsector+totalSector;
			   	//	    Data *pdata=new Data;
			   			pdata->data.insert(segmentCache->pdata->data.begin(),segmentCache->pdata->data.end());
			   		    void *buf=(void *)pdata;
			   		    totalSector=(segmentCache->summary->totalBlock-1)*blocksize;
			   		    if(Flash_Write(f, startsector,totalSector, (void*)buf)) {     // write segment data into disk
			   		    	cout<<"LogWrite  Error: segment data  inum="<<num<<"   fileblock "<<block<<endl;
			   				return 1;	
			   		    }else{

			   		  //  	cout<<"Segment Data: Success write flash. startsector "<<startsector<<" totalSector="<<totalSector<<endl;
			   		  		startsector=startsector+totalSector; 
			   		    	Segment  p1;
			   				p1.data=segmentCache->data;
			   				p1.summary=segmentCache->summary;
			   				while(MRC.size()>=N){
			   					MRC.erase(MRC.begin());
			   					cout<<"log write remove front segment "<<endl;
			   				}
			   		      	MRC.push_back(pair<int,Segment>(segmentCache->summary->segmentNo,p1));
			   		        segmentCache->summary->segmentNo++;   // empty segment , increase segment number for next write
			   		        segmentCache->summary->modifiedTime=time(NULL);
			   	            segmentCache->data.clear();
			   		        segmentCache->pdata->data.clear();
			   		        segmentCache->summary->tables.clear();
			   		        //  delete pdata,summary;
			   		    }
			   		}
			   	//	delete summary;
			   	  Flash_Close(f);

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
			//	cout<<"write data. "<<B.data<<"  "<<BLOCK_SIZE<<endl;
				tmp=tmp-BLOCK_SIZE;
			//	segmentCache->data.insert(pair<int,Block>(generateBlockNo,B));
				segmentCache->pdata->data.insert(pair<int,Block>(generateBlockNo,B));
				segmentCache->summary->tables.insert(pair<inum,int>(num,generateBlockNo));
		}
		logAddress1.segmentNo=segmentCache->summary->segmentNo;
		logAddress1.blockNo=generateBlockNo;
		
 //		delete pdata,summary;
		return 0;

		
}
int Log_read(logAddress logAddress1, u_int length, void * buffer){
    
      if(segmentCache->summary->segmentNo==logAddress1.segmentNo){    // check current segment cache
         	auto block=segmentCache->pdata->data.find(logAddress1.blockNo);
      		if(length<=BLOCK_SIZE){
      		 	memcpy(buffer,block->second.data,length);
      		//   	cout<<"segment cache  "<<(char*)buffer<<"   "<<BLOCK_SIZE<<"  "<<length<<endl;
      		 }else{
      		 	memcpy(buffer,block->second.data,BLOCK_SIZE);
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
      		 	auto block=p.pdata->data.find(logAddress1.blockNo);
      		 	if(block!=p.pdata->data.end()){
      		 	//	cout<<"find  segment "<<logAddress1.segmentNo<<" in the queue "<<endl;
      		 		if(length<=BLOCK_SIZE){
      		 			  memcpy(buffer,block->second.data,length);
      		 		} else{
      		 			 memcpy(buffer,block->second.data,BLOCK_SIZE);
      		 		}
      		 	}else{
      		 		cout<<"Error: segment No= "<<logAddress1.segmentNo<<" do not have blockNo "<<logAddress1.blockNo<<endl;
      		 		return 1;
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
		      	   //     cout<<"Log: Success Read  data  segmentNo"<<logAddress1.segmentNo<<" blockNO ="<<logAddress1.blockNo<<endl;
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
      	    int  StartSector=(logAddress1.segmentNo-1+segmentCache->summary->totalBlock*2)*blocksize;
      		int startblock=StartSector/FLASH_SECTORS_PER_BLOCK;
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

// int main(int argc, char *argv[])
// {
// 	 cout<<"hell World"<<endl;
//      init("FuseFileSystem");
//    //  testWrite("This is the first time to use flash libraray 1 l\n",2);
//    //  testWrite("This is the first time to use flash libraray 2 l\n",2);
//  //    testWrite("This is the first time to use flash libraray 3 l\n",4);
//  //    testWrite("This is the first time to use flash libraray 4 l\n",6);
//  //    testRead(0);
//       TestLogWrite();
// 	//  delete segmentCache;
//     return 1;
// }
