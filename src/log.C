#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <flash.h>
#include "log.h"
static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";
static const char *link_path = "/link";

using namespace std;

void createTest(){
	 char *filename="test.txt";
	 int wearlimit=500;
	 int blocks=300;
	 int flag=Flash_Create(filename, wearlimit,blocks);
	 if(flag) {
	 	cout<<"Error: create a flash   filename="<<filename<<endl;
	 }else{
	 	cout<<"Success: create flash  filename  "<<filename<<endl;
	 }

}
void testWrite(char *buf){
	char *filename="test.txt";
	u_int blocks=100;
    Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
    if(f!=NULL){
    	cout<<"total block for this file "<<blocks<<" the len of buf ="<<strlen(buf)<<endl;
    	if(Flash_Write(f, 0, 1, (void*)buf)){
    		cout<<"Write Flash Error: canont write "<<buf<<" to flash  total block "<<1<<endl;
    	}else{
    		cout<<"Success Write Flash "<<endl;
    	}
    }
    Flash_Close(f);
}
void testRead(){
	char *filename="test.txt";
	u_int blocks=100;
	char *buf;
    Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
    if(f!=NULL){
    	cout<<"total block for this file "<<blocks<<endl;
    	if(Flash_Read(f, 0, 1, (void*)buf)){
    		cout<<"Read Flash Error: canont read "<<buf<<" to flash  total block "<<1<<endl;
    	}else{
    		cout<<"Success Read  Flash   buf="<<buf<<endl;

    	}
    }
    Flash_Close(f);
}

// struct Segment{
// 	int segmentNo;
//     bool  inUse=false;
//     int   liveByte;
// 	time_t   modifiedTime=time(NULL);
//     int  totalBlock; 
//     vector<Block> block;      
// };

int Log_Write(inum num, u_int block, u_int length, void *buffer, logAddress &logAddress1){
	// int len=MRC.size();
	// if(find(MRC.begin(),MRC.end(),num)==MRC.end()){
	// 	 if(len<N){
	// 	 	 Segment *s=new Segment;
	// 	 	 generateSegmentNo++;
	// 	 	 s->segmentNo=generateSegmentNo;
	// 	 	 s->inUse=true;
	// 	 	 s->modifiedTime=time(NULL);
	// 	 	 s->totalBlock=4;
		 	 

	// 	 }

	// }
	return 1;
}
int Log_read(logAddress logAddress1, u_int length, void * buffer){
      char *filename="test.txt";
      u_int blocks;
      Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
      if(f!=NULL){
      	   int offset=logAddress1.blockNo*FLASH_SECTORS_PER_BLOCK;
      	   int blocks=length/FLASH_BLOCK_SIZE+1;
      	   if(Flash_Read(f,offset,blocks,buf)){
      	   	   cout<<"Read Flash Error: canont read   segmentNo= "<<logAddress1.segmentNo<<" blockNo ="<<logAddress1.blockNo<<endl;
      	   	   return 1;
      	   }else{
      	        cout<<"Success Read  data  segmentNo"<<logAddress1.segmentNo<<" blockNO ="<<logAddress1.blockNO<<endl;
      	        return 0;
      	   }
      }else{
      	 cout<<"Error: Fail to open log file  "<<filename<<endl;
      	 return 1;
      }
}
int Log_free(logAddress logAddress1,u_int length){
	  char *filename="test.txt";
      u_int blocks;
      Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
      if(f!=NULL){
      		int startblock=logAddress1.blockNo;
      		int blocks=length/FLASH_BLOCK_SIZE+1;
      		if(Flash_Erase(f,startblock,blocks)){
      			cout<<"Error: Fail to erase blocks segmentNo ="<<logAddress.segmentNo<<" blcokNo ="<<logAddress.blockNo<<endl;
      			return 1;
      		}else{
      			cout<<"Success earse blocks  "<<logAddress.segmentNo<<" blcokNo ="<<logAddress.blockNo<<endl;
      			return 0;
      		}
      }else{
      	 cout<<"Error: Fail to open log file  "<<filename<<endl;
      	 return 1;
      }
}
int main(int argc, char *argv[])
{
	 cout<<"hell World"<<endl;
     createTest();
	 testWrite("This is the first time to use flash libraray l\n");
	testRead();
    return 1;
}
