#include <log.h>

void test1(){
	cout<<"*******************Log layer test 1 simple small write and read ******************************"<<endl;
	char *buf="Hello LFS, welcome to CSC 545 OS class";
	inum num=1;
	logAddress address;
	if(!Log_Write(num, 1, 40,(void*)buf,address)){
		char bufR[10];
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
	if(!File_Write(num, 0, 40, buf)){
	    char bufR[10];
	    if(!File_Read(num, 0, 40, bufR)){
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
	int size=2048;   
	char buf[size];
	for(int i=0;i<size;i++){
		buf[i]='a'+i%26;
	}
	inum num=1;
	if(!File_Write(num, 0, size, buf)){
	    char bufR[size];
	    if(!File_Read(num, 0, size, bufR)){
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
	char *buf="Hello LFS, welcome to CSC 545 OS class";
	inum num=1;
	if(!File_Write(num, 0, 40, buf)){
		char *bufchange="Hello LFS, rewrite string.......";
		File_Write(num, 0, 40, bufchange);
	    char bufR[size];
	    if(!File_Read(num, 0, 40, bufR)){
	    	if(strcmp(bufchange,bufR)!=0){
				cout<<"Fail:  write string "<<buf<<" does not match read string "<<bufR<<endl;
			}else{
				cout<<"**************Success    test 6  pass*******************************"<<endl;
			}
	    }
	}

}
void test7(){
	cout<<"*******************File layer test 6 concat   ******************************"<<endl;
	char *buf="Hello LFS, welcome to CSC 545 OS class";
	inum num=1;
	if(!File_Write(num, 0, 40, buf)){
		char *bufchange="concat string, welcome to LFS project";
		File_Write(num, 40, 40, bufchange);
	    char bufR[80];
	    char *expect="Hello LFS, welcome to CSC 545 OS class concat string, welcome to LFS project"
	    if(!File_Read(num, 0, 80, bufR)){
	    	if(strcmp(expect,bufR)!=0){
				cout<<"Fail:  write string "<<buf<<" does not match read string "<<bufR<<endl;
			}else{
				cout<<"**************Success    test 7  pass*******************************"<<endl;
			}
	    }
	}
}
int main(int argc, char *argv[]){
	init("FuseFileSystem");   
	// log layer test cases
	test1();
	test2();
	test3();
	// file layer test case

	return 1;
}