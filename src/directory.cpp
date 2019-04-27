#include "directory.h"
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;


map<string,vector<string> > *FileSystemMap;   // <path/directory, filename>

int inodeSize=144;   // default vaule



int initDirectory(int cachesize) {
   
    currentinum=initFile(cachesize);
    char buf[BLOCK_SIZE-100];
    if(!File_Read(2, 0, BLOCK_SIZE,buf)){
       FileSystemMap=(map<string,vector<string> > *) buf;
    } else{
      cout<<"Fail: get entry for all file "<<endl;
      return 1;
    }
    return 0;
}

int Directoy_getOneFile(const char *path, const char *filename,struct stat *stbuf){



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
			cout<<"test";
      vector<string> tmp=FileSystemMap->find(path);
      tmp.push_back(filename);
      FileSystemMap->insert(path,tmp);
		//	tables.push_back(pair<FileName,inum>(filecreate, currentinum));
		}
}



int Directoy_updateFile(const char *path, char *filename, struct stat *stbuf) {

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
   	Test_File_Create(1);
   	//File_Write(1, 0, 5, (void *) "hello");
   	Show_Ifile_Contents();

   	struct Inode in;
   	struct Inode got = File_Get(1);
    printf("File got\n");
    printf("Inum 1==%d: %d %d %c %c %d\n", got.inum, got.permissions, got.nlink, got.owner, got.group, got.size);
    
    // File_Get(1, &in);
    // printf("File got\n");
    // printf("Inum 1==%d: %d %d %c %c %d\n", in.inum, in.permissions, in.type, in.owner, in.group, in.size);
    // printf("KNOWN BUG: Calling File_Get and trying print inode.mtime (or any of the times) gives a segfault\n");
    struct stat mystat;
    convertInodeToStat(in, mystat);

    printf("	  Stat  |  Inode\n");
 	printf("uid_t: %d   |   %d\n", mystat.st_uid, in.owner);
 	printf("type:  %d   |   %d\n", mystat.st_mode, in.type);

	//  delete segmentCache;
    return 1;
}
