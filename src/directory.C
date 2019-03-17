#include "directory.h"
#include "file.h"
using namespace std;

int directoryInit(){
   char buffer[BLOCK_SIZE];
   if(!File_Read(IFILE,0, BLOCK_SIZE,buffer)){

   }
}
int 

int main(int argc, char *argv[])
{
	 cout<<"hell World"<<endl;
     init("FuseFileSystem");
   //  testWrite("This is the first time to use flash libraray 1 l\n",2);
   //  testWrite("This is the first time to use flash libraray 2 l\n",2);
 //    testWrite("This is the first time to use flash libraray 3 l\n",4);
 //    testWrite("This is the first time to use flash libraray 4 l\n",6);
 //    testRead(0);
      TestLogWrite();
	//  delete segmentCache;
    return 1;
}
