#include "directory.h"
// #include "cfile.cpp"
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;


map<string,vector<pair<string,int>>> FileSystemMap;   // <path/directory, filename>

int inodeSize=144;   // default vaule



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
    tmp.push_back({"a.txt",3});
    tmp.push_back({"b.txt",4});
    FileSystemMap.insert({"/next/",tmp});
    tmp.clear();
    // root directory
    tmp.push_back({"fuse.h",5});
    tmp.push_back({"log.cpp",6});
    tmp.push_back({"#next",7});
    FileSystemMap.insert({"/",tmp});

    return 0;
}

int Directory_getOneFile(const char *path, const char *filename,struct stat *stbuf){
        auto it=FileSystemMap.find(path);
        if(it!=FileSystemMap.end()){
            for(auto file: it->second){
                 if(strcmp(filename,file.first.c_str())==0){
                    struct Inode node;
                    struct stat t;
                    if(!File_Get(file.second, &node)){
                       convertInodeToStat(node,t);
                       stbuf=&t;
                       return 0;
                    }
                 }
            }
        }

        return 1;

}
int Directory_getAllFiles(const char *path,struct stat *stbuf,int size){
        auto it=FileSystemMap.find(path);
        if(it!=FileSystemMap.end()){
           size=it->second.size();
           auto file=it->second.begin();
           for(int i=0;i<size&&file!=it->second.end();i++){
                struct stat t;
                struct Inode node;
                // if(!File_Get(file->second, &node)){
                //        convertInodeToStat(node,t);
                //        stbuf[i]=t;
                //  }
                 convertInodeToStat(node,t);
                 stbuf[i]=t;
              file++;
           }
        }
        return 0;     
}

void  Directory_destory(){
     
    // File_destory();
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



int Directory_deleteFile(const char *path,char *filename,struct stat *stbuf) {
	// mark inode user=-1, then mark block point to be default value--call Log_writeDeadBlock
	     return 0;
} 

int Directory_readFile(const char *path, char *filename, int offset, char *buf) {
	     return 0;

}

int Directory_writeFile(const char *path, char *filename, int offset, char *buf){
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
	if (Directory_getOneFile(path, filename, stbuf)) {
		printf("Error: Directoy_getOneFile\n");
		return 1;
	}

	return 0;
}


void test1(){
     initDirectory(4);
     char *path="/root/foo/bar";
     char *filename="test.txt";
     struct stat *stbuf;
   //  createFile(path, filename,stbuf);
   //  createFile(path, filename,stbuf);

}


// int main(int argc, char *argv[])
// {
// 	cout<<"hell World"<<endl;
  
//  	printf("Passed test1\n");
//    	initDirectory(4);
//   //  	Test_File_Create(1);
//   //  	//File_Write(1, 0, 5, (void *) "hello");
//   //  	Show_Ifile_Contents();

// 	//  delete segmentCache;
//     return 1;
// }

