#include <sys/syscall.h>
#include <stdio.h>

// These values MUST match the unistd_32.h modifications:
#define __NR_cs3013_syscall1 377

long testCall1 ( void) {
  //run syscall 1
return (long) syscall(__NR_cs3013_syscall1);
}

int main(){
  //create some char buffers
  char x[256], y[256];
  //test syscall 1
  printf("\tcs3013_syscall1: %ld\n", testCall1());
  //open both a virus file and non virus file
  FILE* virus = fopen("virus.txt", "r");
  FILE* not_virus = fopen("not_virus.txt", "r");
  //this should trigger the virus check
  fscanf(virus, "%s", x);
  fscanf(not_virus, "%s", y);
  //close les files
  fclose(virus);
  fclose(not_virus);
  return 0;
}
