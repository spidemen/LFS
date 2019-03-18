#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
//#include <Python.h>
#include <flash.h>
#include "log.h"
#include "file.h"
#include  "directory.h"


/* Convert LFS data structures into JSON */
int convertToJSON( void *buffer ) {

	printf("convertToJSON\n");
	printf("%s\n", (char *)buffer);
	return 0;
}


int main(int argc, char *argv[])
{


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
        Flash f = Flash_Open(file, FLASH_SILENT, &blocks);
        /* fopen returns 0, the NULL pointer, on failure */
        if ( f == NULL )
        {
            printf( "Could not open file\n" );
        }
        else 
        {
        	u_int sector = 0;
        	u_int count = 10;
        	char buffer[BLOCK_SIZE];


            if (Flash_Read(f, sector, 2, (void *) buffer)) 
            {
            	printf("Error with Flash_Read\n");
            	return 1;
            } 
            else 
            {
            	
    	
            	printf("Buffer contents: %s\n", (char *) buffer);
            	convertToJSON( buffer );
            }
            
           Flash_Close(f);
        }
    }
    return 0;
} 
	