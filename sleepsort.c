#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int arc, char **argv)
{
  int status;
  
  while(--argc > 1 && !fork());
  
  int val = atoi(argv[argc]);
  sleep(val);
  
  printf("%d\n", val);
  wait(&status);
  return 0;
}
