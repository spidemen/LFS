#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <python2.7/Python.h>
#include <flash.h>
#include "log.h"
#include "file.h"
#include "directory.h"

int totalsectors = 2;

/* Convert LFS data structures into JSON */
int convertToJSON( void *buffer ) {

	printf("convertToJSON\n");
	printf("%s\n", (char *)buffer);
	return 0;
}


int main(int argc, char *argv[])
{
	PyObject *pName, *pModule, *pDict, *pFunc;
	PyObject *pArgs, *pValue;
	Py_Initialize();
 	PyRun_SimpleString("from time import time,ctime\n"
                     "print 'Today is',ctime(time())\n");

 	// Import the JsonParser as Python module
 	pName = PyString_FromString("parseJson");
 	pModule = PyImport_Import(pName);
 	//Create a dicitonary for the contents of the module
 	pDict = PyModule_GetDict(pModule);
 	//Get tehe add method from the dictionary
 	/*pFunc = PyDict_GetItemString(pDict, "add");
 	//Create tuple for the args for add()
 	pArgs = PyTuple_New(2); 
 	//Convert 2 to a python int
 	pValue = PyInt_FromLong(2);
 	// Set the python int as the 1st and 2nd args 
 	PyTuple_SetItem(pArgs,0,pValue);
 	PyTuple_SetItem(pArgs,1,pValue);

 	//Call the fucntion with the args
 	PyObject *pResult = PyObject_CallObject(pFunc,pArgs);
 	if (pResult==NULL){
 		printf("Calling the add method failed.\n");
 	}
 	long result = PyInt_AsLong(pResult);
  	Py_Finalize();
  	printf("The result is %d.\n", result);*/
  

	if ( argc != 2 ) /* argc should be 2 for correct execution */
    {
        /* We print argv[0] assuming it is the program name */
        printf( "Provide filename: %s <filename>", argv[0]);
    }
    else 
    {
        // We assume argv[1] is a filename to open
        //FILE *file = fopen( argv[1], "r" );
        char* file = argv[1];
        u_int blocks;
        Flash f = Flash_Open(file, FLASH_ASYNC, &blocks);
        /* fopen returns 0, the NULL pointer, on failure */
        if ( f == NULL )
        {
            printf( "Could not open file\n" );
        }
        else 
        {
        	printf("Blocks: %d, BLOCK_SIZE is %d\n", blocks, BLOCK_SIZE);
        	// printf("FLASH_SECTORS_PER_BLOCK %d\n", FLASH_SECTORS_PER_BLOCK);
        	// printf("FLASH_SECTOR_SIZE %d\n", FLASH_SECTOR_SIZE);
        	// printf("So FLASH_BLOCK_SIZE is %d\n", FLASH_BLOCK_SIZE);
        	u_int sector = 0;
        	u_int count = 1;
     

 			// * Flash_Read reads "count" sectors from "flash" into "buffer" 
 			// * starting at sector "sector".
 			blocks = 5;
        	for (int i=0; i<blocks; i++) {
        		sector = i * FLASH_SECTORS_PER_BLOCK;
        		void* buffer;
        		char buf[FLASH_SECTOR_SIZE];
        		if (Flash_Read(f, sector, count, buf)) 
	            {
	            	printf("Error with Flash_Read\n");
	            	return 1;
	            } 
	            else 
	            {
	            	
	            	//cout <<"Buffer contents"<<buffer<<endl;
	            	SegmentSummary *summary = (SegmentSummary *)buf;
	            	cout <<"SegmentSummary segmentNo = "<< summary->segmentNo<<endl;
	            	cout <<"SegmentSummary inUse = "<<summary->inUse<<endl;
	            	cout <<" modifiedTime = "<<summary->modifiedTime<<endl;

	    			
	            	//convertToJSON( buffer );
	            }
        	}
            
            
           Flash_Close(f);
        }
    }
    return 0;
} 


/*
        	logAddress logAddress1;
        	logAddress1.segmentNo = 1;
        	logAddress1.blockNo = 3;
        	u_int length = 50;
        	char buffer[50];
        	int flag;
        	flag = Log_read(logAddress1, length, (void *)buffer);
        	if (flag) {
        		cout << "Error: log_read" <<endl;
        	}
        	cout <<"read from flash: "<<buffer<<endl;
        	flag = Log_read(logAddress1, length, (void *)buffer);
        	if (flag) {
        		cout << "Error: 2 log_read" <<endl;
        	}
        	cout <<"2 read from flash: "<<buffer<<endl;
*/
	