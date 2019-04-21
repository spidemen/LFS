#include "directory.h"
#include "file.h"
#include "log.h"
<<<<<<< HEAD
<<<<<<< HEAD
using namespace std;

=======
=======

>>>>>>> 779ff9f55c81aef2f6459092e9fd72da05b182a1
#include <map>
#include <vector>
using namespace std;


map<string path,vector<string filename>> FileSystemMap; 


<<<<<<< HEAD
>>>>>>> ebeb226ab1ba6b6e0b7d968858be397a7dca2ad5
=======
>>>>>>> 779ff9f55c81aef2f6459092e9fd72da05b182a1
int directoryInit(){
   char buffer[BLOCK_SIZE];
   if(!File_Read(IFILE,0, BLOCK_SIZE,buffer)){
		Ifile *file=(Ifile *)buffer;
		currentinum=file->data.size();   
		for(int i=0;i<file->data.size();i++){
			tables.push_back(pair<FileName,inum>(file->data[i].filename,file->data[i].inum));
		}
   }
}
int directoryCreate(char *filename, char *path){
		string filecreate=path+filename;
		currentinum++;
		if(!File_Create(inum,0)){
			tables.push_back(pair<FileName,inum>(filecreate, currentinum));
		}
}
// int main(int argc, char *argv[])
// {
// 	 cout<<"hell World"<<endl;
   	
// 	//  delete segmentCache;
//     return 1;
// }
