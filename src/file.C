
#include "file.h"

int GLOBAL_INUM = 0;
static const int IFILE_INUM = 0;
static const int TYPE_F = 0;
static const int TYPE_D = 1;

Ifile *ifile = new Ifile;

// return 0 if things are fine, 1 if error


#define INODE_SIZE 144


int Inode_Create(int inum, int type, struct Inode *iptr) {

	printf("Creating inum %d of type %d\n", inum, type);
    if (inum == IFILE_INUM) {
        iptr->filename = "/.ifile";
    } else {
        iptr->filename = "";
    }
    iptr->inum = inum;
    iptr->type = type; 
    iptr->in_use = 1;

    if (type == 0) { //then file so Unix default is 664
        iptr->permissions = 664; //default for file (0) is 664, directory (1) is 775
    } else {
        iptr->permissions = 775; //default for file (0) is 664, directory (1) is 775
    }
    
    struct tm * time_of_last_change;
    time_t rawtime;
    time ( &rawtime );
    iptr->time_of_last_change = localtime( &rawtime );
    

    GLOBAL_INUM++; //Katy Bug: when crashes, resets to 0, this is bad


    // Update the ifile DS right away if creating ifile's inode
    if (inum != IFILE_INUM) {
        ifile->data.push_back(*iptr);
    }


    // Update the ifile's inode
//   Inode ifileInode = ifile->data.front();
 //   ifileInode.time_of_last_change = localtime(&rawtime);
    
    
    // printf( "Inode %d created at: %s", inum, asctime( iptr->time_of_last_change));
    // printf("Ifile inode write contents: \n\t %d %d %s %d %s\n", iptr->inum, iptr->permissions, iptr->owner, iptr->size, iptr->filename);
    
    return 0;
}

int Change_File_Permissions(int inum, int permissions) {
    return 0;
}

int Change_File_Owner(int inum) {
    return 0;
}

int Change_File_Group(int inum) {
    return 0;
}


/* File_Read
Reads the contents of the blocks that the Inode "inum" points to
*/
int File_Read(int inum, int offset, int length, void* buffer) {

    // Consult inode map to get disk address for inode inum
    printf("File_Read from ifileDS: inum %d is type %d\n", ifile->data.at(inum-1).inum, ifile->data.at(inum-1).type);
    
    Inode *iptr = new Inode;
  //  Get_Inode(inum, iptr);
    iptr=&ifile->data[inum-1];
    //KATY HERE BLOCK POINTER ISN"T SET PROPERLY
    printf("Have inode %d that is at seg %d block %d \n", iptr->inum, iptr->Block1Ptr.segmentNo, iptr->Block1Ptr.blockNo);
    // Read inode into memory (if not already in buffer cache)
    // and find appropriate data pointer

    // Read the data block that is referenced by the data pointer
    logAddress ladd;
    ladd.blockNo = iptr->Block1Ptr.blockNo; 
    ladd.segmentNo = iptr->Block1Ptr.segmentNo;
    int flag;
  //  printf("Will call read file %d from segment %d and block %d with length %d and buffer %s\n", inum, ladd.blockNo, ladd.segmentNo, length, buffer);

    // if (length > BLOCK_SIZE) {
    //     Call Log_read enoguh times 


    //     return a buffer with a pointer in it


    //     use memcpy 
    //     memcopy(buffer, pointer, i)
    // }
    char content[BLOCK_SIZE];
    flag = Log_read(ladd, length, (void*)content); 
    if (flag) {
        printf("Error: File_Read\n");
    } else{
        char *writebuf=content;
         writebuf+=offset;
       //  cout<<"file read ="<<content<<"writebuf ="<<*writebuf<<endl;
        memcpy(buffer,writebuf,length);
    }
    
    return 0;
}

 // Creates an Inode and records it
int File_Create(int inum, int type) {

    // Make an Inode for the file and add it to the ifile datastructure
    struct Inode *inode = new Inode;
    Inode_Create(inum, type, inode); 

    int flag;
    u_int block = 0;
    void *buf = (void *) inode;

    // Write the inode to disk
    logAddress inodeAdd; 
    void * nbuf = (void *) inode;
    flag = Log_Write(inum, block, INODE_SIZE, (void *)nbuf, inodeAdd); //Xing -- replace with Inode
    if (flag) {
        printf("Error: Failed to write inode %d. \n", inum);
        return 1;
    } 
    printf("Wrote inode %d at %d %d\n",inum, inodeAdd.blockNo, inodeAdd.segmentNo);

    // Store the location of the inode into the ifile

     ifile->data[inum-1].Block2Ptr=inodeAdd;
//    ifile->data.push_back(tmp);
  //  ifile->data.push_back(*inode);
    ifile->size = ifile->data.size() * INODE_SIZE;
    
// Notice: not need for every write, we can make it for every 10 write
     if(inum%10==0){
        //Write the ifile to disk    
        logAddress ifileAdd;
        void * fbuf = (void *) ifile;
        int length = sizeof(ifile);
        printf("Size of ifile %d\n", length);
        flag = Log_Write(IFILE_INUM, block, length, (void *)fbuf, ifileAdd); //Xing -- replace with Inode
        ifile->addresses.push_back(ifileAdd);
        if (flag) {
            printf("Error: Failed to write ifile. \n");
         //   return 1;
        } else {
            printf("Ifile updated at: block %d, segment %d\n", ifileAdd.blockNo, ifileAdd.segmentNo);
            ifileAddress.push_back(ifileAdd);
        }
    }
    
    return 0;
}

/* File_Write
Assigns the block pointers of the Inode with inum "inum"
inum: the inum of the inode of the file to be accessed
offset: the starting offset of the I/O in bytes
length: length of the I/O in bytes
buffer: the I/O, what we're writing
// */
int File_Write(int inum, int offset, int length, void* buffer) {
    
    int flag;

    // Write inode file contents
    // First convert offset to blocks
    int block = offset/BLOCK_SIZE;  //integer division. Can't get partial blocks?
    printf("Writing at block %d...\n", block);
/**************************************Just for phase 1, later no need consider more block write***************************************************************/
     // // Retrieve file
      Inode *fileinode;
    // Get_Inode(inum, fileinode);

     if(ifile->data.size()<=inum){
        fileinode=&ifile->data[inum-1];
      } else{
        cout<<"Error: cannot get inum from ifile , check create file or not  inum="<<inum<<endl;
        return 0;
      }


      logAddress pRead = fileinode->Block1Ptr;
      
      char rbuf[BLOCK_SIZE];
      char content[BLOCK_SIZE];
      memset(content,'0x00',BLOCK_SIZE);
      char *writebuf=content;
      logAddress dataAdd;

      if(pRead.segmentNo!=0){
          if(!Log_read(pRead, BLOCK_SIZE, rbuf)){
                if(offset+length<BLOCK_SIZE){
                    char *rPoint=rbuf;
                   memcpy(writebuf,rbuf,offset);  // from start to offset
                    writebuf+=offset;
               //     cout<<content<<"1st   "<<rbuf<<". writebuf="<<*writebuf<<" offset ="<<offset<<endl;
                    memcpy(writebuf,buffer,length);  // from offset to length
               //     cout<<content<<"2nd   "<<buffer<<". writebuf="<<*writebuf<<endl;
                    writebuf+=length-1;                   
                    rPoint=rPoint+offset+length-1; 
                    memcpy(writebuf,rPoint,BLOCK_SIZE-offset-length); // copy the ending to writebuf
               //   cout<<"writebuf ="<<*writebuf<<"rpoint ="<<*rPoint<<" rbuf="<<rbuf<<" content ="<<content<<". length="<<length<<endl;
                    if(!Log_Write(inum, 1, length, (void *) content, dataAdd)){
                         ifile->data[inum-1].Block1Ptr=dataAdd;  // update inode
                    }
                    else{
                        cout<<"File: write error  cannot write  "<<writebuf<<endl;
                    }
              }
          }else{
              cout<<"File: read error:  cannot read  inum="<<inum<<endl;
          }
      }else{

          if(offset+length<BLOCK_SIZE){
               writebuf+=offset;
               memcpy(writebuf,buffer,length);  // from offset to length
              if(!Log_Write(inum, 1, length, (void *) writebuf, dataAdd)){
                      ifile->data[inum-1].Block1Ptr=dataAdd;  // update inode
                }
                else{
                    cout<<"File: write error  cannot write  "<<writebuf<<endl;
                }
            }
      }


    return 0;
}


int File_Free(int inum) {
   //TODO
	printf("--File for inum-%d freed\n", inum);
    // Get Inode
    // Inode *iptr = new Inode;
    // Get_Inode(inum, itpr);

    // // Read inode/file's contents
    // logAddress logAddress1;1
    // Log_read(logAddress1);

    // // Free the inode/file's contents
    // int length = iptr->size;
    // Log_free(logAddress1, length);

    // // Read inode

    // // Free inode

    // // Update ifile
    return 0;
}



//Test_File_Create makes an Inode but writes "hello world - Test_File_Create" to log
int Test_File_Create(int inum) { 

    int type = TYPE_F;
    if (File_Create(inum, type) == 0) {
        printf("SUCCESS -- File_Create worked\n");
    } else {
        printf("TEST FAILED -- Test_File_Create\n");
    }

    printf("Ifile contents: \n");
    for (int i=0; i<ifile->data.size(); i++) {
        Inode thisInode = ifile->data.at(i);
        printf("inum: %d has data at block %d and seg %d\n", thisInode.inum, thisInode.Block1Ptr.blockNo, thisInode.Block1Ptr.segmentNo);
    }

    return 1;
}


int Ifile_Create() {
    // Make an Inode for the file
    int inum = IFILE_INUM;
    int type = TYPE_F;
    struct Inode *inode = new Inode;
    Inode_Create(inum, type, inode); 

    int flag;
    u_int block = 1;
    u_int length = ifile->size;

    // Determine where to put the ifile
    logAddress logAdd;

    // Write the ifile to disk
    void * buf = (void *) ifile;
    flag = Log_Write(IFILE_INUM, block, length, (void *)buf, logAdd); //Xing -- replace with Inode
    if (flag) {
        printf("Error: Failed to write ifile. \n");
    } else {
        printf("Initialized %s with block %d, segment %d\n", inode->filename.c_str(), inode->Block1Ptr.blockNo, inode->Block1Ptr.segmentNo);
        printf("         Written to block %d, segment %d\n", logAdd.blockNo, logAdd.segmentNo);
        printf("         Ifile inode info is at block %d segment %d\n", inode->Block1Ptr.blockNo, inode->Block1Ptr.segmentNo);
    }
    ifile->addresses.push_back(logAdd);
    ifile->size = sizeof(*ifile);
    // mataince a array for file
    ifileAddress.push_back(logAdd);

    //printf("-- Sanity check: in ifileDS there are %d inodes \n", ifile->data.size());

    // Check the ifile
    logAddress ladd = ifile->addresses.back();
    char rbuf[INODE_SIZE];
    flag = Log_read(ladd, INODE_SIZE, rbuf);
    if (flag) {
        printf("Error: File_Read\n");
    }
  //  Ifile *iread = (Ifile *)buf;
    //printf("Read ifile (inum %d at seg %d block %d) with vector size %d \n", iread->inum, iread->addresses.back().segmentNo, iread->addresses.back().blockNo, iread->size);
 //   Inode ifileInode = iread->data.front();
    return 0;
}   


int initFile() {

    // Create ifile inode, add it to the ifile data structure,
    // and write the ifile data structure fo disk
    init("FuseFileSystem");   
    Ifile_Create();

    return 0;
}


/***
before submission, a code review is necessary. 
Please first make your code no any compile bug, can pass basic tests, after that we
will do code review. Focuse on style , format , readable. 
I will optimize my code after I finish my part */

// int main(int argc, char *argv[])
// {

// 	// For Katy, when use log layer, fist make mklfs then can call
// 	// this use to init block size and segment , fuse file system
//     // When you want to use function from log Layer, comment main funcion of log.C

// 	printf("Begin file layer, creating ifile (and its inode)...\n");
//     initFile();
//     Test_File_Create(1);
//     test4(); //passes!
//     //test5();
//     test6();

//     return 1;
// }
