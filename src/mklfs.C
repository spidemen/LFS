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

// void createTest(){
// 	 char *filename="test.txt";
// 	 int wearlimit=500;
// 	 int blocks=300;
// 	 int flag=Flash_Create(filename, wearlimit,blocks);
// 	 if(flag) {
// 	 	cout<<"Error: create a flash   filename="<<filename<<endl;
// 	 }else{
// 	 	cout<<"Success: create flash  filename  "<<filename<<endl;
// 	 }

// }
// void testWrite(char *buf){
// 	char *filename="test.txt";
// 	u_int blocks=100;
//     Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
//     if(f!=NULL){
//     	cout<<"total block for this file "<<blocks<<" the len of buf ="<<strlen(buf)<<endl;
//     	if(Flash_Write(f, 0, 1, (void*)buf)){
//     		cout<<"Write Flash Error: canont write "<<buf<<" to flash  total block "<<1<<endl;
//     	}else{
//     		cout<<"Success Write Flash "<<endl;
//     	}
//     }
//     Flash_Close(f);
// }
// void testRead(){
// 	char *filename="test.txt";
// 	u_int blocks=100;
// 	char *buf;
//     Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
//     if(f!=NULL){
//     	cout<<"total block for this file "<<blocks<<endl;
//     	if(Flash_Read(f, 0, 1, (void*)buf)){
//     		cout<<"Read Flash Error: canont read "<<buf<<" to flash  total block "<<1<<endl;
//     	}else{
//     		cout<<"Success Read  Flash   buf="<<endl;
//     	}
//     }
//     Flash_Close(f);
// }

// struct metadata{
//     int blocksize;
//     int segmentsize;
//     int segments;
//     unordered_map<int,Segment> segmentTable;
//     int checkpointStart;     // start block of checkpoint
// };

int createMklfs(char * filename,int blocksize=2,int segmentsize=32,int wearlimit=1000,int flashSizeInsegment=100){
      int totalblock=(blocksize*segmentsize*flashSizeInsegment)/16;
      int flag=Flash_Create(filename, wearlimit,totalblock);
      if(flag) {
        cout<<"Error: create a flash   filename="<<filename<<endl;
        return 1;
      }else{
        cout<<"Success: create flash  filename  "<<filename<<endl;
        u_int blocks;
        Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
        if(f!=NULL){
            // init metadata
            metadata *p=new metadata;
            p->blocksize=blocksize;
            p->segmentsize=segmentsize;
            p->segments=flashSizeInsegment;
            p->checkpointStart=1;
            if(Flash_Write(f, 0, 1, (void*)p)){
                cout<<"Error: cannot write metadat to the flash "<<endl;
                return 1;
            }else{
                cout<<"Success create mklfs. sizeof metadata "<<sizeof(p)<<endl;
                return 0;
            }

        }else{
            cout<<"Fail to open flash file "<<filename<<endl;
            return 1;
        }

      }
}
int main(int argc, char *argv[])
{
	 cout<<"hell World"<<endl;
 //  createTest();
//	 testWrite("This is the first time to use flash libraray l\n");
//	testRead();
    return 1;
}
