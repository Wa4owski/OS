#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
  if(argc < 2){
    printf("Usage: sudo ./print_fpu <process_pid>\n");
    return -1;
  }

  int pid = atoi(argv[1]);

  if(!pid){
    printf("Invalid PID: %s\n", argv[1]);
    return -1;
  }

  FILE *fptr;

  fptr = fopen("/sys/kernel/debug/fpu/pid_here","w");

  if(fptr == NULL){
    printf("Error! pid_here not found or permissions denied.");
    return -1;
  }

  fprintf(fptr, "%d", pid);
  fclose(fptr);

  fptr = fopen("/sys/kernel/debug/fpu/fpu_output", "r");

  if(fptr == NULL){
    printf("Error! fpu_output not found or permissions denied.");
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
