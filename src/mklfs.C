#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <flash.h>
#include "log.h"
using namespace std;
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
           Flash_Close(f); 

        }else{
            cout<<"Fail to open flash file "<<filename<<endl;
            return 1;
        }

      }
}
int main(int argc, char *argv[])
{
    if(argc<2)
	cout<<"create a mklfs: usage  mklfs  optional filename"<<endl;
    int blocksize=2;
    int segmentsize=32;
    int wearlimit=1000;
    int flashSizeInsegment=100;
    for(int i=1;i<argc;i++){
        if(argv[i]=="-b"){
            if(i+2<=argc){
                blocksize=stoi(argv[i+1]);
            }else{
                cout<<"Error usage: -b must follow a number  ex: -b 2"<<endl;
            }
        }
        if(argv[i]=="-l"){
            if(i+2<=argc){
                segmentsize=stoi(argv[i+1]);
            }else{
                cout<<"Error usage: -l must follow a number ex: -l 32 "<<endl;
            }
        }
        if(argv[i]=="-s"){
            if(i+2<=argc){
                flashSizeInsegment=stoi(argv[i+1]);
            }else{
                cout<<"Error usage: -s must follow a number: ex -s 100 "<<endl;
            }
        }
        if(argv[i]=="-w"){
            if(i+2<argc){
                wearlimit=stoi(argv[i+1]);
            } else{
                cout<<"Error usage: -w must follow a number: ex -w 100 "<<endl;
            }
        }
    }

   int flag=createMklfs(argv[argc-1],blocksize,segmentsize,wearlimit,flashSizeInsegment);

    return flag;
}
