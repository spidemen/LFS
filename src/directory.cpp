#include "directory.h"
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;


map<string,vector<pair<string,int> >> FileSystemMap;   // <path/directory, filename>

int inodeSize=144;   // default vaule



int initDirectory(int cachesize) {
   
    currentinum=initFile(cachesize);
    char buf[BLOCK_SIZE-100];
    FileSystemMap.clear();
    for(int i=2;i<currentinum;i++){
      struct Inode node;
      if(!File_Get(i, &node)){
        vector<string> tmp;
        tmp.push_back({node.filename,node.inum});
        FileSystemMap.insert(node.directory,tmp);
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
                    if(!File_Get(i, &node)){
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
      vector<string> tmp=FileSystemMap.find(path);
      tmp.push_back({filename,currentinum});
      FileSystemMap.insert(path,tmp);
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
     initDirectory();
     char *path="/root/foo/bar";
     char *filename="test.txt";
     struct stat *stbuf;
     createFile(path, filename,stbuf);
     createFile(path, filename,stbuf);

}
int main(int argc, char *argv[])
{
	cout<<"hell World"<<endl;
   	initDirectory();
   	test1D();
   	//if () return 1;
 	printf("Passed test1\n");

  //  	struct Inode in;
  //  	struct Inode got = File_Get(1);
  //   printf("File got\n");
  //   printf("Inum 1==%d: %d %d %c %c %d %s\n", got.inum, got.permissions, got.nlink, got.owner, got.group, got.size, got.mtime);
    
  //   struct stat mystat;
  //   convertInodeToStat(in, mystat);

  //   printf("	  Stat  |  Inode\n");
 	// printf("uid_t: %d   |   %d\n", mystat.st_uid, in.owner);
 	// printf("type:  %d   |   %d\n", mystat.st_mode, in.type);

 	

	//  delete segmentCache;
    return 1;
}
