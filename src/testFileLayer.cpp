

void test1F(){
	printf("*******************File layer test 1F simple small write and read ******************************\n");
	char *buf ="Hello LFS, welcome to CSC 545 OS class"; //38
	inum num=1;
	int offset = 0;
	int length = 39;
	File_Create(num, TYPE_F);
	Show_Ifile_Contents();
	if(!File_Write(num, offset, length, (void*)buf)){
		Show_Ifile_Contents();
	    char bufR[length];
	    if(!File_Read(num, offset, length, (void*)bufR)){
	    	if(strcmp(buf,bufR)!=0){
				printf("Fail:  write string %s does not match read string %s \n",buf,bufR);
			}else{
				printf("**************Success    test 1F pass*******************************\n");
			}
	    }
	}
}

void test2F(){
	printf("*******************File layer test 2F second simple small write and read ******************************\n");
	Test_File_Create(2);
	char* buf="Hello"; //30 chars Hello writing the second thing
	inum num=2;
	int offset = 0;
	int length = 6;
	if(!File_Write(num, offset, length, (void *) buf)){
		printf("WROTE %s with length %d\n", buf, strlen(buf));
	    char bufR[length];
	    if(!File_Read(num, 0, length, (void *) bufR)){
	    	//printf("Read done %s \n", (char*)buf);
	    	if(strcmp(buf,bufR)!=0){
				printf("Fail:  write string %s does not match read string %s \n",buf,bufR);
			}else{
				printf("**************Success    test 2F pass*******************************\n");
			}
	    }
	}
}

void test3F(){
	printf("*******************File layer test 3F simple big write and read ******************************\n");
	int num=1;
	Test_File_Create(num);
	int size=BLOCK_SIZE*6;// -1; //worked up to at least 10   
	char buf[size];
	for(int i=0;i<(size);i++){
		buf[i]='a'+i%26;
	}
	buf[size-1] = '\0';
	

	printf("\t About to write: %s  =======\n", buf);
	if(!File_Write(num, 0, size, buf)){
	    char bufR[size];
	    if(!File_Read(num, 0, size, bufR)){
	    	if(strcmp(buf,bufR)!=0){
				printf("Fail:  write string %s does not match read string \n", bufR);
			}else{
				printf("**************Success    test 3F pass*******************************\n");
			}
	    }
	}
}

void test4F(){
	printf("*******************File layer test 4F overwrite  ******************************\n");
	char *buf= "aaaaaaaaa";//"First write, welcome to CSC 545 OS class"; //40
	inum num=4;
	int offset=1;
	Test_File_Create(num);
	if(!File_Write(num, 0, 10, (void*)buf)){
		char *bufchange= "bb"; //"Second write LFS, rewrite string........"; //40
		File_Write(num, 0, 3, (void*)bufchange);
	    char bufR[10];
	    if(!File_Read(num, 0, 10, (void*)bufR)){
	    	char *bufresult = "bbaaaaaaa";
	    	if(strcmp(bufresult,bufR)!=0){
				printf("Fail test 6:  read string %s does not match expected read string %s\n",bufR, bufresult);
			}else{
				printf("**************Success    test 4F  pass*******************************\n");
			}
	    }
	}

}

void test5F(){
	printf("*******************File layer test 5F concat  ******************************\n");
	char *buf= "hello";
	inum num=5;
	int offset=1;
	Test_File_Create(num);
	if(!File_Write(num, 0, 6, (void*)buf)){
		char *bufchange= " world"; 
		offset = 5;
		File_Write(num, offset, 7, (void*)bufchange);
	    char bufR[12];
	    if(!File_Read(num, 0, 12, (void*)bufR)){
	    	char *bufresult = "hello world";
	    	if(strcmp(bufresult,bufR)!=0){
				printf("Fail test 5F:  read string %s does not match expected read string %s\n", bufR, bufresult);
			}else{
				printf("**************Success    test 5F  pass*******************************\n");
			}
	    }
	}

}

void test6F(){
	printf("*******************File layer test 6F offset read  ******************************\n");
	char *buf="hello world, this is a test of a very long offset read that covers two blocks."; //78
	int length = 79;
	inum num=6;
	Test_File_Create(num);
	int nooffset = 0;
	if(!File_Write(num, nooffset, length, (void*)buf)){
		int offset = 13;
	    char bufR[length-offset];
	    char *expect="world, this is a test of a very long offset read that covers two blocks.";
	    if(!File_Read(num, offset, length-offset, (void*)bufR)){
	    	if(strcmp(expect,bufR)!=0){
				printf("Fail test 6:  read string %s does not match expect string %s \n",bufR, expect);
			}else{
				printf("**************Success    test 6F  pass*******************************\n");
			}
	    }
	}
}

void test7F() {
	printf("*******************File layer test 7F dead block  ******************************\n");
	char *buf= "aaaaaaaa";//"First write, welcome to CSC 545 OS class"; //40
	int num=1;
	int offset=0;
	Test_File_Create(num);
	Test_File_Create(num+1);
	Test_File_Write(num);
	Test_File_Write(num+1);
	Show_Ifile_Contents();
	struct logAddress logAddOld = IfileArray.data[num].Block1Ptr;
	printf("old block %d seg %d\n", logAddOld.blockNo, logAddOld.segmentNo);
	struct logAddress logAddNew;
	for (int i=1; i<28; i++) {
		File_Write(num, 0, 12, (void*) "bbbbbbbbbbb");
		// Log_Write(num, 1, 12, (void*)"bbbbbbbbbbb", &logAddNew);
		// if (logAddOld.segmentNo != logAddNew.segmentNo) {
		// 	Log_writeDeadBlock(num, logAddOld, logAddNew);
		// 	printf("Dead blocks written -- WORKS WHEN USING A NEW FUSE (rm FuseFileSystem first)\n");
		// }
	}
	printf("new block %d seg %d\n", logAddNew.blockNo, logAddNew.segmentNo);
	//Show_Ifile_Contents();
}

void test8F() {
	printf("**************File layer test 8F checkpoint ************\n");
	
	int num=IfileArray.data.size();
	File_Create(num, TYPE_F);
	char* buffer = "test checkpoint";
	File_Write(num, 0, 16, (void*)buffer);
	Change_Permissions(1, 0700);
	char* directory = "dir/";
	char* filename = "fname";
	struct stat* stbuf;
	File_Naming(num, directory, filename, stbuf);
	Show_Ifile_Contents();
	Print_Inode(num);
 	WriteIfle();
	Log_destroy();

	return;
}

void test9F(){
	printf("***********Test 9F: File_Naming ************\n");
	File_Create(1, TYPE_F);
	Show_Ifile_Contents();
	char* directory = "dir/";
	char* filename = "fname";
	struct stat* stbuf;
	File_Naming(1, directory, filename, stbuf);
	Show_Ifile_Contents();
	if (!strcmp(IfileArray.data[1].filename, filename)) {
		printf("Success: filename is now %s\n", IfileArray.data[1].filename );
	} else {
		printf("Error: File_Naming didn't change the filename.\n");
	}

	return;
}

void test10F() {
	printf("***********Test 10F: File_Get ************\n");
	Inode in;
	File_Get(1, &in);
	printf("%d %s %c %c  %7d  %s %s\n", in.permissions, in.nlink, in.owner, in.group, in.size, in.mtime, in.filename);
	Print_Inode(1);
	printf("^^^^ If match, then File_Get works properly\n");
	return;
}


// cleanning and recovery test
// void test4Destroy()
// {
// 	Test_File_Creat(1);
// 	Test_File_Write(1);
// 	WriteIfle();
// 	//Log_CheckPoint(&oldAdrress, &newAdress, 1, 1);

// 	// cleaning test
// 	int num = 1;
// 	logAddress oldAddress, newAdress;
// 	int count = 0;
// 	for (auto it : segmentUsageTable)
// 	{
// 		oldAddress.segmentNo = it.first;
// 		for (int i = 0; i < it.second.blocksByteSize; i++)
// 		{
// 			pair<int, int> tmp = it.second.blocksByte[i];
// 			oldAddress.blockNo = tmp.first;
// 			if (tmp.first % 10 != 0)
// 				Log_writeDeadBlock(num, oldAddress, oldAddress);
// 		}
// 		count++;
// 		if (count > 2)
// 			break;
// 	}
// 	//char buf1[50] = "Hello LFS, welcome to CSC 545 OS classa";
// 	Test_File_Create(2);
// 	Test_File_Write(2);
// 	test2(1, address, buf1);
// 	Log_CheckPoint(&oldAdrress, &newAdress, 1, 1);
// }


void test5Destroy(){
	File_Create(1, 1);
	File_Write(1, 0, 5, (void*) "katy");
	File_Destroy();
	return;
}

void test6Destroy(){
	char buf[5];
	File_Read(1, 0, 5, buf);
	printf("After destroy content %s\n", buf);
	return;
}

void TestPermissions() {
	int inum = 3;
	Change_Permissions(inum, 0700);
	printf("per own grp sz  time \n");
	Print_Inode(inum);

	struct Inode inode = IfileArray.data[inum];//Get_Inode(inum);

	return;
}

void TestOwner() {
	int inum = 3;
	uid_t owner = getuid();
	Change_Owner(inum, owner);
	printf("per own grp sz  time \n");
	Print_Inode(inum);

	struct Inode inode = IfileArray.data[inum];//Get_Inode(inum);
	if (inode.owner != owner) {
		printf("Fail: old owner was not changed (old: %c, new: %c).\n ", inode.owner, owner);
	} else {
		printf("************* Success   TestOwner pass (%c == %c) **********\n", inode.owner, owner);
	}
	return;
}

void TestGroup() {
	int inum = 3;
	gid_t group = getgid();
	Change_Group(inum, group);
	printf("per own grp sz  time \n");
	Print_Inode(inum);

	struct Inode inode = IfileArray.data[inum]; //Get_Inode(inum);
	if (inode.group != group) {
		printf("Fail: old group was not changed (old: %c, new: %c).\n ", inode.group, group);
	} else {
		printf("************* Success   TestOwner pass (%c == %c) **********\n", inode.group, group);
	}
	return;
}


void test10(){
  //  initFile(4);
    struct IfileWrite writeTest;
    test8F();
    int i;
    for( i=0;i<IfileArray.data.size()&&i<6;i++){
    	writeTest.data[i]=IfileArray.data[i];

    }
   writeTest.size=i;
   logAddress newAdd;
   Log_Write(IFILE_INUM, 0, BLOCK_SIZE-100, &writeTest, &newAdd);
   Log_CheckPoint(&newAdd,&newAdd,1,3);
   char ifilecontents[BLOCK_SIZE-100];
   memset(ifilecontents,0,BLOCK_SIZE-100);
   Log_read(newAdd, BLOCK_SIZE-100, ifilecontents);
   cout<<" ifle  segmentNo="<<newAdd.segmentNo<<"  blockNo="<<newAdd.blockNo<<endl;

   struct IfileWrite *tmp=(struct IfileWrite *)ifilecontents;
   for(int i=0;i<tmp->size;i++){
   	  cout<<"read inode "<<i<<"  group "<<tmp->data[i].group<<" inum="<<tmp->data[i].inum<<endl;
   }
   	Log_destroy();
}

void test11(){

	init("FuseFileSystem",4);  
	logAddress newAdd;
	newAdd.segmentNo=1;
	newAdd.blockNo=4;
   char ifilecontents[BLOCK_SIZE-100];
   memset(ifilecontents,0,BLOCK_SIZE-100);
   Log_read(newAdd, BLOCK_SIZE-100, ifilecontents);
   cout<<" ifle  segmentNo="<<newAdd.segmentNo<<"  blockNo="<<newAdd.blockNo<<endl;

   struct IfileWrite *tmp=(struct IfileWrite *)ifilecontents;
   for(int i=0;i<4;i++){
   	  cout<<"read inode "<<i<<"  group "<<tmp->data[i].group<<" inum="<<tmp->data[i].inum<<endl;
   }
}

void test12(){
	File_Create(1, TYPE_F);
	struct Inode inode = IfileArray.data[1];
	printf("Got Inode %d \n", inode.inum);
	Print_Inode(1);
	inode.permissions = S_IFREG | 0774;; 
	struct stat s;
	convertInodeToStat(1, &s);
	printf("Inode has been converted.\n");
	Print_Inode(1);
	return;
}
void simple1(){
	struct logAddress oldAddress, newAddress;
	Log_CheckPoint(&oldAddress, &newAddress, 1,1);
	struct logAddress *newone = (struct logAddress *)malloc(sizeof(logAddress)*2);
	Log_CheckPoint(&newAddress, newone, 1, 2);
	cout<<"before checkpoint size "<<Log_GetIfleAddress(newone, 1)<<endl;
}

void simple2(){
	struct logAddress oldAddress, newAddress;
	logAddress *newone=(struct logAddress *)malloc(sizeof(logAddress)*3);
	cout<<"checkpoint size "<<Log_GetIfleAddress(newone, 1)<<endl;

}

void readInIfile() {
	int size = 4;
	int ifilesize = initFile(size);
	printf("***** Recovered contents ifilesize=%d *******\n", ifilesize);
	printf("inum  permis    type  path/filename \n");
	for (int i=0; i<IfileArray.data.size(); i++) {
		struct Inode in = IfileArray.data[i];
		printf("%d=%d: %5d   %d | %-4s  %-4s \n", i, in.inum, in.permissions, in.type, in.path, in.filename);
	}
}