#include <fuse.h>
#include <stdio.h>
// #include <string.h>
#include <string>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <python2.7/Python.h>
#include <flash.h>
#include "log.h"
// #include "file.h"
#include "cfile.h"
#include "directory.h"

int totalsectors = 2;

/* Convert LFS data structures into JSON */
int convertToJSON( Block *b, FILE* jsonfile) {

	int STR_SIZE =  sizeof(*b);
	char blockString[STR_SIZE];
	sprintf(blockString, "{\"blockNo\" :\"%d\", \"aLive\" : \"%d\", \"offset\" : \"%d\"}", b->blockNo, b->aLive, b->offset);

	//cout << blockString << endl;
	fputs(blockString, jsonfile);
	
	return 0;
}

int convertInodeToJSON( Inode *i, FILE* jsonfile) {

    int STR_SIZE =  sizeof(*i);
    char inodeString[STR_SIZE];
    sprintf(inodeString, "{\"inum\" :\"%d\", \"type\" : \"%d\", \"size\" : \"%d\", \"in_use\" : \"%d\", \"path\" : \" %s \"}", i->inum, i->type, i->size, i->in_use, i->path);

    fputs(inodeString, jsonfile);
    
    return 0;
}



int main(int argc, char *argv[])
{


	if ( argc != 2 ) /* argc should be 2 for correct execution */
    {
        //We print argv[0] assuming it is the program name 
        printf( "Provide filename (./lfsck <filename>): '%s' is not a file\n", argv[0]);
    }
    else 
    {
        // We assume argv[1] is a filename to open
        //FILE *file = fopen( argv[1], "r" );
        int size = 4;
        initFile(size);
        char* file = argv[1];
        u_int blocks;
        Flash f = Flash_Open(file, FLASH_ASYNC, &blocks);
        printf("Blocks: %u\n", blocks);

        if ( f == NULL )
        {
            printf( "Could not open file\n" );
        }
        else 
        {
        	u_int sector = 0;
        	u_int count = 1;
     		FILE *jsonfile;
     		jsonfile = fopen("memory.json", "w");
     		fputs("[", jsonfile);

            int nodes = 3;
            for (int i=1; i<=nodes; i++) {
                struct Inode inode;
                File_Get(i, &inode);
                inode.in_use = 1;
                if (i==1) inode.in_use = 0;
                convertInodeToJSON(&inode, jsonfile);
                if (i<nodes) {
                        fputs(",", jsonfile);
                }
            }
        	
        	fputs("]", jsonfile);
        	fclose(jsonfile);
            
           	Flash_Close(f);
        }
        std::string filename = "parseJson.py";
        std::string command = "python ";
        command += filename;
        system(command.c_str());
    }


    return 0;
} 


	