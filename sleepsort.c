#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
  pid_t pid;
  int status;
  
  while(--argc > 1 && !(pid = fork()));
  
  int val = atoi(argv[argc]);
  sleep(val);
  
  printf("%d\n", val);
  waitpid(pid, &status, 0);
  return 0;
}
