#include "directory.h"
// #include "cfile.cpp"
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
using namespace std;

// useful Link: http://man7.org/linux/man-pages/man7/inode.7.html


//  first character: # directory
//                   -  link
//                   eg {"#next",7} directory next  {"-a",8}  link a
map<string,vector<pair<string,int>>> FileSystemMap;   // <path/directory, filename>

int inodeSize=144;   // default vaule

#define TYPE_FILE 3
#define TYPE_DIRECTORY 2
#define TYPE_LINK   4

char allFileName[40][10];

struct stat stbuf;

int initDirectory(int cachesize) {
    cout<<"C++ compile success, fuse call function"<<endl;
    currentinum=initFile(cachesize);
    char buf[BLOCK_SIZE];
    FileSystemMap.clear();
    for(int i=2;i<currentinum;i++){
      struct Inode node;
      if(!File_Get(i, &node)&&node.in_use){
        vector<pair<string,int>> tmp;
        string a=node.filename;
        string b=node.path;
        tmp.push_back({a,node.inum});
        FileSystemMap.insert({b,tmp});
      }
    }

    // code for test directory 
    vector<pair<string,int> > tmp;
    tmp.push_back({"a.txt",11});
    tmp.push_back({"b.txt",4});
    tmp.push_back({"-link",9});
    tmp.push_back({".",9});
    FileSystemMap.insert({"/next",tmp});
    tmp.clear();
    // root directory
    tmp.push_back({".",10});
    tmp.push_back({"fuse.h",2});
    tmp.push_back({"log.cpp",3});
   // tmp.push_back({"hello",8});
    tmp.push_back({"#next",7});
    FileSystemMap.insert({"/",tmp});
    File_Create(2, 1);
    File_Create(3, 1);

    return 0;
}

int Directory_getOneFile(const char *path, const char *filename,struct stat *stbuf,char *filenameReturn){
        auto it=FileSystemMap.find(path);
        if(it!=FileSystemMap.end()){
            for(auto file: it->second){
                 if(strcmp(filename,file.first.c_str())==0){
                    struct Inode node;
                    struct stat t;
                    // if(!File_Get(file.second, &node)){
                    //    convertInodeToStat(node,t);
                    //    stbuf=&t;
                    //    strcat(filenameReturn,file.first.c_str());
                    //    return 0;
                    // }
                 //    strcat(filenameReturn,file.first.c_str());
                     memcpy(filenameReturn,file.first.c_str(),10);
                     return 0;
                 }
            }
        }

        return 1;

}

int SplitPath(const char * path, string &Entrypath,string &filename){
        string tmp=path;
        int position=tmp.find_last_of("/");
     //   string path1;
        if(position==0){
            Entrypath="/";
         } else{
             Entrypath=tmp.substr(0,position);
         }
         filename=tmp.substr(position+1);
        cout<<"path ="<<Entrypath<<" filename ="<<filename<<endl;
        return 0;
}

// get inum , stat , inode of file or entry
/**
*  return:   num --inum of file
    sfbuf-- fuse structure 
   input: path full path of entry
*/
int Directory_Types(const char *path,struct stat *stbuf,int *num){
      string tmp=path;
      auto it=FileSystemMap.find(path);
      if(it!=FileSystemMap.end()){
        InitStat(stbuf);                    // test code, later will use File_Get can covert inode into stat
        stbuf->st_mode = S_IFDIR | 0700;
        stbuf->st_nlink = 2;
        auto it1=it->second.begin();      // alway make "." stay on the front of vector
        if(it1->first=="."){
          int numSize=it1->second;

          memcpy(num,&numSize,sizeof(int));
       }
        return TYPE_DIRECTORY; // directory
      } else{
         string path1,filename;
        int position=tmp.find_last_of("/");
        
        if(position==0){
            path1="/";
         } else{
             path1=tmp.substr(0,position);
         }
         filename=tmp.substr(position+1);
   //     SplitPath(path,path1,filename);
        cout<<"path ="<<path1<<" filename ="<<filename<<endl;
        it=FileSystemMap.find(path1);
        if(it!=FileSystemMap.end()){
             for(auto it2: it->second){
            //    cout<<"iteration   &"<<path1<<"&   "<<it2.first<<endl;
                if(strcmp(it2.first.c_str(),filename.c_str())==0){
                   InitStat(stbuf);
                   stbuf->st_mode = S_IFREG | 0774;
                   stbuf->st_nlink = 1;
                   int numSize=it2.second;
                   memcpy(num,&numSize,sizeof(int));
                    return TYPE_FILE;  // file
                } else{
                   if(it2.first[0]=='-'){
                       tmp=it2.first.substr(1);
                       if(strcmp(tmp.c_str(),filename.c_str())==0){
                          stbuf->st_mode = S_IFLNK | 0700;
                          stbuf->st_nlink = 1;
                          int numSize=it2.second;
                          memcpy(num,&numSize,sizeof(int));
                          return TYPE_LINK;  // link
                       }
                   }
                }
             }
        }
        
      }
     
      return 0;   // not find
 
}
int Directory_getAllFiles(const char *path,struct stat *stbuf,int *size,char allFilename[50][10]){
        auto it=FileSystemMap.find(path);
        if(it!=FileSystemMap.end()){
           int arraySize=it->second.size();
           memcpy(size,&arraySize,sizeof(int));
           auto file=it->second.begin();
           for(int i=0;i<arraySize&&file!=it->second.end();i++){
                struct stat t;
                struct Inode node;
                // if(!File_Get(file->second, &node)){
                //        convertInodeToStat(node,t);
                //        stbuf[i]=t;
                //  }
               //  convertInodeToStat(node,t);

                  InitStat(&t);
                 if(file->first.find("#")!=std::string::npos){
                    string tmp=file->first.substr(1);
                    t.st_mode = S_IFDIR | 0700;
                    t.st_nlink = 2;
                    strcat(allFilename[i],tmp.c_str());
                 } else if(file->first[0]=='-'){
                    string tmp=file->first.substr(1);
                      t.st_mode = S_IFREG | 0774;
                      t.st_nlink = 1;
                      strcat(allFilename[i],tmp.c_str());
                 } else{
                       t.st_mode = S_IFLNK | 0700;
                       t.st_nlink = 1;
                       strcat(allFilename[i],file->first.c_str());
                 }
                 stbuf[i]=t;

               cout<<"filename "<<file->first<<endl;
               file++;
           }
           return 0;
        } else{
             string path1,filename;
             SplitPath(path,path1,filename);
             it=FileSystemMap.find(path1);
             if(it!=FileSystemMap.end()){
                for(auto it1:it->second){
                 if(strcmp(it1.first.c_str(),filename.c_str())==0){
                      int arraySize=1;
                      memcpy(size,&arraySize,sizeof(int));
                      strcat(allFilename[0],it1.first.c_str());
                      InitStat(&stbuf[0]);
                     return 0;  // one entry found
                 }
               }
             }

            return 1;  // no entry found
        }
    return 0;     
}


int Directory_Open(const char *path){
    auto it=FileSystemMap.find(path);
    if(it!=FileSystemMap.end()){   // entry exitf
       return 0;  // success
    } else
    return -1;   // fail
}

int Directory_EntyUpdate(const char *path,int type){
      auto it=FileSystemMap.find(path);
      string path1,filename;
      SplitPath(path,path1,filename);
      if(it!=FileSystemMap.end()){   // entry exit
          if(type==1){    // add
              cout<<"Error: entry already exit "<<endl;
              return 0;
          } else if(type==2){  // delete
              FileSystemMap.erase(it);  // delete directory
              auto it2=FileSystemMap.find(path1);  // directory from parents
              filename="#"+filename;
              if(it2!=FileSystemMap.end()){
                vector<pair<string,int> > tmp=it2->second;
                 vector<pair<string,int> >::iterator it3=tmp.begin();
                 while(it3!=tmp.end()){
                  //   cout<<path1<<"& "<<it3->first<<"   &"<<filename<<endl;
                    if(strcmp(it3->first.c_str(),filename.c_str())==0){
                         tmp.erase(it3);
                         FileSystemMap.erase(it2);
                         FileSystemMap.insert({path1,tmp});
                         cout<<"delete entry  "<<path<<"   "<<it3->first<<endl;
                          break;
                    }
                    it3++;
                 }
             }  
              return 1;
          }

      } else{   // entry not exit in table
        if(type==1){    // add
           vector<pair<string,int> > tmp;
           currentinum++;
           tmp.push_back({".",12});  // later use currentinum for num
           FileSystemMap.insert({path,tmp});
       //     File_Create(10, 1);   
           // parent directoy
           auto it2=FileSystemMap.find(path1);
           if(it2!=FileSystemMap.end()){
               filename="#"+filename;
               it2->second.push_back({filename,12});
           }  
           cout<<"debug  path="<<path1<<"& filename="<<filename<<"&"<<endl;
           cout<<" Add entry "<<path<<endl;
          return 1; 
         } else if(type==2){    // delete
             cout<<" Error: directory not exit delete "<<endl;
             return 0;
         }
      }
      return 1;
}

void  Directory_destory(){
     
    // File_destory();
}

 int InitStat(struct stat *stbuf){
    stbuf->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
    stbuf->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
    stbuf->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
    stbuf->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now
    return 1;
 }

int convertInodeToStat(struct Inode inode, struct stat s) {
	s.st_dev = 0;
	s.st_ino = inode.inum;
	if (inode.type) { //dir
	    s.st_mode = S_IFDIR;
	} else {
		s.st_mode = S_IFMT;
	}
    if(strcmp(inode.filename,".")==0)
      s.st_nlink = 2;  //  directory
    else 
      s.st_nlink = 1;   // file 

    s.st_uid = (uid_t) inode.owner;
    s.st_gid = (gid_t) ((u_int) inode.group);
    s.st_rdev = 0; //If file is character or block special
    s.st_size = (off_t) inode.size;
    s.st_atime = (time_t) inode.atime;
    s.st_mtime = (time_t) inode.mtime;
    s.st_ctime = (time_t) inode.ctime;
    s.st_blksize = BLOCK_SIZE;
    s.st_blocks = inode.numBlocks;

}


int Directory_createFile(const char *path, char *filename, struct stat *stbuf){
	   char *fullpath;
	   strcat(fullpath,path);
	   strcat(fullpath,filename);
		 currentinum++;
		if(!File_Create(currentinum,0)){
      File_Naming(currentinum,path,filename,stbuf);
			cout<<"test";
      auto it=FileSystemMap.find(path);
      if(it!=FileSystemMap.end()){
          vector<pair<string,int> > tmp;
          tmp.push_back({filename,currentinum});
          FileSystemMap.insert({path,tmp});
      } else{
           it->second.push_back({filename,currentinum});
          // FileSystemMap.insert({path,tmp});
      }
		}

    return 0;
}


int Directory_updateFile(const char *path, char *filename, struct stat *stbuf) {
	    
       return 0;
}


int Directory_deleteFile(const char *path,struct stat *stbuf) {
	// mark inode user=-1, then mark block point to be default value--call Log_writeDeadBlock
	     return 0;
} 

int Directory_readFile(const char *path, int offset, int size,char *buf) {
      // get num
      int num;
      if(Directory_Types(path,&stbuf,&num)==TYPE_FILE) {   // file
        cout<<"debug  directory  read |"<<buf<<" num="<<num<<" size ="<<size<<" offset="<<offset<<endl;
            File_Read(num, offset,size,buf);

       } else{
           cout<<" entry  "<<path<<"  not file  num="<<num<<endl;
       }

       // check file exit
       // check write permission
       // do read

	     return 0;
}

int Directory_writeFile(const char *path, int offset, int size,char *buf){
       // get inum
       int num;
       if(Directory_Types(path,&stbuf,&num)==TYPE_FILE) {   // file
           cout<<"debug  directory  write |"<<buf<<" num="<<num<<" size ="<<size<<" offset="<<offset<<endl;
            File_Write(num, offset,size,buf);
       } else{
           cout<<" entry  "<<path<<"  not file  num="<<num<<endl;
       }

       // check file exit
       // check write permission


       // do write
    //   File_Write(2, offset, ,buf)

       return 0;
}


int test1D() {
	printf("************* Begin Test1D: create a new file ****************\n");
	char* path = "test1/";
	char* fname = "testfile1";
	struct stat* stbuf;
	printf("Creating file %s %s (inum: %d)\n", path, fname, currentinum);
	if (Directory_createFile(path, fname, stbuf)) {
		printf("***Error: Directory_createFile -- test1D creating testfile1****\n");
		return 1;
	}
	Show_Ifile_Contents();
	printf("************* End Test1D: create a new file ****************\n");
	return 0;

}
int test2D() {
	char* path = "test2/";
	char* filename = "testfile2";
	struct stat* stbuf;
  char *filenameReturn;
	if (Directory_getOneFile(path, filename, stbuf,filenameReturn)) {
		printf("Error: Directoy_getOneFile\n");
		return 1;
	}

	return 0;
}


void test1(){
     initDirectory(4);
   //  char *path="/root/foo/bar";
     char *filename="test.txt";
     struct stat stbuf[10];
     int size=0;

   //  Directory_getAllFiles("/",stbuf,&size,allFileName);
   //  for(int i=0;i<size;i++){
   //   cout<<"size of file ="<<size<<"  filename= "<<allFileName[i]<<endl;
   //  }
   // char fileNameReturn[10];
   // if(Directory_getOneFile("/next/","a.txt",stbuf,fileNameReturn)){
   //    cout<<"No file found "<<endl;
   // } else{
   //  cout<<"filename ="<<fileNameReturn<<endl;
   // }
   // cout<<Directory_Types("/",stbuf,&size)<<"  num="<<size<<endl;

   // File_Create(2, 1);
   // char buf[40]="hello word test fuse";
   // File_Write(2, 0, 40,buf);
   // memset(buf,0,40);
   // File_Read(2, 0,40,buf);
   // cout<<"read from file "<<buf<<endl;
   //  createFile(path, filename,stbuf);
   //  createFile(path, filename,stbuf);
    Directory_EntyUpdate("/test",1);
     Directory_EntyUpdate("/test",1);
    int filetype=Directory_Types("/test",stbuf,&size);
    cout<<filetype<<"  inum="<<size<<endl;
    Directory_EntyUpdate("/test",2);
    filetype=Directory_Types("/test",stbuf,&size);
    cout<<filetype<<"  inum="<<size<<endl;

}


// int main(int argc, char *argv[])
// {
// 	cout<<"hell World"<<endl;
  
//    	printf("Passed test1\n");
//    	initDirectory(4);
//     test1();
//   //  	Test_File_Create(1);
//   //  	//File_Write(1, 0, 5, (void *) "hello");
//   //  	Show_Ifile_Contents();

// 	//  delete segmentCache;
//     return 1;
// }

