#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

typedef struct ancestry {
  pid_t ancestors[10];
  pid_t siblings[100];
  pid_t children[100];
} ancestry;

// These values MUST match the unistd_32.h modifications:
//#define __NR_cs3013_syscall1 377
#define __NR_cs3013_syscall2 378
//#define __NR_cs3013_syscall3 379

long testCall2 (unsigned short* num, ancestry* a) {

  unsigned short rca = (unsigned short) fork();
  //unsigned short rcb = (unsigned short) fork();
  //unsigned short rc = (unsigned short) fork();


  if (rca < 0) {
    fprintf(stderr, "fork failed\n");
    return -1;
  } else if (rca == 0) { // child (new process)
    while(1);
  } else {
    *num = 1226;
    printf("%d\n", rca);
    return (long) syscall(__NR_cs3013_syscall2, num, a);
  }
}

int main () {

  unsigned short* pid = (unsigned short*) malloc(sizeof(unsigned short));
  ancestry* info = (ancestry*)malloc(sizeof(ancestry));
  //*pid = 1;

  printf("The return values of the system calls are:\n");

  //printf("\tcs3013_syscall1: %ld\n", testCall1());
  //printf("ey\n");
  printf("\tcs3013_syscall2: %ld\n", testCall2(pid, info));
  printf("The first parent:, %d\n", info->ancestors[0]);
  printf("The second parent:, %d\n", info->ancestors[1]);
  printf("The third parent:, %d\n", info->ancestors[2]);

  //printf("\tcs3013_syscall3: %ld\n", testCall3());
  return 0;
}
