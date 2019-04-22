#include "directory.h"
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;


map<string,vector<string> > FileSystemMap; 

int inodeSize=144;   // default vaule

int currentinum=1;


int initDirectory(){
   
    initFile(4);
   cout<<"compile C++ test"<<endl;
   char buffer[BLOCK_SIZE];
   if(!File_Read(IFILE,0, BLOCK_SIZE,buffer)){
	//	Ifile *file=(Ifile *)buffer;
//		currentinum=file->data.size();   
		// for(int i=0;i<file->data.size();i++){
		// 	cout<<"test";
		// //	tables.push_back(pair<FileName,inum>(file->data[i].filename,file->data[i].inum));
		// }
   }
}

int createFile(const char *path, char *filename, struct stat *stbuf){
	   char *fullpath;
	   strcat(fullpath,path);
	   strcat(fullpath,filename);
		currentinum++;
		if(!File_Create(currentinum,0)){
			cout<<"test";
		//	tables.push_back(pair<FileName,inum>(filecreate, currentinum));
		}
}


// int main(int argc, char *argv[])
// {
// 	 cout<<"hell World"<<endl;
//    	directoryInit();
// 	//  delete segmentCache;
//     return 1;
// }
