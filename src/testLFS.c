#include <stdio.h>
int main(int argc, char *argv[])
{
	FILE *f=fopen("temp/hello","w");
	if(f==NULL){
		printf("open error \n");
	}

	if(fprintf(f, "This is testing for fprintf...\n")<0)
	{
		printf("write error \n");
	}
	if(fclose(f)){
		printf("Fail to close \n");
	}
	return 1;
}