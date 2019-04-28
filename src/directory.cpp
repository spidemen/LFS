#include "directory.h"

#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;


map<string,vector<pair<string,int>>> FileSystemMap;   // <path/directory, filename>

int inodeSize=144;   // default vaule



int initDirectory(int cachesize) {
   
    currentinum=initFile(cachesize);
    char buf[BLOCK_SIZE];
    FileSystemMap.clear();
    for(int i=2;i<currentinum;i++){
      struct Inode node;
      if(!File_Get(i, &node)){
        vector<pair<string,int>> tmp;
        string a=node.filename;
        string b=node.path;
        tmp.push_back({a,node.inum});
        FileSystemMap.insert({b,tmp});
      }
    }
    return 0;
}

int Directoy_getOneFile(const char *path, const char *filename,struct stat *stbuf){
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
int Directoy_getAllFiles(const char *path,struct Inode *files,struct stat *stbuf,int size){
<<<<<<< HEAD
                        
=======

        return 0;     

}

void  Directoy_destory(){
     
    // File_destory();
>>>>>>> f112644d66aaab9eaa8cfed32518749c3acbf4a4
}

int convertInodeToStat(struct Inode inode, struct stat s) {
	s.st_dev = 0;
	s.st_ino = inode.inum;
	if (inode.type) { //dir
	    s.st_mode = S_IFDIR;
	} else {
		s.st_mode = S_IFMT;
	}
    s.st_nlink = 1; //number of links to the file
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


int createFile(const char *path, char *filename, struct stat *stbuf){
	   char *fullpath;
	   strcat(fullpath,path);
	   strcat(fullpath,filename);
		currentinum++;
		if(!File_Create(currentinum,0)){
      File_Naming(currentinum,path,filename);
			cout<<"test";
      vector<pair<string,int> > tmp=FileSystemMap.find(path)->second;
      tmp.push_back({filename,currentinum});
      FileSystemMap.insert({path,tmp});
		//	tables.push_back(pair<FileName,inum>(filecreate, currentinum));
		}
}

int Directory_getOneFile(const char *path, const char *filename,struct stat *stbuf) {
	struct Inode in = File_Get(1);
	return 0;
}

int Directory_updateFile(const char *path, char *filename, struct stat *stbuf) {
	return 0;
}

int Directory_createFile(const char *path, char *filename, struct stat *stbuf) {
	printf("HERE IM IN CREATE FILE: %s %s\n", path, filename);

   	char *fullpath;
   	fullpath = (char *) calloc(strlen(path) + strlen(filename) + 1, sizeof(char));
   	strcpy(fullpath,path);
   	printf("fullpath1: %s\n", fullpath);
   	strcat(fullpath,filename);
   	printf("fullpath2: %s\n", fullpath);
	currentinum++;
	printf("Creating file %s (inum: %d)\n", fullpath, currentinum);
	if(!File_Create(currentinum,0)){ //always 0 because this is creates a file
		cout<<"test";

		//tables.push_back(pair<FileName,inum>(filename, currentinum));
	}
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

void  Directory_destory() {
	return;
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
     createFile(path, filename,stbuf);
     createFile(path, filename,stbuf);

}
int main(int argc, char *argv[])
{
	cout<<"hell World"<<endl;
<<<<<<< HEAD
   	initDirectory();
   	test1D();
   	//if () return 1;
 	printf("Passed test1\n");
=======
   	initDirectory(4);
  //  	Test_File_Create(1);
  //  	//File_Write(1, 0, 5, (void *) "hello");
  //  	Show_Ifile_Contents();
>>>>>>> f112644d66aaab9eaa8cfed32518749c3acbf4a4

  //  	struct Inode in;
  //  	struct Inode got = File_Get(1);
  //   printf("File got\n");
<<<<<<< HEAD
  //   printf("Inum 1==%d: %d %d %c %c %d %s\n", got.inum, got.permissions, got.nlink, got.owner, got.group, got.size, got.mtime);
    
=======
  //   printf("Inum 1==%d: %d %d %c %c %d\n", got.inum, got.permissions, got.nlink, got.owner, got.group, got.size);
    
  //   // File_Get(1, &in);
  //   // printf("File got\n");
  //   // printf("Inum 1==%d: %d %d %c %c %d\n", in.inum, in.permissions, in.type, in.owner, in.group, in.size);
  //   // printf("KNOWN BUG: Calling File_Get and trying print inode.mtime (or any of the times) gives a segfault\n");
>>>>>>> f112644d66aaab9eaa8cfed32518749c3acbf4a4
  //   struct stat mystat;
  //   convertInodeToStat(in, mystat);

  //   printf("	  Stat  |  Inode\n");
 	// printf("uid_t: %d   |   %d\n", mystat.st_uid, in.owner);
 	// printf("type:  %d   |   %d\n", mystat.st_mode, in.type);
<<<<<<< HEAD

 	
=======
>>>>>>> f112644d66aaab9eaa8cfed32518749c3acbf4a4

	//  delete segmentCache;
    return 1;
}
