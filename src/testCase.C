#include <log.h>
#include <file.h>
void test1(){
	cout<<"*******************Log layer test 1 simple small write and read ******************************"<<endl;
	char *buf="Hello LFS, welcome to CSC 545 OS class";
	inum num=1;
	logAddress address;
	if(!Log_Write(num, 1, 40,(void*)buf,address)){
		char bufR[40];
		if(!Log_read(address, 40,(void *)bufR)){
			if(strcmp(buf,bufR)!=0){
				cout<<"Fail:  write string "<<buf<<" does not match read string "<<bufR<<endl;
			}else{
				cout<<"**************Success    test 1 pass*******************************"<<endl;
			}
		}
	}
}
void test2(){
	cout<<"*******************Log layer test 1 simple big write and read ******************************"<<endl;
	int size=510;   // size more than 512 read and write is the same, there are some problem with log_write point will fixed after phase 1
	char buf[size];
	for(int i=0;i<size;i++){
		buf[i]='a'+i%26;
	}
	inum num=1;
	logAddress address;
	if(!Log_Write(num, 1,size,(void*)buf,address)){
		char bufR[size];
		if(!Log_read(address, size,(void *)bufR)){
			if(strcmp(buf,bufR)!=0){
				cout<<"Fail: Big write string  does not match read string  (only one block read and write for log layer)"<<endl;
			}else{
				cout<<"**************Success    test 2 pass*******************************"<<endl;
			}
		}
	}
}
void test3(){
	cout<<"*********************Layer multipy write and read tests********************************************************"<<endl;
	string buf="test log write and read: hello LFS";
	logAddress address;
	logAddress read;
	 int  flag;
	 for(int i=0;i<24;i++){			// default a segment have 4 block, implement N=4 recently policy
	 	 buf="test log write and read: hello LFS th "+to_string(i)+"ith write";
	 	 const char *bufwrite=buf.c_str();
		 flag=Log_Write(1, 2, 50, (void*)bufwrite, address);
		 if(flag) {
		 	cout<<"Error: Fail Pass log write test"<<endl;
		 }else{
		 	if(i==0) read=address;
		 	// cout<<"Success write a log   segmentNo="<<address.segmentNo<<" blockNO=. "<<address.blockNo<<endl;
		 }
	}
	// first read , these should read from flash layer
	char readbuf[50];
	flag=Log_read(read, 50, (void*)readbuf);
	string expect="test log write and read: hello LFS th "+to_string(0)+"ith write";
	if(strcmp(readbuf,expect.c_str())!=0){
		cout<<"Fail implement 4 Most recently Access segment   write string "<<expect<<" does not math. "<<readbuf<<endl;
	} else{
		 flag=Log_read(read, 50, (void*)readbuf);
		 if(strcmp(readbuf,expect.c_str())==0){
		 	cout<<"************************Success test 3 Pass ******************************"<<endl;
		 }else{
		 	cout<<"Fail 2nd read from queue   write string "<<expect<<" does not math. "<<readbuf<<endl;
		 }
	}
	 
}
void test4(){
	cout<<"*******************File layer test 4 simple small write and read ******************************"<<endl;
	char *buf="Hello LFS, welcome to CSC 545 OS class";
	inum num=1;
	if(!File_Write(num, 0, 40, (void*)buf)){
	    char bufR[40];
	    if(!File_Read(num, 0, 40, (void*)bufR)){
	    	if(strcmp(buf,bufR)!=0){
				cout<<"Fail:  write string "<<buf<<" does not match read string "<<bufR<<endl;
			}else{
				cout<<"**************Success    test 4 pass*******************************"<<endl;
			}
	    }
	}
}
void test5(){
	cout<<"*******************File layer test 5 simple big write and read ******************************"<<endl;
	int size=300;   
	char buf[size];
	for(int i=0;i<size;i++){
		buf[i]='a'+i%26;
	}
	inum num=1;
	if(!File_Write(num, 0, size, (void*)buf)){
	    char bufR[size];
	    if(!File_Read(num, 0, size, (void*)bufR)){
	    	if(strcmp(buf,bufR)!=0){
				cout<<"Fail:  write string "<<buf<<" does not match read string "<<bufR<<endl;
			}else{
				cout<<"**************Success    test 5 pass*******************************"<<endl;
			}
	    }
	}
}
void test6(){
	cout<<"*******************File layer test 6 overwrite  ******************************"<<endl;
	char *buf="Hello LFS, welcome to CSC 545 OS class..";
	inum num=1;
	if(!File_Write(num, 0, 40, (void*)buf)){
		char *bufchange="Hello LFS, rewrite string.......";
		File_Write(num, 0, 40, (void*)bufchange);
	    char bufR[40];
	    if(!File_Read(num, 0, 40, (void*)bufR)){
	    	if(strcmp(bufchange,bufR)!=0){
				cout<<"Fail test 6:  write string "<<buf<<" does not match read string "<<bufR<<endl;
			}else{
				cout<<"**************Success    test 6  pass*******************************"<<endl;
			}
	    }
	}

}
void test7(){
	cout<<"*******************File layer test 7 concat string  ******************************"<<endl;
	char *buf="hello";  // 4+1 chararcter
	inum num=2;
	Test_File_Create(num);
	if(!File_Write(num, 0, 5, (void*)buf)){
		char *bufchange="world";
		if(File_Write(num, 6, 6, (void*)bufchange)){
			cout<<"test 7 write error "<<endl;
		}
	    char bufR[50];
	    char *expect="hello0world";
	    if(!File_Read(num, 0, 11, (void*)bufR)){ // total 11 character
	    	if(strcmp(expect,bufR)!=0){
				cout<<"Fail test 7:  write string >>"<<expect<<"<<does not match read string >>"<<bufR<<"<<"<<endl;
			}else{
				cout<<"**************Success    test 7  pass*******************************"<<endl;
			}
	    }
	}
}
void test8(){
	cout<<"*******************File layer test 8 offset read  ******************************"<<endl;
	char *buf="hello world";
	inum num=3;
	Test_File_Create(num);
	if(!File_Write(num, 0, 12, (void*)buf)){
		
	    char bufR[50];
	    char *expect="world";
	    if(!File_Read(num, 6, 12, (void*)bufR)){
	    	if(strcmp(expect,bufR)!=0){
				cout<<"Fail test 7:  write string >>"<<expect<<"<<does not match read string >>"<<bufR<<"<<"<<endl;
			}else{
				cout<<"**************Success    test 8  pass*******************************"<<endl;
			}
	    }
	}
}





// void test1(char *cat)
// {
// 	//	printf("*******************Log layer test 1 simple small write and read ****************************** \n");
// 	char buf[50] = "Hello LFS, welcome to CSC 545 OS class";
// 	strcat(buf, cat);
// 	//	char  *buf="Hello LFS, welcome to CSC 545 OS class";
// 	inum num = 1;
// 	struct logAddress address;
// 	if (!Log_Write(num, 1, 50, (void *)buf, &address))
// 	{
// 		//	cout<<"buf ="<<buf<<">"<<endl;
// 		char bufR[50];
// 		if (!Log_read(address, 50, (void *)bufR))
// 		{
// 			printf("return logadress segmentNo= %d  blockNo=%d \n", address.segmentNo, address.blockNo);
// 			if (strcmp(buf, bufR) != 0)
// 			{
// 				printf("Fail:  write string  %s does not match read string %s \n", buf, bufR);
// 			}
// 			else
// 			{
// 				//		printf("**************Success    test 1 pass*******************************\n ");
// 			}
// 		}
// 	}

// 	// Log_writeDeadBlock(num,address,address);
// }
// void test2(int b, struct logAddress address, char *buf)
// {
// 	char buf1[1];
// 	for (int i = 0; i <= b * BLOCK_NUMBER; i++)
// 	{
// 		buf1[0] = 'a' + (i % 26);
// 		test1(buf1);
// 	}
// 	// struct logAddress address;
// 	// address.segmentNo=2;
// 	// address.blockNo=32;
// 	// char *buf="Hello LFS, welcome to CSC 545 OS classf";
// 	char bufR[50];
// 	if (!Log_read(address, 50, (void *)bufR))
// 	{
// 		printf("return logadress segmentNo= %d  blockNo=%d \n", address.segmentNo, address.blockNo);
// 		if (strcmp(buf, bufR) != 0)
// 		{
// 			printf("Fail:  write string  %s does not match read string %s \n", buf, bufR);
// 		}
// 		else
// 		{
// 			printf("**************Success    test 1 pass*******************************\n ");
// 		}
// 	}
// }

// int test3()
// {

// 	logAddress oldAdrress, newAdress;
// 	logAddress address;
// 	address.segmentNo = 2;
// 	address.blockNo = 32;
// 	char buf[50] = "Hello LFS, welcome to CSC 545 OS classf";
// 	test2(1, address, buf);
// 	Log_CheckPoint(&oldAdrress, &newAdress, 1, 1);
// 	// pmetadata->currentsector=128;
// 	cout << " rewrite metadata " << endl;
// }
// void test5()
// {

// 	char buf[50] = "Destory test";
// 	logAddress address;
// 	Log_Write(1, 1, 50, (void *)buf, &address);
// 	Log_destroy();
// }
// void test6()
// {
// 	logAddress address;
// 	address.segmentNo = 1;
// 	address.blockNo = 1;
// 	char bufR[50];
// 	Log_read(address, 50, (void *)bufR);
// 	cout << "after destory  content " << bufR << endl;
// }

// // checkpoint size test
// void test7(){
// 	logAddress oldAdrress,newAdress;
//     Log_CheckPoint(&oldAdrress, &newAdress, 1, 1);
//     logAddress *newOne=(struct logAddress *) malloc(sizeof(logAddress)*2);
//     Log_CheckPoint(&newAdress, newOne, 1, 2);
//     cout<<"before checkpoint size "<<Log_GetIfleAddress(newOne, 1)<<endl;
// }
// void test8(){
// 	logAddress oldAdrress,newAdress;
// 	logAddress *newOne=(struct logAddress *) malloc(sizeof(logAddress)*3);
// 	cout<<"checkpoint size "<<Log_GetIfleAddress(newOne, 1)<<endl;
// 	cout<<"size of segment ="<<sizeof(struct SegmentUsageTable)<<endl;

// }
// void test1()
// {
//   //  char *path="/root/foo/bar";
//   char *filename = "test.txt";
//   struct stat stbuf[10];
//   int size = 0;
//   Directory_getAllFiles("/",stbuf,&size,allFileName);
//   printf("Done Directory_getAllFiles\n");
 
//   // printf("Done Directory_createFile\n");
//   // char* path;
//   // int offset = 0;
//   // int size = 10;
//   // char *buf = "dir write";
//   // Directory_writeFile(const char *path, int offset, int size, char *buf);
  
//   //  for(int i=0;i<size;i++){
//   //   cout<<"size of file ="<<size<<"  filename= "<<allFileName[i]<<endl;
//   //  }
//   // char fileNameReturn[10];
//   // if(Directory_getOneFile("/next/","a.txt",stbuf,fileNameReturn)){
//   //    cout<<"No file found "<<endl;
//   // } else{
//   //  cout<<"filename ="<<fileNameReturn<<endl;
//   // }
//   // cout<<Directory_Types("/",stbuf,&size)<<"  num="<<size<<endl;

//   // File_Create(2, 1);
//   // char buf[40]="hello word test fuse";
//   // File_Write(2, 0, 40,buf);
//   // memset(buf,0,40);
//   // File_Read(2, 0,40,buf);
//   // cout<<"read from file "<<buf<<endl;
//   //  createFile(path, filename,stbuf);
//   //  createFile(path, filename,stbuf);
//   Directory_EntyUpdate("/test", 1);
//   int filetype = Directory_Types("/next/s", stbuf, &size);
//   cout << filetype << "  inum=" << size << endl;
//   // Directory_EntryRename("/test", "/update.h",TYPE_DIRECTORY);
//   // // vector<pair<string, int>> tmp;
//   // // tmp.push_back({"update...", 2});
//   // // char *test="/";
//   // // FileSystemMap[test]=tmp;
  
//   // filetype = Directory_Types("/updatej", stbuf, &size);
//   // cout << filetype << "  inum=" << size << endl;
// }
// // init test
// void testD3(){

// vector<pair<string,string> > pathtest={{"/next","."},{"/next","c.txt"},{"/next/next","."},{"/next/next","d.txt"}};
//     int count=0;
//     for(int i=0;i<pathtest.size();i++){
//       vector<pair<string, int>> tmp;
//       string a = pathtest[i].first;
//       string b = pathtest[i].second;
//       auto it=FileSystemMap.find(a);
//       if(it!=FileSystemMap.end()){
//         tmp=it->second;
//       }
//       // parent directory
//       string path1,filename;
//       SplitPath(a.c_str(),path1,filename);

//       if(b=="."){
//         cout<<"debug Directory "<<a<<"    "<<b<<endl;
//           vector<pair<string, int>> tmpParent;
//           auto it2=FileSystemMap.find(path1);
//           if(it2!=FileSystemMap.end()){  // parent directory exit
//                tmpParent=it2->second;
//                filename="#"+filename;
//                tmpParent.push_back({filename,count++});
//                 FileSystemMap[path1]=tmpParent;
//           }
//       }
//       tmp.push_back({b, count++});
//       FileSystemMap[a]=tmp;
//     }
// }



// void test2(){

//     struct stat *stbuf;
//     Directory_createFile("/a.txt", stbuf);
//     Directory_createFile("/j.txt", stbuf);
//     //Show_Ifile_Contents();

//     struct stat dirbuf[10];
//     int size = 0;
//     int inum;
//     Directory_Types("/",&dirbuf[0],&inum);
//     // Directory_getAllFiles("/",dirbuf,&size,allFileName);
//     // Directory_writeFile("/a.txt", 0, 6, "hello");
//     //Directory_writeFile("/j.txt", 0, 6, "omega");

//  //   File_Destroy();
//     // int inum = 1;
//     // Change_Permissions(inum, "666");
//     // Inode in;
//     // File_Get(inum, &in);
//     // struct stat s;
//     // convertInodeToStat(&in, &s);
//    //  currentinum++;
//    //  char buf[40]="hello";
//    // File_Create(currentinum, 0);  // add directory
//    //  File_Write(currentinum,0,6,buf);
// }


int main(int argc, char *argv[]){
	init("FuseFileSystem");   
	// log layer test cases
	test1();
	test2();
	test3();

	// file layer test case
	initFile();
	Test_File_Create(1);
	test4();
	//test5();  // 
	test6();
	test7();
	test8();
	return 1;
}