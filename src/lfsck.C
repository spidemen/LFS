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


int callPythonJson(PyObject *pDict, PyObject *pFunc, PyObject *pArgs, PyObject *pValue) {

	//Get the add method from the dictionary
 	pFunc = PyDict_GetItemString(pDict, "parseJsonFile");
 	if (pFunc == NULL) {
 		cout << "couldn't get parseJSON function" <<endl;
 	}
 	//Create tuple for the args for add()
 	pArgs = PyTuple_New(2); 
 	pValue = PyInt_FromLong(2);
 	PyTuple_SetItem(pArgs,0,pValue);
 	PyTuple_SetItem(pArgs,1,pValue);

 	PyObject *pResult = PyObject_CallObject(pFunc, pArgs);
 	 if (pResult==NULL){
 		printf("Calling the parseJSON method failed.\n");
 		return 1;
 	}

 	char* result = PyString_AsString(pResult);
  	
  	printf("Outcome of parseJSON: %s.\n", result);

}


/* Convert LFS data structures into JSON */
int convertToJSON( Block *b, FILE* jsonfile) {

	int STR_SIZE =  sizeof(*b);
	char blockString[STR_SIZE];
	sprintf(blockString, "{'blockNo' :' %d', 'aLive' : '%d', 'blockUse' : '%d', 'data' : '%s', 'offset' : '%d'}", b->blockNo, b->aLive, b->blockUse, b->data, b->offset);

	//cout << blockString << endl;
	fputs(blockString, jsonfile);
	
	return 0;
}


int main(int argc, char *argv[])
{
	PyObject *pName, *pModule, *pDict, *pFunc, *pArgs, *pValue, *pValue1, *pValue2;
	Py_Initialize();
 	PyRun_SimpleString("from time import time,ctime\n"
                     "print 'Today is',ctime(time())\n");
 	PyRun_SimpleString("import sys");
 	PyRun_SimpleString("import os");
 	PyRun_SimpleString("sys.path.append(os.getcwd())");

 	// Import the JsonParser as Python module
 	pName = PyString_FromString("parseJson");
 	if (pName == NULL) {
 		printf("Error parsing string\n");
 		return 1;
 	}

 	pModule = PyImport_Import(pName);
 	if (pModule == NULL) {
 		printf("Error with pModule and importing parseJson.py\n");
 		return 1;
 	}
 	Py_DECREF(pName);
 	//Create a dicitonary for the contents of the module
 	pDict = PyModule_GetDict(pModule);

 	// Test to check if python communication is working
  	//pythonTest(pDict, pFunc, pArgs, pValue1, pValue2);

	if ( argc != 2 ) /* argc should be 2 for correct execution */
    {
        //We print argv[0] assuming it is the program name 
        printf( "Provide filename: %s <filename>", argv[0]);
    }
    else 
    {
        // We assume argv[1] is a filename to open
        //FILE *file = fopen( argv[1], "r" );
        char* file = argv[1];
        u_int blocks;
        Flash f = Flash_Open(file, FLASH_ASYNC, &blocks);
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
     		FILE *jsonfile;
     		jsonfile = fopen("memory.json", "w");
     		fputs("{", jsonfile);

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
	            	if (summary->segmentNo != 0) {
	            		cout <<"SegmentSummary segmentNo = "<< summary->segmentNo<<endl;
	            		cout <<"SegmentSummary inUse = "<<summary->inUse<<endl;
	            		cout <<" modifiedTime = "<<summary->modifiedTime<<endl;
	            	}
	    			Block *d = (Block *)buf;
	    			cout << "Block no"<<d->blockNo << "| " <<d->aLive<< "| " << d->blockUse << "| "<<d->offset <<endl;
	            	convertToJSON( d , jsonfile);
	            }
        	}
        	
        	fputs("}", jsonfile);
        	fclose(jsonfile);

            callPythonJson( pDict, pFunc,  pArgs , pValue);
            
           	Flash_Close(f);
        }
    }

    Py_Finalize();
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
	