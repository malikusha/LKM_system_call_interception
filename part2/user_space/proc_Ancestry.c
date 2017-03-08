#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

//ancestor struct to use for later
typedef struct ancestry {
  pid_t ancestors[10];
  pid_t siblings[100];
  pid_t children[100];
} ancestry;

//location on the syscall table
#define __NR_cs3013_syscall2 378

//run the testcall
long testCall2 (unsigned short* num, ancestry* a) {
  return (long) syscall(__NR_cs3013_syscall2, num, a);
}

int main (int argc, char* argv[]) {

  //set up and run the strtol for the first input to get hte desired pid
  char** endptr = (char**) malloc(sizeof(char*));
  unsigned short input_read = (unsigned short) strtol(argv[1], endptr, 10);

  //malloc the pointers to be passed in; set up values
  unsigned short* pid = (unsigned short*) malloc(sizeof(unsigned short));
  ancestry* info = (ancestry*)malloc(sizeof(ancestry));
  *pid = input_read;

  //run syscall_2 to get the geneaology of te pid
  printf("The return values of the system calls are:\n");
  printf("\tcs3013_syscall2: %ld\n", testCall2(pid, info));
  return 0;
}
