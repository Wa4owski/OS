#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if(argc < 2){
		printf("Usage: ./print_inode <path to file>\n");
		return -1;
	}
	char *symlinkpath = argv[1];
	char *ptr;
	ptr = realpath(symlinkpath, NULL);
	if(!ptr){
		printf("Path %s is incorrect.\n", symlinkpath);
		return -1;
	}
	printf("%s\n", ptr);
	
	FILE *fptr;

	fptr = fopen("/sys/kernel/debug/inode/path_here", "w");

	if(fptr == NULL){
		printf("Error! path_here not found");
		return -1;
	}

	fprintf(fptr, "%s", ptr);
	fclose(fptr);

	fptr = fopen("/sys/kernel/debug/inode/inode_output", "r");

  if(fptr == NULL){
    printf("Error! fpu_output not found");
    return -1;
  }

  char c = fgetc(fptr);
  while (c != EOF){
    printf("%c", c);
    c = fgetc(fptr);
  }
  
  fclose(fptr);

   return 0;
}
