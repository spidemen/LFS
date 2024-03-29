#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <flash.h>
#include <map>
#include "log.cpp"
using namespace std;

//char *filename="FuseFileSystem";
int totalsectors=SUPERBLOCK;
int createMklfs(char * filename,int blocksize,int segmentsize=32,int wearlimit=10000,int flashSizeInsegment=100){
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
        //    p->checkpointStart=1;
            p->limit=wearlimit;
            p->currentsector=segmentsize*blocksize;
            p->currentBlockNumber=0;
            p->currentSegmentNumber=1;
            p->reUsedTableSize=0;
         //   totalsectors=blocksize*segmentsize;
            if(Flash_Write(f, 0, totalsectors, (void*)p)){
                cout<<"Error: cannot write metadat to the flash "<<endl;
                return 1;
            }else{
                cout<<"Success create mklfs. sizeof metadata "<<sizeof(p)<<"total block="<<totalblock<<endl;
                return 0;
            }

           Flash_Close(f); 
        //   delete p;

        }else{
            cout<<"Fail to open flash file "<<filename<<endl;
            return 1;
        }

    }
}
int readMklfs(char *filename){
    u_int blocks;
    Flash f=Flash_Open(filename,FLASH_ASYNC, &blocks);
    if(f!=NULL){
      //  void *buf;
        char buf[totalsectors*512];
        if(Flash_Read(f, 0,totalsectors,buf)){
            cout<<"Read Flash Error: canont read "<<buf<<" to flash  total block "<<1<<endl;
        }else{
            cout<<"Success Read  Flash   buf="<<endl;
            metadata *p= (metadata *)buf;
            cout<<" metadata  blocksize ="<<p->blocksize<<endl;
            cout<<"  metadata  segment size in unit blocks "<<p->segmentsize<<endl;
            cout<<"metadata   total  segments  "<<p->segments<<endl;
            cout<<"metadata   wearlimit ="<<p->limit<<endl;
        }
    }

}

int main(int argc, char *argv[])
{
    if(argc<2)
	 { 
       cout<<"create a mklfs: usage  mklfs   [optional] filename"<<endl;
       cout<<"  -b size, --block=size "<<endl;
       cout<<"Size of a block, in sectors. The default is 2 (1KB) "<<endl;
       cout<<"-l size, --segment=size  "<<endl;
       cout<<"Segment size, in blocks. The segment size must be a multiple of the flash"  
       <<"erase block size, report an error otherwise. The default is 32"<<endl;
       cout<<"-s segments, --segments=segments "<<endl;
       cout<<"Size of the flash, in segments.  The default is 100 "<<endl;
       cout<<"-w limit, --wearlimit=limit "<<endl;
       cout<<"Wear limit for erase blocks. The default is 1000"<<endl;

      }
  // default vaule 
    int blocksize=2;
    int segmentsize=32;
    int wearlimit=1000;
    int flashSizeInsegment=100;
    for(int i=1;i<argc-1;){
       // cout<<".  "<<argv[i]<<endl;
        if(strcmp(argv[i],"-b")==0){
            if(i+2<=argc){
                blocksize=stoi(argv[i+1]);
                i+=2;
                continue;
            }else{
                cout<<"Error usage: -b must follow a number  ex: -b 2"<<endl;
            }
        } else
        if(strcmp(argv[i],"-l")==0){
            if(i+2<=argc){
                segmentsize=stoi(argv[i+1]);
                 i+=2;
                 continue;
            }else{
                cout<<"Error usage: -l must follow a number ex: -l 32 "<<endl;
            }
        } else
        if(strcmp(argv[i],"-s")==0){
            if(i+2<=argc){
                flashSizeInsegment=stoi(argv[i+1]);
                 i+=2;
                 continue;
            }else{
                cout<<"Error usage: -s must follow a number: ex -s 100 "<<endl;
            }
        }
        if(strcmp(argv[i],"-w")==0){
            if(i+2<argc){
                wearlimit=stoi(argv[i+1]);
                  i+=2;
                  continue;
            } else{
                cout<<"Error usage: -w must follow a number: ex -w 100 "<<endl;
            }
        } else
         {
          //   cout<<" i= "<<i<<" argc ="<<argc<<endl;
            cout<<"create a mklfs: usage  mklfs   [optional] filename"<<endl;
           cout<<"  -b size, --block=size "<<endl;
           cout<<"Size of a block, in sectors. The default is 2 (1KB) "<<endl;
           cout<<"-l size, --segment=size  "<<endl;
           cout<<"Segment size, in blocks. The segment size must be a multiple of the flash"  
           <<"erase block size, report an error otherwise. The default is 32"<<endl;
           cout<<"-s segments, --segments=segments "<<endl;
           cout<<"Size of the flash, in segments.  The default is 100 "<<endl;
           cout<<"-w limit, --wearlimit=limit "<<endl;
           cout<<"Wear limit for erase blocks. The default is 1000"<<endl;
           return 0;
        }
    }

    int flag=createMklfs(argv[argc-1],blocksize,segmentsize,wearlimit,flashSizeInsegment);
    filename=argv[argc-1];
    readMklfs(argv[argc-1]);
    return flag;
}
